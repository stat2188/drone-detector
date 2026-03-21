#include "scanner_thread.hpp"
#include "radio.hpp"
#include "receiver_model.hpp"

namespace drone_analyzer {

ScannerThread::ScannerThread(DroneScanner& scanner) noexcept
    : scanner_(scanner)
    , thread_(nullptr)
    , sweep_current_freq_(sweep_start_) {
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
        if (scanning_) {
            if (sweep_enabled_ && step_is_sweep_) {
                // Sweep step: tune to sweep frequency
                do_sweep_step();
                message.freq = static_cast<int64_t>(sweep_current_freq_);
                message.range = 0;
                EventDispatcher::send_message(message);
            } else {
                // DB scan step: normal scanner operation
                ErrorCode err = scanner_.perform_scan_cycle();
                if (err == ErrorCode::SUCCESS) {
                    ErrorResult<FreqHz> freq_result = scanner_.get_current_frequency();
                    if (freq_result.has_value()) {
                        message.freq = static_cast<int64_t>(freq_result.value());
                        message.range = 0;
                        EventDispatcher::send_message(message);
                    }
                }
            }

            // Toggle step type for next iteration
            if (sweep_enabled_) {
                step_is_sweep_ = !step_is_sweep_;
            }
        }

        chThdSleepMilliseconds(SCANNER_SLEEP_MS);
    }
}

void ScannerThread::do_sweep_step() noexcept {
    radio::set_tuning_frequency(sweep_current_freq_);

    // Advance to next sweep frequency
    sweep_current_freq_ += sweep_step_hz_;
    sweep_current_step_++;

    // Wrap around when sweep is complete
    if (sweep_current_freq_ >= sweep_end_ || sweep_current_step_ >= 240) {
        sweep_current_freq_ = sweep_start_;
        sweep_current_step_ = 0;
    }
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
        scanning_ = false;
        chThdTerminate(thread_);
        chThdWait(thread_);
        thread_ = nullptr;
    }
}

void ScannerThread::set_scanning(bool scanning) noexcept {
    scanning_ = scanning;
    if (scanning && sweep_enabled_) {
        // Reset sweep to start when (re)starting scan
        sweep_current_freq_ = sweep_start_;
        sweep_current_step_ = 0;
        step_is_sweep_ = false;  // First step is always DB
    }
}

bool ScannerThread::is_scanning() const noexcept {
    return scanning_;
}

bool ScannerThread::is_active() const noexcept {
    return thread_ != nullptr;
}

void ScannerThread::set_sweep_enabled(bool enabled) noexcept {
    sweep_enabled_ = enabled;
    if (enabled) {
        sweep_current_freq_ = sweep_start_;
        sweep_current_step_ = 0;
    }
}

void ScannerThread::set_sweep_range(FreqHz start, FreqHz end, FreqHz step) noexcept {
    sweep_start_ = start;
    sweep_end_ = end;
    sweep_step_hz_ = step;
    sweep_current_freq_ = start;
    sweep_current_step_ = 0;
}

} // namespace drone_analyzer
