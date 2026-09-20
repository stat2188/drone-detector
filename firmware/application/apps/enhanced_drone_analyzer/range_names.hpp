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
 * APPEND-ONLY CONTRACT — DO NOT INSERT, REORDER OR REMOVE ENTRIES.
 * Settings files (settings_manager.cpp: sw*_dw*_name keys) persist the RAW
 * INDEX into this table. Inserting/removing in the middle silently remaps
 * every already-saved label to a different meaning. New labels must be
 * appended after the last entry (index space grows, old files stay valid).
 * Out-of-range stored indices degrade to 0 ("-" = no label) on load.
 *
 * @note Flash-only data (constexpr) — zero SRAM cost.
 * @note Latin-uppercase only: the PortaPack default font has no Cyrillic
 *       glyphs, so labels are transliterated (RER = РЭР, REB = РЭБ,
 *       RADIO = рация, FRIEND = свои, FOE = не свои).
 * @note Index 0 ("-" = no label) restores the "Unknown" fallback.
 * @note SWP-tab selector width guard: drone_sweep_view.cpp statically
 *       asserts every entry fits its 10-character RangeNameSelector field;
 *       the drone list (type_name[16]) additionally bounds labels by
 *       RANGE_NAME_LENGTH_LIMIT (15) — keep both guards in mind.
 */
constexpr const char* const RANGE_NAMES[] = {
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
    "GNSS",     // 18 — generic satellite navigation (any constellation)
    "LORA",     // 19 — LoRa / LoRaWAN telemetry link
    "GSM",      // 20 — cellular 2G/3G/4G/5G
    "CTRL",     // 21 — generic radio-control link (RC)
    "AUDIO",    // 22 — wireless mic / audio link
    "TV",       // 23 — analog TV / broadcast video channel
    "ADSB",     // 24 — ADS-B aircraft transponder
    "CIVIL",    // 25 — civilian emitter
    "MIL",      // 26 — military emitter
    "JAMMER",   // 27 — EW jammer / blocker
    "RADAR",    // 28 — radar emitter
    "METEO",    // 29 — radiosonde / weather balloon
    "BEACON",   // 30 — beacon / marker transmitter
    "TEST",     // 31 — test / calibration emitter
    "DRONE",    // 32 — generic drone signal (type unknown)
};

/// Derived from the array above — never a hand-maintained duplicate count.
constexpr size_t RANGE_NAME_COUNT =
    sizeof(RANGE_NAMES) / sizeof(RANGE_NAMES[0]);

/**
 * @brief Maximum label length that fits DisplayDroneEntry::type_name[16]
 */
constexpr size_t RANGE_NAME_LENGTH_LIMIT = 15;

static_assert(RANGE_NAME_COUNT <= 64,
    "RANGE_NAME_COUNT must stay representable in the persisted uint8_t index");

/** Compile-time strlen over the Flash-resident label table. */
constexpr size_t range_name_strlen(const size_t idx) noexcept {
    size_t len = 0;
    while (RANGE_NAMES[idx][len] != '\0') ++len;
    return len;
}

/// Every label must fit DisplayDroneEntry::type_name[16] (16 incl. NUL).
constexpr bool range_names_fit_length_limit() noexcept {
    for (size_t i = 0; i < RANGE_NAME_COUNT; ++i) {
        if (range_name_strlen(i) > RANGE_NAME_LENGTH_LIMIT) return false;
    }
    return true;
}
static_assert(range_names_fit_length_limit(),
    "RANGE_NAMES entry exceeds RANGE_NAME_LENGTH_LIMIT — "
    "it would be truncated on the drone list");

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
