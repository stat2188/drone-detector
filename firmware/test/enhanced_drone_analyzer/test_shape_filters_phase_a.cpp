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

// ============================================================================
// Phase A regression tests (A-1..A-5 + F1/F2 + the FLAT_WIDE_BAR_PCT
// judgment cap, audit 2026-10).
//
// Convention: this target does NOT link scanner.cpp and cannot include
// scanner.hpp (it pulls ch.h). The pure easing/arming FORMULAS, the
// structural/decision predicates and the constants now live in host-clean
// headers included below — one author, no formula copies (audit F4: the
// pre-F1 suite mirrored the arming inputs and therefore could not catch
// the phantom-Flat-seat regression; the A-3/A-5/A-1 mirrors followed — the
// scanner call sites and these tests share flatness_structural(),
// auto_gate_from_iqr() and raw_fallback_engages() bodies).
// Mirrors left in this file are the short structural booleans only:
//   A-1  scanner.hpp ShapeVerdict::similarity_pct() ratio math
//        (the raw-fallback DECISION is the shared raw_fallback_engages())
//   A-2  constants.hpp DEFAULT_SPECTRUM_MAX_WIDTH (= 110) — real constant
//   A-3  real flatness_structural(): FLATNESS_WIDE_WAIVER_BINS term
//   A-5  real auto_gate_from_iqr(): 3σ IQR formula + raw-Mar cap
//   F1   real eased_flatness_limit + FLATNESS_EFFECTIVE_CEILING (99):
//        silenced group at w ∈ {0,5,10}, re-armed at w = 15
//   F1+  real flatness_judgment_limit(): FLAT_WIDE_BAR_PCT (85) cap —
//        mid-band FP hole closed, Sens-OFF/fallback byte-identity kept
//   F2   real DB_EXTENT_CAP_BINS (110) vs SWEEP_EXTENT_CAP_BINS (114):
//        the DB-only Step-13 MaxW arming must read DISARMED at defaults
//   A-4  intentionally untested: documented analysis-only decision (no new
//        guard exists — see EDA README TBD section)
// ============================================================================

#include "doctest.h"
#include <cstdint>

#include "constants.hpp"
#include "shape_gate_logic.hpp"

