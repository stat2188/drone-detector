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
// SweepProcessor gate regression tests (audit 2026-10).
//
// Target: the O1 ENTRY SKIP in DroneScannerUI::on_sweep_spectrum() — the
// CRITICAL defect where next_detectable_center()'s STRICTLY-ABOVE contract
// was misread as "the entry center is guard-zone":
//
//   sweep [2400, 2700] MHz + slot [2400, 2405] MHz
//   f_center_ini = f_min + 9.375 MHz detects [2400.000, 2419.45] ⊇ slot,
//   but next_detectable_center(ini) honestly returns the pass-completion
//   anchor (no grid point strictly above ini lands inside the slot's useful
//   interval) → the unguarded jump credited the whole window, retuned past
//   the slot and permanently elided the only slice covering it.
//
// The fix is SweepProcessor::span_intersects_active_slot() — the "is THIS
// center detectable?" query next() structurally cannot answer — sharing the
// slot_useful_interval() geometry with the walk so guard and walk can never
// drift apart. These tests pin:
//   1  CRITICAL: signal-bearing entry center → guard disarmed, jump blocked
//   2  genuine guard entry center → skip still jumps to a detectable center
//   3  no active slots → fail-open identity (skip disarmed)
//   4  inverted (malformed) slot → inactive → fail-open identity
//   5  slot outside the window → entry guard, jump to the completion anchor
//   6  oracle property: span_intersects ⇔ span intersects some active slot
//   7  next_detectable_center never stalls (strictly advances)
// ============================================================================

#include "doctest.h"
#include <cstdint>

#include "sweep_processor.hpp"

TEST_CASE("entry-skip guard: signal-bearing entry center is never treated as guard (CRITICAL)") {
    using namespace drone_analyzer;

    // Sweep [2400, 2700] MHz with the slot hugging the window's bottom edge.
    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {2400, 0, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {2405, 0, 0, 0, 0};
    const FreqHz f_min = 2400ULL * MHZ;
    const FreqHz f_max = 2700ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;
    const FreqHz ini = f_min + SWEEP_FIRST_SLICE_LEAD_HZ;  // 2,409,375,000

    // The entry center's detectable span [ini − LEAD, ini + REACH] =
    // [2400.000, 2419.45] MHz ⊇ [2400, 2405] MHz → SIGNAL-BEARING.
    // The entry-skip must consult this probe and stay disarmed.
    CHECK(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, ini));

    // Why the probe is REQUIRED: the walk only searches STRICTLY above ini,
    // finds no grid point inside the slot's useful interval
    // [2389.92, 2414.375] MHz (ini + step = 2418.20 MHz > 2414.375 MHz) and
    // honestly reports the pass-completion anchor. Read naively as "entry is
    // guard-zone", this jump would skip the slot on EVERY pass (f_center_ini
    // is preserved by reset()).
    const FreqHz entry_next = SweepProcessor::next_detectable_center(
        start, end, DETECTION_WINDOWS_PER_WINDOW, f_min, f_max, step, ini);
    CHECK(entry_next > ini + step);  // the false jump the guard now blocks

    // The jump target cannot see the slot at all — i.e. taking it would lose
    // the ONLY slice covering [2400, 2405] MHz.
    CHECK(entry_next - SWEEP_FIRST_SLICE_LEAD_HZ > 2405ULL * MHZ);
}

TEST_CASE("genuine guard entry center: skip still jumps to the first detectable center") {
    using namespace drone_analyzer;

    // Slot parked mid-window: the entry center is genuinely guard-zone.
    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {2500, 0, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {2505, 0, 0, 0, 0};
    const FreqHz f_min = 2400ULL * MHZ;
    const FreqHz f_max = 2700ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;
    const FreqHz ini = f_min + SWEEP_FIRST_SLICE_LEAD_HZ;

    // [ini − LEAD, ini + REACH] = [2400.000, 2419.45] MHz misses
    // [2500, 2505] MHz → the entry-skip may fire.
    CHECK_FALSE(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, ini));

    // First grid center whose span reaches the slot: ceil((lo − REACH − ini)
    // / step) = ceil(80,546,875 / 8,828,125) = 10 → ini + 10·step.
    const FreqHz expected = ini + 10 * step;  // 2,497,656,250
    const FreqHz next_c = SweepProcessor::next_detectable_center(
        start, end, DETECTION_WINDOWS_PER_WINDOW, f_min, f_max, step, ini);
    CHECK(next_c == expected);
    CHECK(next_c > ini + step);  // genuine jump
    // The jump lands ON a detectable center (idempotent for later passes:
    // reset() keeps f_center_ini = next_c and the guard then disarms).
    CHECK(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, next_c));
}

