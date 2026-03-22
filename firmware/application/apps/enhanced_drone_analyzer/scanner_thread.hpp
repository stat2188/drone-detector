#ifndef SCANNER_THREAD_HPP
#define SCANNER_THREAD_HPP

#include <cstdint>
#include <cstddef>
#include "ch.h"
#include "scanner.hpp"
#include "message.hpp"
#include "event_m0.hpp"
#include "constants.hpp"
#include "radio.hpp"

namespace drone_analyzer {

class ScannerThread {
public:
    explicit ScannerThread(DroneScanner& scanner) noexcept;
    ~ScannerThread() noexcept;

    ScannerThread(const ScannerThread&) = delete;
    ScannerThread& operator=(const ScannerThread&) = delete;
    ScannerThread(ScannerThread&&) = delete;
    ScannerThread& operator=(ScannerThread&&) = delete;

    void start() noexcept;
    void stop() noexcept;

    void set_scanning(bool scanning) noexcept;
    [[nodiscard]] bool is_scanning() const noexcept;
    [[nodiscard]] bool is_active() const noexcept;

    void set_sweep_enabled(bool enabled) noexcept;
    void set_sweep_range(FreqHz start, FreqHz end, FreqHz step) noexcept;
    void trigger_sweep_pass() noexcept;

private:
    static constexpr size_t STACK_WORDS = THD_WA_SIZE(SCANNER_THREAD_STACK_SIZE) / sizeof(stkalign_t);

    static msg_t static_fn(void* arg);
    void run() noexcept;

    DroneScanner& scanner_;
    Thread* thread_{nullptr};
    stkalign_t wa_[STACK_WORDS];

    bool scanning_{false};

    // Batch sweep: 240 steps in a row, auto-triggers every N frequencies
    bool sweep_enabled_{false};
    bool sweep_active_{false};
    static constexpr uint16_t SWEEP_AUTO_INTERVAL = 20;  // sweep every 20 frequencies
    uint16_t db_scan_counter_{0};
    FreqHz sweep_start_{0};
    FreqHz sweep_end_{0};
    FreqHz sweep_step_hz_{0};
    FreqHz sweep_current_freq_{0};
    uint16_t sweep_total_steps_{240};  // dynamic: range / SWEEP_SLICE_BW
    uint16_t sweep_current_step_{0};

    // Dwell: stay on frequency when signal detected
    uint8_t dwell_cycles_{0};
};

} // namespace drone_analyzer

#endif // SCANNER_THREAD_HPP