namespace phase_a {

using namespace drone_analyzer;

// ---- A-2 mirror: MaxW width gate (Step 6/6b + Step 13 parity) ----
static inline bool maxw_passes(uint16_t width, uint8_t maxw_cfg) {
    return width <= maxw_cfg;
}

// A-5 (update_auto_gate clamp) and A-3 (flat_structural) have NO mirror
// anymore: the tests call the shared bodies auto_gate_from_iqr() and
// flatness_structural() from shape_gate_logic.hpp — same function the
// scanner consumes (audit F4 pattern). The A-3 wrapper only BINDS the
// floor to its shipped default (rssi_sens <= 0 ⇒ FLATNESS_MIN_PEAK_MARGIN);
// the F > 0 / margin-floor / waiver logic itself is the shared body.
static inline bool flat_struct_at_default(uint8_t flatness_cfg,
                                          uint8_t peak_margin,
                                          uint16_t effective_width) {
    return flatness_structural(flatness_cfg, peak_margin,
                               FLATNESS_MIN_PEAK_MARGIN, effective_width);
}

// ---- A-1 mirror: ShapeVerdict::similarity_pct() (scanner.hpp ~2266) ----
static inline uint8_t similarity_pct_mirror(uint16_t sim_armed,
                                            uint16_t sim_pass) {
    const uint16_t armed_set = static_cast<uint16_t>(sim_armed & SHAPE_SIM_MASK);
    const uint32_t armed_count = __builtin_popcount(armed_set);
    if (armed_count == 0u) return 100u;
    const uint16_t pass_set = static_cast<uint16_t>(sim_pass & armed_set);
    const uint32_t pct =
        (__builtin_popcount(pass_set) * 100u) / armed_count;
    return static_cast<uint8_t>((pct > 100u) ? 100u : pct);
}

// ---- A-1 composition around the shared raw_fallback_engages() decision ----
// The boolean itself is the scanner's (shape_gate_logic.hpp); this wrapper
// only carries the two side effects the caller applies after it fires.
struct FallbackResult {
    bool fallback;
    uint16_t effective_armed;
    uint16_t w2_eff;
};

static inline FallbackResult raw_fallback_mirror(bool sens_parallel,
                                                 uint16_t eased_armed,
                                                 uint16_t raw_armed,
                                                 uint16_t w2) {
    FallbackResult r{};
    r.fallback = raw_fallback_engages(sens_parallel, eased_armed, raw_armed);
    r.effective_armed = r.fallback ? raw_armed : eased_armed;
    r.w2_eff = r.fallback ? uint16_t{10000} : w2;
    return r;
}

// ---- F1/F2 fixture: arming masks built from the REAL eased formulas ----
// 2026-10 defaults (constants.hpp) on a DB-wideband plateau:
// Sharp = 165, Flat = 25, Sym = 25, Valley = 0, Kurt off, width = 110.
struct DefaultArming {
    uint16_t sim;   // eased (Sens ON) similarity set
    uint16_t raw;   // Sens-OFF ladder set
};

static inline DefaultArming default_arming_at(uint8_t peak_margin) {
    const uint8_t w = shape_adapt_weight(peak_margin, SHAPE_ADAPT_GATE_FLOOR);
    const uint16_t w2 = static_cast<uint16_t>(w) * w;
    constexpr uint16_t kWidebandPlateau = 110;  // DB-wideband fragment
    DefaultArming a{};
    const bool sim_sharp =
        eased_sharpness_limit(DEFAULT_SPECTRUM_PEAK_SHARPNESS, w2) >
        SHARPNESS_NOOP_CEILING;
    const bool sim_flat =
        flatness_structural(DEFAULT_SPECTRUM_FLATNESS, peak_margin,
                            FLATNESS_MIN_PEAK_MARGIN, kWidebandPlateau) &&
        (w > 0) &&  // Sens-ON outer: !sensitive_mode || adapt_w > 0
        (kWidebandPlateau > FLATNESS_MIN_SIGNAL_WIDTH) &&
        (eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, w2) <
         FLATNESS_EFFECTIVE_CEILING);
    const bool sim_sym =
        (DEFAULT_SPECTRUM_SYMMETRY > 0) && (kWidebandPlateau > 1) &&
        (w > 0) &&  // same outer w > 0 guard as the real Step-11 arming
        (eased_sym_limit(DEFAULT_SPECTRUM_SYMMETRY, w2) > 0);
    a.sim = static_cast<uint16_t>(
        (sim_sharp ? SHAPE_BIT_SHARPNESS : uint16_t{0}) |
        (sim_flat ? SHAPE_BIT_FLATNESS : uint16_t{0}) |
        (sim_sym ? SHAPE_BIT_SYMMETRY : uint16_t{0}));
    const bool raw_sharp =
        eased_sharpness_limit(DEFAULT_SPECTRUM_PEAK_SHARPNESS, 10000u) >
        SHARPNESS_NOOP_CEILING;
    const bool raw_flat =
        flatness_structural(DEFAULT_SPECTRUM_FLATNESS, peak_margin,
                            FLATNESS_MIN_PEAK_MARGIN, kWidebandPlateau) &&
        (kWidebandPlateau > FLATNESS_MIN_SIGNAL_WIDTH) &&
        (eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 10000u) <
         FLATNESS_EFFECTIVE_CEILING);
    const bool raw_sym =
        (DEFAULT_SPECTRUM_SYMMETRY > 0) && (kWidebandPlateau > 1);
    a.raw = static_cast<uint16_t>(
        (raw_sharp ? SHAPE_BIT_SHARPNESS : uint16_t{0}) |
        (raw_flat ? SHAPE_BIT_FLATNESS : uint16_t{0}) |
        (raw_sym ? SHAPE_BIT_SYMMETRY : uint16_t{0}));
    return a;
}

}  // namespace phase_a

// ============================================================================
// A-2: DEFAULT_SPECTRUM_MAX_WIDTH = 110
// ============================================================================

TEST_CASE("A-2: MaxW default matches the DB sideband cap (110)") {
    using namespace phase_a;
    // The DB sideband-cap rejects > 110 by construction; the scan default
    // must agree so both paths gate wideband the same way at defaults.
    CHECK(DEFAULT_SPECTRUM_MAX_WIDTH == 110);
    // 110 is AT the DB cap (inert in DB) but BELOW the sweep cap:
    CHECK(DEFAULT_SPECTRUM_MAX_WIDTH < SWEEP_EXTENT_CAP_BINS);
    CHECK(DEFAULT_SPECTRUM_MAX_WIDTH == DB_EXTENT_CAP_BINS);
}

