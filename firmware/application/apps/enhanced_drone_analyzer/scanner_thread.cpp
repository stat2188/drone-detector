#include "scanner_thread.hpp"
#include "portapack.hpp"

namespace drone_analyzer {

ScannerThread::ScannerThread(DroneScanner& scanner) noexcept
    : scanner_(scanner), thread_(nullptr) {
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
            // CRITICAL FIX: Perform scan cycle FIRST, then get frequency
            // Previous code captured frequency BEFORE perform_scan_cycle(),
            // causing RetuneMessage to contain stale (old) frequency
            // while spectrum data was captured AFTER tuning to new frequency.
            // This created a permanent 1-cycle offset causing database
            // tracking errors and overflow on 5th frequency.
            //
            // Timeline of bug:
            //   1. freq_before = get_current_frequency() → gets F[i]
            //   2. perform_scan_cycle() → updates to F[i+1], tunes hardware to F[i+1]
            //   3. RetuneMessage sent with F[i] (WRONG - frequency is stale!)
            //   4. Hardware captures spectrum at F[i+1]
            //   5. UI receives spectrum but thinks it's from F[i]
            //   6. process_spectrum_message() associates spectrum with wrong frequency
            //   7. Tracked drones accumulate at incorrect frequencies
            //   8. By 5th cycle, accumulated error causes database wraparound/overflow
            //
            // Fixed timeline:
            //   1. perform_scan_cycle() → updates to F[i+1], tunes hardware to F[i+1]
            //   2. freq_after = get_current_frequency() → returns F[i+1] (actual tuned frequency)
            //   3. RetuneMessage sent with F[i+1] (CORRECT - matches hardware state)
            //   4. Hardware captures spectrum at F[i+1]
            //   5. UI receives spectrum and knows it's from F[i+1]
            //   6. process_spectrum_message() correctly associates spectrum with F[i+1]
            //   7. No frequency offset, no accumulation, no overflow
            ErrorCode err = scanner_.perform_scan_cycle();

            // Get frequency AFTER scan cycle (when radio is already tuned to new frequency)
            // perform_scan_cycle_internal() guarantees current_frequency_ is updated on SUCCESS:
            //   - Line 479: freq_result = database_.get_next_frequency(current_frequency_)
            //   - Line 482: current_frequency_ = freq_result.value()
            //   - Line 513: hardware_.tune_to_frequency(current_frequency_)
            //   - Line 531-532: return ErrorCode::SUCCESS (only if tuning succeeded)
            ErrorResult<FreqHz> freq_result = scanner_.get_current_frequency();
            if (err == ErrorCode::SUCCESS && freq_result.has_value()) {
                message.freq = static_cast<int64_t>(freq_result.value());
                message.range = 0;
                EventDispatcher::send_message(message);
            }

            // Sleep during dwell — use scan_interval_ms from settings (default 100ms)
            // Normal scanning: no sleep, runs at full hardware speed
            if (scanner_.is_dwelling()) {
                const ScanConfig cfg = scanner_.get_config();
                const uint32_t dwell_sleep = (cfg.scan_interval_ms > 0) ? cfg.scan_interval_ms : SCANNER_SLEEP_MS;
                chThdSleepMilliseconds(dwell_sleep);
            }
        }
        chThdSleepMilliseconds(1);  // Yield to RTOS when not scanning
    }
}

void ScannerThread::start() noexcept {
    if (thread_ == nullptr) {
        thread_ = chThdCreateStatic(
            wa_, sizeof(wa_), NORMALPRIO + 10,
            ScannerThread::static_fn, this);
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

}  // namespace drone_analyzer
