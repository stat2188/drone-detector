#include "scanner_thread.hpp"

namespace drone_analyzer {

ScannerThread::ScannerThread(DroneScanner& scanner) noexcept
    : scanner_(scanner)
    , thread_(nullptr)
    , sweep_current_freq_(0) {
}

ScannerThread::~ScannerThread() noexcept {
    stop();
}

msg_t ScannerThread::static_fn(void* arg) {
    static_cast<ScannerThread*>(arg)->run();
    return 0;
}

void ScannerThread::run() noexcept {
    static RetuneMessage message{};

    while (!chThdShouldTerminate()) {
        if (__atomic_load_n(&scanning_, __ATOMIC_ACQUIRE)) {
            if (sweep_enabled_ && sweep_active_) {
                // Sweep pass: all 240 steps in a row (fast)
                for (uint16_t i = 0; i < 240; ++i) {
                    if (chThdShouldTerminate()) break;

                    radio::set_tuning_frequency(sweep_current_freq_);
                    message.freq = static_cast<int64_t>(sweep_current_freq_);
                    message.range = 0;
                    EventDispatcher::send_message(message);

                    sweep_current_freq_ += sweep_step_hz_;
                    sweep_current_step_++;
                    if (sweep_current_freq_ >= sweep_end_ || sweep_current_step_ >= 240) {
                        sweep_current_freq_ = sweep_start_;
                        sweep_current_step_ = 0;
                    }

                    chThdSleepMilliseconds(3);  // PLL stabilization only
                }

                sweep_active_ = false;
                db_scan_counter_ = 0;
            } else {
                // DB scan step
                ErrorCode err = scanner_.perform_scan_cycle();
                if (err == ErrorCode::SUCCESS) {
                    ErrorResult<FreqHz> freq_result = scanner_.get_current_frequency();
                    if (freq_result.has_value()) {
                        message.freq = static_cast<int64_t>(freq_result.value());
                        message.range = 0;
                        EventDispatcher::send_message(message);
                    }
                }

                // Auto-trigger sweep every N DB cycles
                if (sweep_enabled_) {
                    db_scan_counter_++;
                    if (db_scan_counter_ >= SWEEP_AUTO_INTERVAL) {
                        sweep_active_ = true;
                    }
                }
            }
        }

        chThdSleepMilliseconds(SCANNER_SLEEP_MS);
    }
}

void ScannerThread::trigger_sweep_pass() noexcept {
    sweep_active_ = true;
}

void ScannerThread::start() noexcept {
    if (thread_ == nullptr) {
        thread_ = chThdCreateStatic(
            wa_, sizeof(wa_),
            NORMALPRIO + 10,
            ScannerThread::static_fn, this
        );
    }
}

void ScannerThread::stop() noexcept {
    if (thread_ != nullptr) {
        __atomic_store_n(&scanning_, false, __ATOMIC_RELEASE);
        sweep_active_ = false;
        chThdTerminate(thread_);
        chThdWait(thread_);
        thread_ = nullptr;
    }
}

void ScannerThread::set_scanning(bool scanning) noexcept {
    __atomic_store_n(&scanning_, scanning, __ATOMIC_RELEASE);
}

bool ScannerThread::is_scanning() const noexcept {
    return __atomic_load_n(&scanning_, __ATOMIC_ACQUIRE);
}

bool ScannerThread::is_active() const noexcept {
    return thread_ != nullptr;
}

void ScannerThread::set_sweep_enabled(bool enabled) noexcept {
    sweep_enabled_ = enabled;
    db_scan_counter_ = 0;
}

void ScannerThread::set_sweep_range(FreqHz start, FreqHz end, FreqHz step) noexcept {
    sweep_start_ = start;
    sweep_end_ = end;
    sweep_step_hz_ = step;
    sweep_current_freq_ = start;
    sweep_current_step_ = 0;
}

} // namespace drone_analyzer