TEST_CASE("A-2: width gate boundary at defaults") {
    using namespace phase_a;
    CHECK(maxw_passes(110, DEFAULT_SPECTRUM_MAX_WIDTH));   // inclusive bound
    CHECK(!maxw_passes(111, DEFAULT_SPECTRUM_MAX_WIDTH));  // old 111-114 hole closed
    CHECK(!maxw_passes(114, DEFAULT_SPECTRUM_MAX_WIDTH));
}

// ============================================================================
// F2: Step-13 MaxW arming must compare against the DB cap, not the sweep cap
// ============================================================================

TEST_CASE("F2: extent caps are derived from the FFT window constants") {
    using namespace phase_a;
    // DB sidebands [10..119) and [136..246) = 110 bins each:
    CHECK(DB_EXTENT_CAP_BINS == 110);
    // Sweep sidebands [6..119) and [136..250) = 114 bins each:
    CHECK(SWEEP_EXTENT_CAP_BINS == 114);
    CHECK(DB_EXTENT_CAP_BINS < SWEEP_EXTENT_CAP_BINS);
}

TEST_CASE("F2: default MaxW reads DISARMED in the DB Step-13 arming") {
    using namespace phase_a;
    // The DB-only arming predicate is `max_width < DB_EXTENT_CAP_BINS`.
    // At the shipped default 110 this must be FALSE: an extent can never
    // measure more than 110 bins in DB, so MaxW rejects nothing there and
    // must not claim wideband ownership (otherwise the neighbor
    // post-filter is skipped under a Sharp<=100 + Flat=0 config — the
    // documented D-B hole, reopened by the old sweep-cap constant 114).
    CHECK(!(DEFAULT_SPECTRUM_MAX_WIDTH < DB_EXTENT_CAP_BINS));
    // A user-lowered MaxW DOES arm (can reject a measurable extent):
    CHECK(100 < DB_EXTENT_CAP_BINS);
    // And the same default would read ARMED under the sweep cap — the
    // exact confusion the F2 fix removes from the DB-only site:
    CHECK(DEFAULT_SPECTRUM_MAX_WIDTH < SWEEP_EXTENT_CAP_BINS);
}

// ============================================================================
// A-5: auto-gate cap = RAW user Mar
// ============================================================================

TEST_CASE("A-5: 3-sigma gate is measured, not collapsed, at default Mar=10") {
    using namespace phase_a;
    // The old cap (Mar - 2 = 8) made the clamp [8, 8]: dead measurement.
    // Clean spectrum (iqr=2 -> sigma3=4) opens to the floor:
    CHECK(auto_gate_from_iqr(2, DEFAULT_SPECTRUM_MARGIN) ==
          SHAPE_ADAPT_GATE_FLOOR);
    // Moderately noisy (iqr=4 -> sigma3=9) tracks the measurement:
    CHECK(auto_gate_from_iqr(4, DEFAULT_SPECTRUM_MARGIN) == 9);
    // Noisy (iqr=8 -> sigma3=18) is capped at the RAW Mar, not Mar-2:
    CHECK(auto_gate_from_iqr(8, DEFAULT_SPECTRUM_MARGIN) ==
          DEFAULT_SPECTRUM_MARGIN);
}

TEST_CASE("A-5: explicit tightening below the floor wins verbatim") {
    using namespace phase_a;
    // User Mar = 5 (< FLOOR 8): even a noisy measurement cannot exceed 5.
    CHECK(auto_gate_from_iqr(8, 5) == 5);
    // Clean + tightened: floor must not override the user's Mar either.
    CHECK(auto_gate_from_iqr(2, 5) == 5);
}

TEST_CASE("A-5: static floor-vs-default invariant (constants.hpp assert)") {
    using namespace phase_a;
    // Mirrors static_assert(SHAPE_ADAPT_GATE_FLOOR < DEFAULT_SPECTRUM_MARGIN).
    CHECK(SHAPE_ADAPT_GATE_FLOOR < DEFAULT_SPECTRUM_MARGIN);
}

// ============================================================================
// A-3: wide-fragment waiver of the flatness margin floor
// ============================================================================

