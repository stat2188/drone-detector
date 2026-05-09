/*
 * Copyright (C) 2025 PortaPack Mayhem Edition Contributors
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program. If not, see <https://www.gnu.org/licenses/>.
 *
 */

#include "analog_video_view.hpp"

#include <cstring>
#include <cstdio>
#include "baseband_api.hpp"
#include "portapack.hpp"
#include "audio.hpp"

namespace drone_analyzer {

// ============================================================================
// Validation
// ============================================================================

[[nodiscard]] bool AnalogVideoView::is_frequency_valid(FreqHz freq) noexcept {
    // Stack: ~0 bytes (no locals)
    // Flash: ~12 bytes (two comparisons)
    return (freq >= MIN_FREQUENCY_HZ) && (freq <= MAX_FREQUENCY_HZ);
}

// ============================================================================
// Constructor / Destructor
// ============================================================================

AnalogVideoView::AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept
    : nav_(nav)
    , frequency_(frequency)
    , receiver_active_(false) {

    tv_widget_.show_audio_spectrum_view(false);

    add_child(&tv_widget_);

    tv_widget_.set_parent_rect({0, HEADER_H, 240, 320 - HEADER_H});
}

AnalogVideoView::~AnalogVideoView() noexcept {
    restore_receiver();
}

// ============================================================================
// Lifecycle
// ============================================================================

void AnalogVideoView::on_show() {
    // Guard: prevent double-initialization if on_show called twice
    if (receiver_active_) {
        return;
    }
    
    if (!is_frequency_valid(frequency_)) {
        // Invalid frequency — skip hardware config, show empty screen
        // paint() will display "Invalid Freq" header
        receiver_active_ = false;
        tv_widget_.on_show();
        return;
    }
    
    setup_video_receiver();
    tv_widget_.on_show();
    receiver_active_ = true;
}

void AnalogVideoView::on_hide() {
    tv_widget_.on_hide();
    restore_receiver();
    receiver_active_ = false;
}

// ============================================================================
// Rendering
// ============================================================================

void AnalogVideoView::paint(Painter& painter) {
    // Stack: ~48 bytes (freq_str[24] + locals)
    
    // First paint children (tv_widget_), then overlay header
    View::paint(painter);

    // Draw header background
    painter.fill_rectangle({0, 0, 240, HEADER_H}, Color::black());

    // Format frequency safely
    char freq_str[24];
    if (!is_frequency_valid(frequency_)) {
        strncpy(freq_str, "Invalid Freq", sizeof(freq_str) - 1);
        freq_str[sizeof(freq_str) - 1] = '\0';
    } else {
        const uint32_t mhz = static_cast<uint32_t>(frequency_ / 1'000'000ULL);
        const uint32_t khz = static_cast<uint32_t>((frequency_ % 1'000'000ULL) / 1'000ULL);
        // snprintf with static buffer — safe, no heap
        snprintf(freq_str, sizeof(freq_str), "%lu.%03lu MHz",
                 static_cast<unsigned long>(mhz),
                 static_cast<unsigned long>(khz));
    }

    painter.draw_string(
        Point{4, 4},
        Theme::getInstance()->fg_light->font,
        Theme::getInstance()->fg_light->foreground,
        Color::black(),
        freq_str);
}

void AnalogVideoView::focus() {
    tv_widget_.focus();  // Delegate focus to video widget for keyboard interaction
}

// ============================================================================
// Hardware Control
// ============================================================================

void AnalogVideoView::setup_video_receiver() noexcept {
    // Stack: ~0 bytes (no locals)
    
    audio::output::mute();
    baseband::shutdown();

    // baseband::run_prepared_image expects a valid address.
    // HackRF m4_code base should always be non-zero; no null check needed since
    // the linker provides this address at build time.
    baseband::run_prepared_image(portapack::memory::map::m4_code.base());

    portapack::receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    portapack::receiver_model.set_sampling_rate(2000000);
    portapack::receiver_model.set_baseband_bandwidth(2000000);
    portapack::receiver_model.set_target_frequency(frequency_);
    portapack::receiver_model.enable();
}

void AnalogVideoView::restore_receiver() noexcept {
    // Stack: ~0 bytes (no locals)
    
    audio::output::mute();
    portapack::receiver_model.disable();
    baseband::shutdown();
}

[[nodiscard]] bool AnalogVideoView::is_valid() const noexcept {
    return receiver_active_ && is_frequency_valid(frequency_);
}

}  // namespace drone_analyzer