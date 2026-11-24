#ifndef GATE_ACTIVITY_HPP
#define GATE_ACTIVITY_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include <string_view>

#include "constants.hpp"
#include "drone_types.hpp"
#include "ui_widget.hpp"
#include "ui_font_fixed_5x8.hpp"

namespace drone_analyzer {

/**
 * @brief Bitmask slots for the shape-chain gates this module visualizes.
 *
 * BIT POSITIONS are display slots (bit0..bit11); the ENUM NAMES carry the
 * detector's own step numbering (EDA README §14 chain,
 * scanner.cpp::apply_shape_filters) so the two can be diffed line by line:
 *
 *   slot  scanner step         gate
 *   ----  -------------  ------------------------------------------
 *    0    Step 3  Margin        peak must clear the entry gate
 *    1    Step 5  Min Width     minimum emission width in bins
 *    2    Step 6  Max Width     maximum emission width in bins
 *    3    Step 7  Sharpness     inverted-V: peak_margin*100/avg_margin
 *    4    Step 8  Peak Ratio    the RAT setting: margin*10/width
 *    5    Step 9  Valley Depth  flanks must stay BELOW the depth
 *    6    Step 10 Flatness      flat-top share must stay <= the limit
 *    7    Step 11 Symmetry      min/max flank width >= the limit
 *    8    Step 13 Neighbor      ┐ Steps 13-15 are post-filter trackers
 *    9    Step 15 RSSI-var      │ (multi-frame state the UI thread has
 *   10    Step 14 Mahalanobis   │ none of); Step 12 is in-frame but needs
 *   11    Step 12 Kurtosis      ┘ its own scratch pass — none of the four
 *                                are evaluated here (see
 *                                evaluate_candidate_gates).
 *
 * Step 4 is the width MEASUREMENT shared by Steps 5-11, not a gate — it has
 * no slot of its own. Steps 13-15 run outside apply_shape_filters entirely.
 *
 * @note The underlying type MUST stay wide enough for all twelve bits. It
 *       was `uint8_t`: the four post-filter enumerators (1u<<8 .. 1u<<11)
 *       did not fit, and `gates_passed_mask |= static_cast<uint8_t>(...)`
 *       narrowed them to 0 — a silent no-op for every one of those steps.
 *       Both this enum and GateVerdict::gates_passed_mask are 16-bit now.
 */
enum class ShapeGateBit : uint16_t {
    Step3_Margin       = 1u << 0,
    Step5_MinWidth     = 1u << 1,
    Step6_MaxWidth     = 1u << 2,
    Step7_Sharpness    = 1u << 3,
    Step8_PeakRatio    = 1u << 4,
    Step9_Valley       = 1u << 5,
    Step10_Flatness    = 1u << 6,
    Step11_Symmetry    = 1u << 7,
    Step13_Neighbor    = 1u << 8,
    Step15_RSSIVar     = 1u << 9,
    Step14_Mahalanobis = 1u << 10,
    Step12_Kurtosis    = 1u << 11,
};

/** @brief Per-candidate shape-chain gate verdict (16 bytes SRAM).
 *  Records which shape steps cleared or rejected a detection candidate, the
 *  measured metrics, and the candidate's absolute RF — so the UI can place a
 *  marker on the spectrum without re-deriving bin geometry at paint time.
 *
 *  No heap, no virtuals, no FP. All integers.
 */
struct GateVerdict {
    uint8_t  bin{};               ///< FFT bin index (0-255) of the candidate peak
    uint8_t  power{};             ///< Peak power at that bin (0-255, pre-shelf)
    uint8_t  margin{};            ///< peak - noise_floor (units)
    uint8_t  width{};             ///< Measured emission width in bins (Step 4)
    uint16_t gates_passed_mask{}; ///< ShapeGateBit slots that did NOT reject
    int8_t   reject_reason{-1};   ///< -1  = cleared Steps 3-11,
                                  ///<  0..11 = ShapeGateBit slot of the step
                                  ///<          that rejected first,
                                  ///< 0x7F = not evaluated (null data, or a
                                  ///<          bin outside every sideband)
    uint8_t  pad{};               ///< Aligns `freq` to 8 bytes (struct = 16 B)
    FreqHz   freq{0};             ///< Absolute RF of the peak (Hz); 0 = no RF
                                  ///< placeable (DC spike / underflow).