TEST_CASE("A-3: waiver constant equals the MinW default (40 bins)") {
    using namespace phase_a;
    CHECK(FLATNESS_WIDE_WAIVER_BINS == 40);
    CHECK(FLATNESS_WIDE_WAIVER_BINS == DEFAULT_SPECTRUM_MIN_WIDTH);
    CHECK(FLATNESS_WIDE_WAIVER_BINS > FLATNESS_MIN_SIGNAL_WIDTH);
}

TEST_CASE("A-3: margin floor unchanged for narrow fragments") {
    using namespace phase_a;
    // margin 30 < 40, width 39 < 40: Flat stays disarmed (old behavior).
    CHECK(!flat_struct_at_default(25, 30, 39));
    // margin 39, width 39: still disarmed (both bounds strict).
    CHECK(!flat_struct_at_default(25, 39, 39));
}

TEST_CASE("A-3: waiver arms Flat for wide fragments below the floor") {
    using namespace phase_a;
    // The WiFi-plateau hole: margin in the gate..floor band, width >= 40.
    CHECK(flat_struct_at_default(25, 30, 40));
    CHECK(flat_struct_at_default(25, 8, 110));
}

TEST_CASE("A-3: margin floor still arms strong narrow peaks") {
    using namespace phase_a;
    CHECK(flat_struct_at_default(25, 40, 5));
    CHECK(flat_struct_at_default(25, 100, 1));
}

TEST_CASE("A-3: F = 0 never arms (user disabled)") {
    using namespace phase_a;
    CHECK(!flat_struct_at_default(0, 100, 110));
    CHECK(!flat_struct_at_default(0, 30, 60));
}

// ============================================================================
// A-1: raw fallback + similarity ratio
// ============================================================================

TEST_CASE("A-1: similarity_pct masks first, counts second") {
    using namespace phase_a;
    // Sharp|Flat = 0x05 must score 100/100 = 100%, NOT 2*100/5 = 40%
    // (audit D-SCORE: dividing by the mask VALUE rejected every
    // multi-armed target — Sens ON became stricter than Sens OFF).
    CHECK(similarity_pct_mirror(SHAPE_BIT_SHARPNESS | SHAPE_BIT_FLATNESS,
                                SHAPE_BIT_SHARPNESS | SHAPE_BIT_FLATNESS) == 100);
    // Half agreement on a 2-seat set:
    CHECK(similarity_pct_mirror(SHAPE_BIT_SHARPNESS | SHAPE_BIT_FLATNESS,
                                SHAPE_BIT_SHARPNESS) == 50);
    // Stray non-similarity bits are ignored in BOTH numerator and denominator:
    CHECK(similarity_pct_mirror(0x1Fu | 0x100u, 0x01u | 0x100u) == 20);
    // Empty armed set = full score (documented Sens-OFF identity):
    CHECK(similarity_pct_mirror(0u, 0u) == 100);
}

TEST_CASE("A-1: fallback engages only in the silenced-by-easing band") {
    using namespace phase_a;
    const uint16_t w2_near_gate = 400;  // small w, gate..gate+SLACK band
    // The defect: eased set empty (all gates eased into no-op), raw set
    // non-empty (Sens OFF would run FLAT) -> fallback to raw + w2 = 10000.
    const auto r = raw_fallback_mirror(
        /*sens_parallel=*/true, /*eased=*/0u, /*raw=*/SHAPE_BIT_FLATNESS,
        w2_near_gate);
    CHECK(r.fallback);
    CHECK(r.effective_armed == SHAPE_BIT_FLATNESS);
    CHECK(r.w2_eff == 10000);
    // Score now votes at the exact user limit -> the flat-top is rejected:
    CHECK(similarity_pct_mirror(r.effective_armed, 0u) == 0);
}

TEST_CASE("A-1: identity cases never engage the fallback") {
    using namespace phase_a;
    // User gates OFF: raw set empty too -> empty score keeps passing.
    auto r = raw_fallback_mirror(true, 0u, 0u, 400);
    CHECK(!r.fallback);
    CHECK(similarity_pct_mirror(r.effective_armed, 0u) == 100);
    // Sens OFF: fallback never engages regardless of sets.
    r = raw_fallback_mirror(false, 0u, SHAPE_BIT_FLATNESS, 400);
    CHECK(!r.fallback);
    CHECK(r.effective_armed == 0u);
    CHECK(r.w2_eff == 400);
    // Eased set non-empty: the normal path, no fallback.
    r = raw_fallback_mirror(true, SHAPE_BIT_SHARPNESS, SHAPE_BIT_FLATNESS, 400);
    CHECK(!r.fallback);
    CHECK(r.effective_armed == SHAPE_BIT_SHARPNESS);
    CHECK(r.w2_eff == 400);
}

