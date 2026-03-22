#include "scanner_thread.hpp"

namespace drone_analyzer {

ScannerThread::ScannerThread(DroneScanner& scanner) noexcept
    : scanner_(scanner)
    , thread_(nullptr) {
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

        chThdSleepMilliseconds(SCANNER_SLEEP_MS);
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
        __atomic_store_n(&scanning_, false, __ATOMIC_RELEASE);
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

void ScannerThread::do_sweep_step() noexcept {
    // Sweep is now handled by UI thread (Looking Glass pattern).
    // This method is retained for compatibility but not used.
}

} // namespace drone_analyzer
