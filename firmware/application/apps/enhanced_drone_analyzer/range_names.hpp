#ifndef RANGE_NAMES_HPP
#define RANGE_NAMES_HPP

#include <cstdint>
#include <cstddef>

namespace drone_analyzer {

/**
 * @brief Hardcoded detection-range label table for the SWP tab
 *
 * Each of the 5 detection-range slots per sweep window can carry a user
 * selectable LABEL (index into this table). At detection time the scanner
 * resolves the slot containing the peak frequency and stores the index on
 * the TrackedDrone; the drone list then shows the label INSTEAD of the
 * generic "Unknown" type string (sweep-mode drones have no DB entry).
 *
 * @note Flash-only data (constexpr) — zero SRAM cost.
 * @note Latin-uppercase only: the PortaPack default font has no Cyrillic
 *       glyphs, so labels are transliterated (RER = РЭР, REB = РЭБ,
 *       RADIO = рация, FRIEND = свои, FOE = не свои).
 * @note Index 0 ("-" = no label) restores the "Unknown" fallback.
 */
constexpr size_t RANGE_NAME_COUNT = 18;

/**
 * @brief Maximum label length that fits DisplayDroneEntry::type_name[16]
 */
constexpr size_t RANGE_NAME_LENGTH_LIMIT = 15;

constexpr const char* const RANGE_NAMES[RANGE_NAME_COUNT] = {
    "-",        // 0 — no label: fall back to the drone type string
    "FPV",      // 1 — analog/digital FPV video downlink
    "MAVIC",    // 2 — DJI Mavic control/telemetry
    "AUTEL",    // 3 — Autel control link
    "RER",      // 4 — radio-electronic reconnaissance
    "REB",      // 5 — radio-electronic warfare (EW)
    "RADIO",    // 6 — walkie-talkie / PMR
    "FRIEND",   // 7 — friendly / own emitter
    "FOE",      // 8 — hostile emitter
    "VIDEO",    // 9 — generic video link
    "LINK",     // 10 — generic data link
    "C2",       // 11 — command & control
    "TELEM",    // 12 — telemetry
    "WIFI",     // 13 — 2.4/5 GHz WLAN
    "BT",       // 14 — Bluetooth
    "GPS",      // 15 — GNSS L1
    "GLONASS",  // 16 — GLONASS
    "CUSTOM",   // 17 — user-defined
};

static_assert(RANGE_NAME_COUNT <= 64,
    "RANGE_NAME_COUNT must stay representable in the persisted uint8_t index");

/**
 * @brief Map a stored label index to its Flash-resident string
 * @param idx Index into RANGE_NAMES (out-of-range → index 0, no label)
 * @return NUL-terminated label, never nullptr
 * @note O(1), no allocation, thread-safe (read-only data).
 */
[[nodiscard]] inline const char* range_name_to_string(uint8_t idx) noexcept {
    return RANGE_NAMES[(idx < RANGE_NAME_COUNT) ? idx : 0];
}

} // namespace drone_analyzer

#endif // RANGE_NAMES_HPP