// ============================================================================
// F1: eased Flat lands on the abstention ceiling (99) for w <= 10 — the
//     whole similarity group is silenced there and the raw fallback must
//     engage; at w = 15 Sharp(101)/Flat(98) re-arm and the normal path
//     resumes. Formulas are the REAL ones (shape_gate_logic.hpp).
// ============================================================================

TEST_CASE("F1: eased flatness limit sits at the abstention ceiling for w <= 10") {
    using namespace phase_a;
    // Default Flat = 25: 25 + 75*(10000-w2)/10000 = 99 iff w2 <= 133.
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 0u) == 100);
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 25u) == 99);   // w=5
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 100u) == 99);  // w=10
    // w = 15 (w2 = 225): 98 — below the ceiling, a REAL reject bar:
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 225u) == 98);
    // The F1 arming predicate treats 99 as an abstention, 98 as a verdict:
    CHECK(!(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 100u) <
            FLATNESS_EFFECTIVE_CEILING));
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 225u) <
          FLATNESS_EFFECTIVE_CEILING);
    // Sens OFF (w2 = 10000): the raw default F = 25 is armed:
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 10000u) ==
          DEFAULT_SPECTRUM_FLATNESS);
}

TEST_CASE("F1: fallback engages for margin <= gate+2 (w = 0/5/10)") {
    using namespace phase_a;
    const uint8_t gate = SHAPE_ADAPT_GATE_FLOOR;  // 8 at defaults
    // w = 0 (at the gate), w = 5 (+1 unit), w = 10 (+2 units):
    for (uint8_t above = 0; above <= 2; ++above) {
        const uint8_t margin = static_cast<uint8_t>(gate + above);
        const auto a = default_arming_at(margin);
        // Sharp eased to 100 (dead-band), Flat eased to 99 (abstention),
        // Sym truncated to 0 -> the WHOLE eased group is silent:
        CHECK(a.sim == 0u);
        // The Sens-OFF ladder (Sharp 165, Flat 25, Sym 25) is NOT empty:
        CHECK(a.raw != 0u);
        const auto r = raw_fallback_mirror(
            /*sens_parallel=*/true, a.sim, a.raw,
            /*w2=*/static_cast<uint16_t>(shape_adapt_weight(margin, gate)) *
                   shape_adapt_weight(margin, gate));
        CHECK(r.fallback);
        CHECK(r.w2_eff == 10000);
        // The raw ladder votes at the exact user limits -> a wideband
        // plateau is rejected, not auto-passed:
        CHECK(similarity_pct_mirror(r.effective_armed, 0u) == 0);
    }
}

TEST_CASE("F1: no fallback at w = 15 (Sharp/Flat re-armed)") {
    using namespace phase_a;
    const uint8_t gate = SHAPE_ADAPT_GATE_FLOOR;
    const uint8_t margin = static_cast<uint8_t>(gate + 3);  // w = 15
    const auto a = default_arming_at(margin);
    // Sharp limit 101 > 100 and Flat limit 98 < 99 both arm:
    CHECK((a.sim & SHAPE_BIT_SHARPNESS) != 0u);
    CHECK((a.sim & SHAPE_BIT_FLATNESS) != 0u);
    // Normal (non-fallback) path — documented residual weak band:
    const uint16_t w2 = static_cast<uint16_t>(
        shape_adapt_weight(margin, gate)) * shape_adapt_weight(margin, gate);
    const auto r = raw_fallback_mirror(true, a.sim, a.raw, w2);
    CHECK(!r.fallback);
    CHECK(r.effective_armed == a.sim);
    CHECK(r.w2_eff == w2);
}

TEST_CASE("F1: fallback never engages under Sens OFF") {
    using namespace phase_a;
    const auto a = default_arming_at(SHAPE_ADAPT_GATE_FLOOR);  // w = 0
    CHECK(a.sim == 0u);
    const auto r = raw_fallback_mirror(
        /*sens_parallel=*/false, a.sim, a.raw, 0u);
    CHECK(!r.fallback);
    CHECK(r.w2_eff == 0u);
}

