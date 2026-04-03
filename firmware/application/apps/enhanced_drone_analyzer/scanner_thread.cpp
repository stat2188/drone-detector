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
            // Read config once per iteration (get_config locks mutex internally)
            const ScanConfig cfg = scanner_.get_config();
            const uint32_t sleep_ms = (cfg.scan_interval_ms > 0) ? cfg.scan_interval_ms : SCANNER_SLEEP_MS;

            // Force-resume: consume flag BEFORE dwell logic.
            // force_resume_scanning() sets flag from scanner thread,
            // but perform_scan_cycle_internal() is never called during LOCKING.
            // We must consume the flag here so the state transitions back to SCANNING.
            if (scanner_.try_consume_force_resume_flag()) {
                dwell_cycles_ = 0;
            }

            // Dwell: stay on frequency when signal detected (if enabled)
            if (cfg.dwell_enabled) {
                const ScannerState scan_state = scanner_.get_state();
                if (scan_state == ScannerState::LOCKING || scan_state == ScannerState::TRACKING) {
                    dwell_cycles_++;
                    // Increase dwell time when confirm count is enabled
                    // so scanner has time to make 2 confirmations
                    const uint8_t max_dwell = cfg.confirm_count_enabled ? (MAX_DWELL_CYCLES * 2) : MAX_DWELL_CYCLES;
                    if (dwell_cycles_ >= max_dwell) {
                        // Max dwell reached — force resume scanning
                        if (cfg.noise_blacklist_enabled && __atomic_load_n(&scanning_, __ATOMIC_ACQUIRE)) {
                            const FreqHz locked_freq = scanner_.get_locked_frequency();
                            scanner_.increment_noise_count(locked_freq);
                            scanner_.remove_drone_on_frequency(locked_freq);
                        }
                        scanner_.force_resume_scanning();
                        dwell_cycles_ = 0;
                    }
                    chThdSleepMilliseconds(sleep_ms);
                    continue;
                }
                dwell_cycles_ = 0;
            } else {
                // Dwell disabled: still respect LOCKING/TRACKING state.
                // Scanner should hold frequency while UI thread processes the signal.
                // force_resume_scanning() sets flag which perform_scan_cycle() checks
                // to transition back to SCANNING state.
                const ScannerState scan_state = scanner_.get_state();
                if (scan_state == ScannerState::LOCKING || scan_state == ScannerState::TRACKING) {
                    dwell_cycles_++;
                    // If confirm count is enabled, dwell longer to allow 2 confirmations
                    const uint8_t max_dwell = cfg.confirm_count_enabled 
                        ? (MAX_DWELL_CYCLES + cfg.confirm_count) 
                        : MAX_DWELL_CYCLES;
                    if (dwell_cycles_ >= max_dwell) {
                        scanner_.force_resume_scanning();
                        dwell_cycles_ = 0;
                    }
                    chThdSleepMilliseconds(sleep_ms);
                    continue;
                }
                dwell_cycles_ = 0;
            }

            // Capture frequency BEFORE scan cycle — this is the frequency the hardware
            // is CURRENTLY tuned to. The spectrum data arriving from the baseband was
            // captured at this frequency. After perform_scan_cycle, current_frequency_
            // will point to the NEXT frequency (already tuned).
            ErrorResult<FreqHz> freq_before = scanner_.get_current_frequency();

            ErrorCode err = scanner_.perform_scan_cycle();
            if (err == ErrorCode::SUCCESS && freq_before.has_value()) {
                message.freq = static_cast<int64_t>(freq_before.value());
                message.range = 0;
                EventDispatcher::send_message(message);
            }
        }
        chThdSleepMilliseconds(SCANNER_SLEEP_MS);
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
