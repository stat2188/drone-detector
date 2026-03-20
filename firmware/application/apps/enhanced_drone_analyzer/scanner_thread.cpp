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
    RetuneMessage message{};

    while (!chThdShouldTerminate()) {
        if (scanning_) {
            ErrorCode err = scanner_.perform_scan_cycle();

            if (err == ErrorCode::SUCCESS) {
                ErrorResult<FreqHz> freq_result = scanner_.get_current_frequency();
                if (freq_result.has_value()) {
                    message.freq = freq_result.value();
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
        scanning_ = false;
        chThdTerminate(thread_);
        chThdWait(thread_);
        thread_ = nullptr;
    }
}

void ScannerThread::set_scanning(bool scanning) noexcept {
    scanning_ = scanning;
}

bool ScannerThread::is_scanning() const noexcept {
    return scanning_;
}

bool ScannerThread::is_active() const noexcept {
    return thread_ != nullptr;
}

} // namespace drone_analyzer