TEST_CASE("no active slots → fail-open identity (skip disarmed)") {
    using namespace drone_analyzer;

    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {0, 0, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {0, 0, 0, 0, 0};
    const FreqHz f_min = 2400ULL * MHZ;
    const FreqHz f_max = 2700ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;
    const FreqHz ini = f_min + SWEEP_FIRST_SLICE_LEAD_HZ;

    // Detector fully open → every center counts as detectable.
    CHECK(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, ini));
    CHECK(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, f_max));

    // Identity: plain advance-by-one, bit-identical to the historical
    // `f_center += step_hz`.
    CHECK(SweepProcessor::next_detectable_center(
              start, end, DETECTION_WINDOWS_PER_WINDOW, f_min, f_max, step, ini)
          == ini + step);
}

TEST_CASE("inverted (malformed) slot is inactive → fail-open identity") {
    using namespace drone_analyzer;

    // start > end must count as INACTIVE (constants.hpp rule), never as a
    // slot that kills the whole band.
    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {2500, 0, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {2400, 0, 0, 0, 0};
    const FreqHz f_min = 2400ULL * MHZ;
    const FreqHz f_max = 2700ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;
    const FreqHz ini = f_min + SWEEP_FIRST_SLICE_LEAD_HZ;

    CHECK(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, ini));
    CHECK(SweepProcessor::next_detectable_center(
              start, end, DETECTION_WINDOWS_PER_WINDOW, f_min, f_max, step, ini)
          == ini + step);
}

TEST_CASE("slot outside the window below → entry guard, jump to the completion anchor") {
    using namespace drone_analyzer;

    // [10, 15] MHz sits far below the window: nothing in [2400, 2700] MHz is
    // detectable for this slot, so the whole pass is guard-zone and the
    // entry-skip may jump straight to the pass-completion anchor.
    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {10, 0, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {15, 0, 0, 0, 0};
    const FreqHz f_min = 2400ULL * MHZ;
    const FreqHz f_max = 2700ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;
    const FreqHz ini = f_min + SWEEP_FIRST_SLICE_LEAD_HZ;

    // Useful interval floor clamps at 0 (lo − REACH < 0), ceiling
    // 15 MHz + 9.375 MHz < ini → entry center is guard.
    CHECK_FALSE(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, ini));

    // Anchor = first grid center ≥ window_hi − SLICE_BW/2 =
    // ceil((2,690,000,000 − ini) / step) = 32 → ini + 32·step.
    const FreqHz anchor = ini + 32 * step;  // 2,691,875,000
    CHECK(SweepProcessor::next_detectable_center(
              start, end, DETECTION_WINDOWS_PER_WINDOW, f_min, f_max, step, ini)
          == anchor);
}

TEST_CASE("oracle: span_intersects_active_slot ⇔ detectable span intersects an active slot") {
    using namespace drone_analyzer;

    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {0, 2500, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {0, 2505, 0, 0, 0};
    const FreqHz lo = 2500ULL * MHZ;
    const FreqHz hi = 2505ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;
    const FreqHz first = 2400ULL * MHZ + SWEEP_FIRST_SLICE_LEAD_HZ;

    // Independent oracle: span [c − LEAD, c + REACH] intersects [lo, hi]
    // iff (c − LEAD <= hi) && (c + REACH >= lo) — computed WITHOUT the
    // interval form slot_useful_interval() uses internally.
    for (uint32_t i = 0; i <= 40; ++i) {
        const FreqHz c = first + static_cast<FreqHz>(i) * step;
        const bool expected =
            ((c - SWEEP_FIRST_SLICE_LEAD_HZ) <= hi) && ((c + SWEEP_SLICE_REACH_HIGH_HZ) >= lo);
        CHECK(SweepProcessor::span_intersects_active_slot(start, end, DETECTION_WINDOWS_PER_WINDOW, c) == expected);
    }
}

TEST_CASE("next_detectable_center always advances strictly (no stall)") {
    using namespace drone_analyzer;

    const uint32_t start[DETECTION_WINDOWS_PER_WINDOW] = {2400, 2500, 0, 0, 0};
    const uint32_t end[DETECTION_WINDOWS_PER_WINDOW] = {2405, 2505, 0, 0, 0};
    const FreqHz f_min = 2400ULL * MHZ;
    const FreqHz f_max = 2700ULL * MHZ;
    const FreqHz step = SWEEP_GAPLESS_STEP_MAX_HZ;

    FreqHz from = f_min + SWEEP_FIRST_SLICE_LEAD_HZ;
    for (uint32_t i = 0; i < 48; ++i) {
        const FreqHz next_c = SweepProcessor::next_detectable_center(
            start, end, DETECTION_WINDOWS_PER_WINDOW, f_min, f_max, step, from);
        CHECK(next_c > from);
        from = next_c;
    }
}