// ============================================================================
// F1+: flatness_judgment_limit — the FLAT_WIDE_BAR_PCT (85) cap on the
//      Step-10 PASS/FAIL limit. ARMING (eased_flatness_limit < 99 above)
//      stays UNCAPPED — capping the arming comparison would re-open the F1
//      phantom-seat band; the cap only shapes the verdict comparison. The
//      byte-identity at w2 = 10000 (Sens OFF + the A-1 fallback judge with
//      it) is the safety-critical property: the Sens-OFF ladder must not
//      move with the cap.
// ============================================================================

TEST_CASE("F1+: judgment cap binds the loosened mid band at the default F = 25") {
    using namespace phase_a;
    // w = 15 (w2 = 225): the UNCAPPED eased limit 98 used to judge here —
    // a WiFi/BT plateau measured 86-98% passed exactly in the band where
    // the gates had just opened (audit mid-band FP hole):
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 225u) == 98);
    CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, 225u) ==
          FLAT_WIDE_BAR_PCT);
    // The capped limit sits at the bar for every w2 <= 2000 (eased >= 85
    // there — strict binding while eased > 85 ⇔ w2 <= 1866, pass-through
    // where eased already == 85; every reachable w = 15..40 has w2 <= 1600
    // so the cap strictly binds at each of them):
    for (uint16_t w2 = 225u; w2 < 2000u; w2 += 50u) {
        CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, w2) ==
              FLAT_WIDE_BAR_PCT);
        // ...and is never LOOSER than the uncapped eased value anywhere:
        CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, w2) <=
              eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, w2));
    }
    // w2 = 2000 (w ≈ 44.7): eased lands exactly on the bar, the cap turns
    // into a pass-through and the descent continues eased-only:
    CHECK(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 2000u) ==
          FLAT_WIDE_BAR_PCT);
    CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, 2000u) ==
          FLAT_WIDE_BAR_PCT);
    // ...down to the exact user value at Sens OFF:
    CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, 10000u) ==
          DEFAULT_SPECTRUM_FLATNESS);
}

TEST_CASE("F1+: judgment cap is inert at w2 = 10000 for every user F") {
    using namespace phase_a;
    // Sens OFF and the A-1 fallback both judge with w2_eff = 10000 — the
    // capped limit must EQUAL the uncapped eased value for defaults,
    // permissive values above the bar, and out-of-domain values the
    // defensive saturation in eased_flatness_limit already handles (raw F
    // for in-domain F <= 100; 100 for the domain-guarded F > 100):
    static constexpr uint8_t kCases[] = {0, 25, 84, FLAT_WIDE_BAR_PCT,
                                         90, 100, 150, 255};
    for (uint8_t f : kCases) {
        CHECK(flatness_judgment_limit(f, 10000u) ==
              eased_flatness_limit(f, 10000u));
    }
    // Permissive user value above the bar: never tightened below what was
    // asked — the cap only limits how far EASING may raise the limit:
    CHECK(flatness_judgment_limit(90, 225u) == 90);
    // User value just under the bar: the mid band clamps up to the bar
    // (F = 84 eased to 99 → 85), i.e. the bar rules whenever F < 85:
    CHECK(flatness_judgment_limit(84, 225u) == FLAT_WIDE_BAR_PCT);
    // At defaults the bar still allows easing ABOVE the raw F (gentle
    // weak-peak regime kept, just bounded):
    CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, 225u) >
          DEFAULT_SPECTRUM_FLATNESS);
    // ARMING SPLIT: at w2 = 100 (w = 10) the seat stays CLOSED even though
    // a capped comparison (85 < 99) would open it — arming must keep the
    // exact eased value or the F1 phantom seat reopens for plateaus <= 85:
    CHECK(!(eased_flatness_limit(DEFAULT_SPECTRUM_FLATNESS, 100u) <
            FLATNESS_EFFECTIVE_CEILING));
    CHECK(flatness_judgment_limit(DEFAULT_SPECTRUM_FLATNESS, 100u) ==
          FLAT_WIDE_BAR_PCT);  // the capped value exists, but the closed
                               // seat never reads it (run_flat == false;
                               // the fallback judges at w2_eff = 10000)
}