    [[nodiscard]] bool passed_all() const noexcept { return reject_reason == -1; }
    [[nodiscard]] bool step_passed(ShapeGateBit bit) const noexcept {
        return (gates_passed_mask & static_cast<uint16_t>(bit)) != 0;
    }
};

/** @brief Gate overlay limits for spectrum visualization (Flash-resident).
 *  These are the boundaries the UI draws on the spectrum so the user sees
 *  what the gates would accept or reject.
 *  @note RAW ScanConfig user values — populate straight from
 *        DroneScanner::config_ (spectrum_min_width, spectrum_max_width,
 *        spectrum_valley_depth, ...). The ONE exception is margin_gate:
 *        see its field comment.
 *        Sens-mode easing (eased_*_limit, w²) is deliberately NOT applied;
 *        see the KNOWN DIVERGENCES note on evaluate_candidate_gates.
 *  @note Every field: 0 = that gate is off.
 */
struct GateOverlay {
    uint8_t  margin_gate{};       ///< Step-3 entry gate (units above noise floor).
                                  ///< Pass the detector's EFFECTIVE gate for the
                                  ///< frame — DroneScanner::shape_gate_margin_try() —
                                  ///< NOT the raw config margin: at Sens > 75 with
                                  ///< Sens-mode OFF the detector scales its gate up
                                  ///< by (Sens−75)/2, and the raw value would make
                                  ///< this mirror LOOSER than the detector (false
                                  ///< green). 0 = gate off (mutex contention on the
                                  ///< _try accessor → fall back to the raw config
                                  ///< value at the call site).
    uint8_t  maxw_limit{};        ///< Step-6 MaxW width cap in bins (0 = disabled)
    uint8_t  valley_limit{};      ///< Step-9 flank-max margin must be < this (0 = off)
    uint8_t  flatness_limit{};    ///< Step-10 flatness % must be <= this (0 = off)
    uint8_t  symmetry_limit{};    ///< Step-11 symmetry % must be >= this (0 = off)
    uint8_t  sharpness_limit{};   ///< Step-7 sharpness x100 must be >= this (0 = off)
    uint8_t  ratio_limit{};       ///< Step-8 Rat: margin*10/width must be >= this
                                  ///< (0 = disabled — DEFAULT_SPECTRUM_PEAK_RATIO
                                  ///< is 0, so Rat is inert until the user sets it)
    uint8_t  minw_limit{};        ///< Step-5 MinW in bins. The detector enforces
                                  ///< config_.spectrum_min_width UNCONDITIONALLY,
                                  ///< so pass that value (0 = reject nothing).
};

/** @brief Maximum concurrent gate verdicts we can visualize (top-K CFAR peaks).
 *  8 verdicts × 16 B = 128 B SRAM. Fits comfortably in the 512 B stack budget
 *  when passed by reference; stored as a DroneDisplay member (BSS).
 */
constexpr size_t MAX_GATE_VERDICTS = 8;

/** @brief Convert a ShapeGateBit to a single-character label for on-screen display.
 *  @return Pointer to Flash-resident NUL-terminated string (1 char + NUL).
 */
[[nodiscard]] inline const char* gate_step_label(ShapeGateBit bit) noexcept {
    switch (bit) {
        case ShapeGateBit::Step3_Margin:       return "M";
        case ShapeGateBit::Step5_MinWidth:     return "W";
        case ShapeGateBit::Step6_MaxWidth:     return "X";
        case ShapeGateBit::Step7_Sharpness:    return "S";
        case ShapeGateBit::Step8_PeakRatio:    return "R";
        case ShapeGateBit::Step9_Valley:       return "V";
        case ShapeGateBit::Step10_Flatness:    return "F";
        case ShapeGateBit::Step11_Symmetry:    return "Y";
        case ShapeGateBit::Step13_Neighbor:    return "N";
        case ShapeGateBit::Step15_RSSIVar:     return "r";
        case ShapeGateBit::Step14_Mahalanobis: return "G";
        case ShapeGateBit::Step12_Kurtosis:    return "K";
    }
    return "?";
}

/** @brief Color for a gate step: green = passed, red = rejected, grey = not evaluated.
 *  @param passed true if the step passed.
 *  @return RGBA color constant from constants.hpp.
 */
[[nodiscard]] inline uint32_t gate_step_color(bool passed) noexcept {
    return passed ? COLOR_LOW_THREAT : COLOR_CRITICAL_THREAT;
}

/**
 * @brief Evaluate the in-frame shape chain (Steps 3-11) for ONE candidate.
 * @param spectrum  256-bin raw FFT power of ONE slice (read-only).
 * @param bin       Candidate peak bin (0-255).
 * @param f_center  Slice center of the frame `spectrum` came from — the only
 *                  input that turns `bin` into the absolute RF stored in
 *                  verdict.freq (a sweep window spans many 20 MHz slices, so
 *                  bin→Hz is meaningless without the slice it was captured in).
 * @param noise_floor p25 shelf of the usable bins.
 * @param overlay   Gate limits — RAW ScanConfig values, 0 = gate off.
 * @param verdict   Output: pass/fail per step + metrics + absolute frequency.
 * @return false when a step rejected (verdict.reject_reason = slot index),
 *         true when the candidate cleared Steps 3-11.
 * @note  Full semantics, cost budget and the list of deliberate divergences
 *        from scanner.cpp::apply_shape_filters() live with the definition in
 *        gate_activity.cpp — one doc, one author.
 * @note  NOT CALLED from anywhere yet: DroneScannerUI is the intended
 *        producer (it owns the frame and its f_center), DroneDisplay the
 *        consumer. Both ends exist (set_gate_verdicts / render_gate_overlay)
 *        but no call site wires them together.
 */
[[nodiscard]] bool evaluate_candidate_gates(
    const uint8_t* spectrum,
    size_t bin,
    FreqHz f_center,
    uint8_t noise_floor,
    const GateOverlay& overlay,
    GateVerdict& verdict
) noexcept;

/**
 * @brief Apply the display-side gate floor to a sweep composite.
 * @param composite In/out 240-byte composite pixel buffer (true-position mapped).
 * @param size      Composite size (must be <= COMPOSITE_SIZE).
 * @param overlay   Gate limits — only margin_gate is consumed here.
 * @note Stack: ~8 bytes. Flash: ~90 bytes. No heap, no FP.
 * @note NOT CALLED from anywhere yet.
 */
void prepare_sweep_for_gates(
    uint8_t* composite,
    size_t size,
    const GateOverlay& overlay
) noexcept;

/**
 * @brief Render gate-activity verdicts as a compact overlay on the spectrum.
 * @param painter   UI painter.
 * @param verdicts  Array of gate verdicts (from evaluate_candidate_gates).
 * @param count     Number of valid verdicts (0..MAX_GATE_VERDICTS).
 * @param x0,y0     TOP-LEFT of the CHART area — the plotting rectangle
 *                  render_composite_* uses, i.e. already past the band header.
 *                  Passing the band rect instead would offset every glyph by
 *                  the header height.
 * @param w,h       Chart width/height in pixels (h == the bars' chart_height,
 *                  so the height mapping is byte-identical to theirs).
 * @param f_min,f_max Linear frequency bounds of the chart.
 * @param noise_floor Current shelf — markers are shifted by it exactly as the
 *                  bars are, so they share one vertical scale.
 * @note Stack: ~32 bytes. Flash: ~350 bytes. No heap, no FP.
 * @note NOT CALLED from anywhere yet — DroneDisplay::render_gate_overlay() is
 *        the intended entry point, and paint() has no call site for it.
 */
void render_gate_activity(
    Painter& painter,
    const GateVerdict* verdicts,
    size_t count,
    uint16_t x0,
    uint16_t y0,
    uint16_t w,
    uint16_t h,
    FreqHz f_min,
    FreqHz f_max,
    uint8_t noise_floor
) noexcept;

} // namespace drone_analyzer

#endif // GATE_ACTIVITY_HPP
