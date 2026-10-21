/*
 * Copyright (C) 2023
 *
 * This file is part of PortaPack.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef SHAPE_GATE_LOGIC_HPP
#define SHAPE_GATE_LOGIC_HPP

/*
 * Pure shape-gate easing/arming helpers — SINGLE-AUTHOR formulas shared by
 * scanner.cpp (apply_shape_filters Steps 7-12 + the Step-13 EFFECTIVE-ARMING
 * mirror) and by the host-side regression tests (test_shape_filters_phase_a.cpp),
 * which could not include scanner.hpp (it pulls ch.h). Header-only, no ChibiOS
 * dependencies, no ScanConfig — every parameter is a scalar, so this header
 * compiles both on the ARM target and on the host test toolchain.
 *
 * Moved verbatim from scanner.cpp (audit F4 2026-10): the test suite
 * previously mirrored the eased limits by hand and therefore could not catch
 * arming-predicate regressions (audit F1); one formula, two consumers.
 */

#include <cstdint>

#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief SENS adaptive stringency weight (0..100) for one peak (SHAPE_ADAPT_*).
 * @param peak_margin Peak elevation above the noise shelf (units)
 * @param gate Effective Step-3 gate this frame (shape_gate_margin())
 * @return 0 at/under the gate, ramping linearly to 100 at
 *         gate + SHAPE_ADAPT_SLACK_UNITS (+4 dB). Consumers apply w² easing
 *         (w2/10000), so thresholds sit near their permissive floor at small
 *         w and converge quadratically to the exact user value at w = 100.
 * @note apply_shape_filters forces w = 100 when sensitive_mode is OFF —
 *       every eased formula then collapses to the legacy behavior.
 * Stack: ~8 bytes (registers). Flash: ~40 bytes. No FP; division is by
 *       SHAPE_ADAPT_SLACK_UNITS, static_asserted > 0.
 */
inline uint8_t shape_adapt_weight(uint8_t peak_margin, uint8_t gate) noexcept {
    if (peak_margin <= gate) return 0;
    const uint32_t above = static_cast<uint32_t>(peak_margin - gate);
    const uint32_t w = (above * 100u) / static_cast<uint32_t>(SHAPE_ADAPT_SLACK_UNITS);
    return static_cast<uint8_t>((w > 100u) ? 100u : w);
}

/**
 * @brief Step-10 flatness entry guard floor (sensitivity easing stops here).
 * @note Documented: sens=95 (rssi_sens=20) => guard = max(40-20, 15) = 20.
 * Stack: 0. SRAM: 0. Flash: 0.
 */
constexpr uint8_t FLATNESS_MIN_MARGIN_FLOOR = 15;

/**
 * @brief Step-10 flatness entry guard: min peak margin for the step to run.
 * @param rssi_sens rssi_sens_units() of the same config (scanner.cpp — the
 *        expression needs ScanConfig and stays file-local there)
 * @return FLATNESS_MIN_PEAK_MARGIN (40) when rssi_sens <= 0 (threshold at
 *         or above the -95 dBm anchor, Sens <= 75), eased down by rssi_sens
 *         above the anchor, floored at FLATNESS_MIN_MARGIN_FLOOR.
 * @note SINGLE AUTHOR shared by apply_shape_filters Step 10 (run guard) and
 *       the Step-13 EFFECTIVE-ARMING flat term (audit M3 rev. 2): an armed
 *       Flat verdict may only be granted when the step would actually RUN
 *       for this peak. Byte-identical to the former inline expression —
 *       including the uint8 cast of rssi_sens — safe because the Sens
 *       range (0..100 ⇒ threshold -120..-20 dBm, RSSI_MIN_DBM floor)
 *       bounds rssi_sens at 25 <= 255, so the cast never truncates.
 * Stack: ~4 bytes. SRAM: 0. Flash: ~48 bytes. No FP.
 */
inline uint8_t flatness_min_peak_margin(int32_t rssi_sens) noexcept {
    if (rssi_sens <= 0) return FLATNESS_MIN_PEAK_MARGIN;
    return (FLATNESS_MIN_PEAK_MARGIN > static_cast<uint8_t>(rssi_sens))
        ? static_cast<uint8_t>(FLATNESS_MIN_PEAK_MARGIN - rssi_sens)
        : FLATNESS_MIN_MARGIN_FLOOR;
}

/**
 * @brief Sharpness limit above which Step 7 can mathematically REJECT.
 * @details Audit D5 dead-band: avg_margin is measured over [left..right]
 *          which INCLUDES the peak, so avg_margin <= peak_margin and
 *          sharpness = peak*100/avg >= 100 on EVERY frame. A limit of
 *          100 (or any value <= 100) can therefore never reject — a
 *          sharpness config in the dead-band is an ABSTENTION, not a
 *          verdict, and under the Sens similarity score it must not
 *          occupy a permanent ALWAYS-PASS seat (audit D-ARM — see the
 *          Step-7 arming comment in apply_shape_filters).
 * @note SINGLE AUTHOR of the dead-band ceiling: the Step-7 informative
 *       arming and the Step-13 EFFECTIVE-ARMING test
 *       (`armed_sharp_eff > SHARPNESS_NOOP_CEILING`) share it, so the
 *       score denominator and the wideband-ownership test cannot drift.
 * Stack: 0 (constexpr). SRAM: 0. Flash: 0.
 */
constexpr int32_t SHARPNESS_NOOP_CEILING = 100;

