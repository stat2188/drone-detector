#ifndef SWEEP_PRESETS_HPP
#define SWEEP_PRESETS_HPP

// SINGLE-TU HEADER — SWEEP_PRESETS is a namespace-scope constexpr array, i.e.
// an object with internal linkage: every TU that includes this file emits its
// own .rodata copy of the whole table (+328 B of Flash each, plus the
// validator code). Included by drone_sweep_view.cpp ONLY — add a second
// includer only if a second copy is worth its Flash.

#include <array>
#include <cstddef>
#include <cstdint>

#include "constants.hpp"
#include "range_names.hpp"

namespace drone_analyzer {

/**
 * @brief One detection-range slot of a sweep-window preset (Flash-resident)
 * @note MHz domain — exactly what the SWP NumberFields display and what the
 *       ScanConfig / SettingsStruct MHz mirror stores. 0/0 = slot OFF
 *       (is_det_win_slot_active). label_idx is an index into RANGE_NAMES.
 */
struct DetRangeSlotPreset {
    uint32_t from_mhz{0};
    uint32_t to_mhz{0};
    uint8_t label_idx{0};
};

/**
 * @brief One sweep window of a preset: scan span + 5 detection-range slots
 * @note Invariant (statically validated below): every active slot lives inside
 *       [start_hz, end_hz] — the same invariant enforced at runtime by
 *       clamp_det_mhz_to_window() / sanitize_det_ranges() in drone_sweep_view.cpp.
 *       A preset that violates it would be silently clamped on apply.
 */
struct SweepWindowPreset {
    FreqHz start_hz{0};
    FreqHz end_hz{0};
    bool enabled{false};
    std::array<DetRangeSlotPreset, DETECTION_WINDOWS_PER_WINDOW> slots{};
};

/**
 * @brief Complete 4-window preset (applied to ALL windows at once)
 * @note name doubles as the button label — must stay <= 15 chars (libstdc++
 *       SSO: a longer string allocates from the heap when ui::Button
 *       constructs its std::string; the codebase budget is 15).
 */
struct SweepPreset {
    const char* name;
    std::array<SweepWindowPreset, MAX_SWEEP_WINDOWS> windows;
};

/**
 * @brief Compile-time resolution of a RANGE_NAMES label index
 * @param label NUL-terminated label to look up (Flash string)
 * @return Index into RANGE_NAMES, or 0 ("-" = no label) when not found
 * @note Keeps the preset table self-documenting ({"FPV"} instead of a magic
 *       index) and immune to hand-maintained index duplicates. Append-only
 *       RANGE_NAMES contract still applies — this only tracks it.
 */
[[nodiscard]] constexpr uint8_t range_label_index(const char* label) noexcept {
    for (uint8_t i = 0; i < static_cast<uint8_t>(RANGE_NAME_COUNT); ++i) {
        size_t j = 0;
        while (label[j] != '\0' && RANGE_NAMES[i][j] != '\0' && label[j] == RANGE_NAMES[i][j]) {
            ++j;
        }
        if (label[j] == '\0' && RANGE_NAMES[i][j] == '\0') {
            return i;
        }
    }
    return 0;
}

// ============================================================================
// DANGER DRONES — factory band plan for the drone-relevant spectrum
// ============================================================================
// Detection ranges are an ALLOW-LIST: with at least one active slot,
// SweepProcessor::is_detection_window_allowed() drops every peak outside the
// union of the active ranges BEFORE the Mahalanobis/tracking stage — gated
// peaks never become tracked drones (scanner.cpp: apply_sweep_tracking).
// So each window lists only the drone-bearing sub-bands; everything else
// (GSM, LTE, broadcast TV, DVB-T, airband) stays out of the ranges and
// cannot fire a detection. All slots are labelled so the drone list shows
// the band class instead of the "Unknown" fallback.
//
// Window spans equal the shared factory defaults (constants.hpp) — enabling
// all four windows sweeps: 5.8 GHz FPV + 2.4 GHz + 1.2 GHz FPV + sub-GHz.
//
// Band plan (MHz):
//   W1 5645..5945 (5.8 GHz FPV / DJI video downlinks)
//     R1 5645..5725 FPV   analog FPV ch1..2 / RaceBand 5658, 5695
//     R2 5725..5850 MAVIC DJI O3/O4 air units + analog RaceBand 5732..5843
//     R3 5850..5945 FPV   analog RaceBand 5866, 5905 / high FPV channels
//     R4/R5 off
//   W2 2400..2483 (2.4 GHz ISM; the .5 MHz edge does not survive MHz storage)
//     R1 2400..2427 WIFI  WLAN (Wi-Fi drone bridges, FPV wifi)
//     R2 2427..2452 MAVIC DJI control/telemetry
//     R3 2452..2472 CTRL  generic RC (SFHSS, ExpressLRS 2.4)
//     R4 2472..2483 WIFI  WLAN upper edge
//     R5 off
//   W3 1120..1360 (1.2 GHz long-range FPV)
//     R1 1120..1360 FPV   entire band (single video-downlink slot)
//     R2..R5 off
//   W4 433..928 (sub-GHz control / telemetry)
//     R1 433..436  CTRL   433 MHz ISM RC links (EU/China SRD 433.05..434.79)
//     R2 863..870  LORA   LoRa/LoRaWAN drone telemetry (EU SRD)
//     R3 902..928  CTRL   915 MHz ISM RC/telemetry (US/AU, ExpressLRS 900)
//     R4/R5 off
//
// Invariants enforced on every preset (sweep_presets_are_valid() below):
//   1. each window span is ordered and inside the hardware frequency range;
//   2. an active slot is ordered (from <= to) and inside its own window span,
//      with a RANGE_NAMES index that exists — 0 (unlabelled) is legal for an
//      active slot, a label on a range-less slot is not;
//   3. an inactive slot is exactly 0/0 with no label (half-open slots are
//      unrepresentable and rejected);
//   4. no two active slots of a window overlap beyond their shared boundary
//      frequency. The gate expands slots to CLOSED Hz ranges, so neighbours
//      legally touch (end == next start) — that is exactly what keeps the
//      union gapless. The gate unions the ranges, so a touch cannot
//      double-count a peak; an INTERIOR overlap of 2+ MHz would shadow the
//      later label (the drone list resolves the FIRST matching slot) and
//      burn one of the 5 slots per window.
constexpr SweepPreset SWEEP_PRESETS[] = {
    {
        "DANGER DRONES",
        {{
            // W1 — 5.8 GHz FPV / DJI video
            {SWEEP_DEFAULT_START_HZ, SWEEP_DEFAULT_END_HZ, true,
             {{{5645, 5725, range_label_index("FPV")},
               {5725, 5850, range_label_index("MAVIC")},
               {5850, 5945, range_label_index("FPV")},
               {},
               {}}}},
            // W2 — 2.4 GHz ISM
            {SWEEP2_DEFAULT_START_HZ, SWEEP2_DEFAULT_END_HZ, true,
             {{{2400, 2427, range_label_index("WIFI")},
               {2427, 2452, range_label_index("MAVIC")},
               {2452, 2472, range_label_index("CTRL")},
               {2472, 2483, range_label_index("WIFI")},
               {}}}},
            // W3 — 1.2 GHz long-range FPV
            {SWEEP3_DEFAULT_START_HZ, SWEEP3_DEFAULT_END_HZ, true,
             {{{1120, 1360, range_label_index("FPV")}, {}, {}, {}, {}}}},
            // W4 — 433 / 868 / 915 MHz control & telemetry
            {SWEEP4_DEFAULT_START_HZ, SWEEP4_DEFAULT_END_HZ, true,
             {{{433, 436, range_label_index("CTRL")},
               {863, 870, range_label_index("LORA")},
               {902, 928, range_label_index("CTRL")},
               {},
               {}}}},
        }},
    },
};

/// Number of presets in the table above (add new entries here — the SWP UI
/// currently exposes preset 0; further presets need a selector, not code).
constexpr size_t SWEEP_PRESET_COUNT = sizeof(SWEEP_PRESETS) / sizeof(SWEEP_PRESETS[0]);

/// Preset applied by the "DANGER DRONES" button in the SWP tab.
constexpr uint8_t SWEEP_PRESET_DANGER_DRONES = 0;

// Presets are selected by their own index (the SWP button passes
// SWEEP_PRESET_DANGER_DRONES into SWEEP_PRESETS[i]), never by the window
// selector — so the index must be inside the table or the read runs off its
// end. Assert against the PRESET count, not against MAX_SWEEP_WINDOWS: the
// two are unrelated numbers (a 5-preset table would be perfectly legal) and
// comparing them would assert nothing about actual use.
static_assert(SWEEP_PRESET_DANGER_DRONES < SWEEP_PRESET_COUNT,
    "SWEEP_PRESET_DANGER_DRONES indexes past the end of SWEEP_PRESETS");

// ----------------------------------------------------------------------------
// Compile-time validation — the preset table is data, so validate it like data
// ----------------------------------------------------------------------------

/** Compile-time strlen over a Flash-resident C string. */
[[nodiscard]] constexpr size_t preset_name_strlen(const char* s) noexcept {
    size_t n = 0;
    while (s[n] != '\0') ++n;
    return n;
}

/** Every preset name must fit ui::Button's std::string SSO (15 chars). */
constexpr bool sweep_preset_names_fit_sso() noexcept {
    for (size_t p = 0; p < SWEEP_PRESET_COUNT; ++p) {
        if (preset_name_strlen(SWEEP_PRESETS[p].name) > 15) return false;
    }
    return true;
}
static_assert(sweep_preset_names_fit_sso(),
    "Sweep preset name exceeds 15 chars — ui::Button would allocate from the heap");

/**
 * @brief Do two detection ranges of one window genuinely collide?
 * @return true when both slots are active AND claim at least 2 MHz in common
 *         (an OFF slot, 0/0, never collides).
 * @note The detection gate expands a slot to a CLOSED Hz range
 *       (SweepProcessor::is_detection_window_allowed: lo = from * 1e6,
 *       hi = to * 1e6, both inclusive), so consecutive slots are REQUIRED to
 *       touch — end == next start is what keeps the union gapless; any gap
 *       would blind the gate across the whole MHz in between. Touching is
 *       one shared frequency claimed twice (first match wins the label),
 *       which is legal. What this rejects is an INTERIOR overlap of 2+ MHz:
 *       there the later slot's label becomes unreachable over real spectrum
 *       and one of the 5 slots per window is wasted.
 * @note Compile-time only — a violation fails the build instead of silently
 *       shadowing a label at run time.
 */
[[nodiscard]] constexpr bool preset_slots_conflict(
    const DetRangeSlotPreset& a, const DetRangeSlotPreset& b) noexcept {
    if (a.from_mhz == 0 || b.from_mhz == 0) return false;  // 0/0 = OFF slot
    // Shared MHz run = [max(from), min(to)]; checks (a)-(d) already proved
    // both slots non-empty, ordered and inside the window before this runs.
    const uint32_t lo = (a.from_mhz > b.from_mhz) ? a.from_mhz : b.from_mhz;
    const uint32_t hi = (a.to_mhz < b.to_mhz) ? a.to_mhz : b.to_mhz;
    return lo < hi;  // 2+ MHz shared = interior overlap; lo == hi = touching
}

/**
 * @brief Validate every preset window against the runtime SWP invariants
 * @return true when every window span is inside the hardware range and every
 *         active detection slot (a) has both bounds, (b) is ordered,
 *         (c) lives inside its own window span (MHz floor of the Hz bound —
 *         identical to what the SWP NumberFields display), (d) carries a
 *         valid RANGE_NAMES index and (e) overlaps no other active slot of
 *         the same window beyond their shared boundary frequency (closed Hz
 *         ranges must be allowed to touch — see preset_slots_conflict). A
 *         labelled slot with a 0 range would be dead weight (labels resolve
 *         only from active slots), so it fails too.
 */
constexpr bool sweep_presets_are_valid() noexcept {
    for (size_t p = 0; p < SWEEP_PRESET_COUNT; ++p) {
        for (size_t w = 0; w < MAX_SWEEP_WINDOWS; ++w) {
            const SweepWindowPreset& win = SWEEP_PRESETS[p].windows[w];
            if (win.start_hz >= win.end_hz) return false;
            if (win.start_hz < HARDWARE_MIN_FREQ_HZ || win.end_hz > HARDWARE_MAX_FREQ_HZ) {
                return false;
            }
            for (size_t s = 0; s < DETECTION_WINDOWS_PER_WINDOW; ++s) {
                const DetRangeSlotPreset& slot = win.slots[s];
                if (slot.label_idx >= RANGE_NAME_COUNT) return false;
                const bool from_off = (slot.from_mhz == 0);
                const bool to_off = (slot.to_mhz == 0);
                if (from_off != to_off) return false;  // half-open slot is not representable
                if (from_off) {
                    if (slot.label_idx != 0) return false;  // label without a range
                    continue;
                }
                if (slot.from_mhz > slot.to_mhz) return false;
                if (slot.from_mhz < (win.start_hz / MHZ)) return false;
                if (slot.to_mhz > (win.end_hz / MHZ)) return false;
            }
            // (e) pairwise — runs only after (a)-(d) proved every slot is
            // either 0/0 or a sane ordered range, so the helper sees clean
            // intervals.
            for (size_t a = 0; a < DETECTION_WINDOWS_PER_WINDOW; ++a) {
                for (size_t b = a + 1; b < DETECTION_WINDOWS_PER_WINDOW; ++b) {
                    if (preset_slots_conflict(win.slots[a], win.slots[b])) return false;
                }
            }
        }
    }
    return true;
}
static_assert(sweep_presets_are_valid(),
    "Sweep preset violates the SWP invariants (window span / slot order / label index / slot overlap)");

} // namespace drone_analyzer

#endif // SWEEP_PRESETS_HPP