/**
 * @brief SENS eased sharpness limit — shared author of Step 7 and arming.
 * @param user_sharpness Raw ScanConfig::spectrum_peak_sharpness (0..255)
 * @param w2 adapt_w², 0..10000 (uint16: 100² fits exactly)
 * @return 100 + (user − 100)·w²/10000: the documented no-op (100) at
 *         w2 = 0 — avg includes the peak, so sharpness ≥ 100 by
 *         construction and a limit ≤ 100 can never reject (audit D5
 *         dead-band 51..100); the exact user value at w2 = 10000.
 * @note Consumed by apply_shape_filters Step 7 AND the Step-13
 *       EFFECTIVE-ARMING test (audit M3 rev. 2).
 * Stack: ~8 bytes (register). SRAM: 0. Flash: ~40 bytes. No FP.
 */
inline int32_t eased_sharpness_limit(uint8_t user_sharpness, uint16_t w2) noexcept {
    return 100 +
        ((static_cast<int32_t>(user_sharpness) - 100) *
         static_cast<int32_t>(w2)) / 10000;
}

/**
 * @brief SENS eased flatness limit — shared author of Step 10 and arming.
 * @param user_flatness Raw ScanConfig::spectrum_flatness (0..255)
 * @param w2 adapt_w², 0..10000
 * @return F + (100 − F)·(10000 − w²)/10000 after saturating F to
 *         SPECTRUM_FLATNESS_PCT_MAX: ≤ 100 always (pct > 100 unreachable ⇒
 *         100 is the abstain ceiling), exact user F at w2 = 10000.
 * @note DOMAIN GUARD (audit D-CFG-RANGE) lives here: the `(100 − F)` term
 *       is UNSIGNED — an F of 101..255 underflows it, the /10000 quotient
 *       then overflows the narrowing cast and the limit comes out as
 *       garbage. Shipped producers clamp 0..100 (settings_manager parse +
 *       NumberField), but this chain eats a raw ScanConfig. Consumed by
 *       apply_shape_filters Step 10 AND Step-13 EFFECTIVE-ARMING (M3 rev. 2).
 * @note ARMING COMPARISON: `< FLATNESS_EFFECTIVE_CEILING` (constants.hpp,
 *       audit F1 2026-10) — a limit of 99 is an abstention, not a verdict.
 * Stack: ~8 bytes. SRAM: 0. Flash: ~56 bytes. No FP.
 */
inline uint16_t eased_flatness_limit(uint8_t user_flatness, uint16_t w2) noexcept {
    const uint16_t flat_user =
        (user_flatness > SPECTRUM_FLATNESS_PCT_MAX)
            ? static_cast<uint16_t>(SPECTRUM_FLATNESS_PCT_MAX)
            : static_cast<uint16_t>(user_flatness);
    return static_cast<uint16_t>(flat_user +
        static_cast<uint16_t>(
            ((static_cast<uint32_t>(SPECTRUM_FLATNESS_PCT_MAX) - flat_user) *
             (10000u - w2)) / 10000u));
}

/**
 * @brief SENS eased valley-depth limit — shared author of Step 9 and arming.
 * @param user_valley_depth Raw ScanConfig::spectrum_valley_depth (0..200,
 *        settings clamp) — 0 is the documented DISABLE sentinel and is
 *        excluded by the valley_outer run guard before this helper runs.
 * @param peak_margin raw_peak − noise_floor (Step 3 gate already cleared,
 *        so >= 1).
 * @param w2 adapt_w², 0..10000.
 * @return D + (peak_margin/3 + 1)·(10000 − w²)/10000: the exact user D at
 *         w² = 10000 (Sens OFF byte-identity), the smooth-flank ceiling
 *         (margin/3 + 1) added at w² = 0. MAX-POLARITY (a HIGHER limit is
 *         MORE permissive: reject iff max_valley >= limit).
 * @note Consumed by apply_shape_filters Step 9 AND the Step-9
 *       INFORMATIVE-ARMING capability test (audit D-ARM ext. 2026-10,
 *       regime (3) in constants.hpp): one formula, so the arming test
 *       cannot drift from what the chain runs. uint16: 200 + 86 = 286 max.
 * Stack: ~8 bytes (register). SRAM: 0. Flash: ~48 bytes. No FP.
 */
inline uint16_t eased_valley_limit(uint8_t user_valley_depth, uint8_t peak_margin, uint16_t w2) noexcept {
    return static_cast<uint16_t>(user_valley_depth) +
        static_cast<uint16_t>(
            (static_cast<uint32_t>(peak_margin / 3u + 1u) *
             (10000u - w2)) / 10000u);
}

/**
 * @brief SENS eased symmetry limit — shared author of Step 11 and arming.
 * @param user_symmetry Raw ScanConfig::spectrum_symmetry (0..100, settings
 *        clamp) — 0 is the documented DISABLE sentinel, excluded by the
 *        Step-11 run guard before this helper runs.
 * @param w2 adapt_w², 0..10000.
 * @return user·w²/10000 (MIN-POLARITY: reject iff sym_pct < limit). At
 *         w² = 10000 the exact user Y (Sens OFF byte-identity); the
 *         INTEGER TRUNCATION to 0 for user·w² < 10000 is the vacuous-bar
 *         regime: ok = (sym_pct >= 0) is then true for EVERY geometry, so
 *         a zero limit can never reject — see the Step-11
 *         INFORMATIVE-ARMING capability test (audit D-ARM ext. 2026-10,
 *         regime (4) in constants.hpp).
 * @note Consumed by apply_shape_filters Step 11 AND its arming — one
 *       formula, cannot drift.
 * Stack: ~4 bytes (register). SRAM: 0. Flash: ~32 bytes. No FP.
 */
inline uint8_t eased_sym_limit(uint8_t user_symmetry, uint16_t w2) noexcept {
    return static_cast<uint8_t>(
        (static_cast<uint32_t>(user_symmetry) * w2) / 10000u);
}

}  // namespace drone_analyzer

#endif  // SHAPE_GATE_LOGIC_HPP
