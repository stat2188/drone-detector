/*
 * Copyright (C) 2024 Mayhem Firmware Contributors
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

#include "fullscreen_tv_view.hpp"
#include "portapack.hpp"
#include "string_format.hpp"

#include <string>

using namespace portapack;

namespace ui::external_app::analogtv {

FullscreenTvView::FullscreenTvView(NavigationView& nav)
    : nav_(nav) {
    // Initialize with fullscreen rect
    set_parent_rect({0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT});

    // Add buttons to view
    add_children({&button_scan, &button_mode, &button_up, &button_down});

    // Setup button callbacks
    button_scan.on_select = [this](Button&) {
        if (scan_state_ == ScanState::SCANNING) {
            stop_auto_scan();
            button_scan.set_text("Scan");
        } else {
            start_auto_scan();
            button_scan.set_text("Stop");
        }
    };

    button_mode.on_select = [this](Button&) {
        toggle_scan_mode();
    };

    button_up.on_select = [this](Button&) {
        frequency_up();
    };

    button_down.on_select = [this](Button&) {
        frequency_down();
    };
}

FullscreenTvView::~FullscreenTvView() {
    stop_auto_scan();
    baseband::shutdown();
    receiver_model.disable();
}

void FullscreenTvView::paint(Painter& painter) {
    // Clear screen to black
    painter.fill_rectangle({0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT}, Color::black());

    // Render video if active (fullscreen, above buttons)
    if (video_active_) {
        render_video_frame();
    }

    // Draw scan status overlay if scanning
    if (scan_state_ == ScanState::SCANNING) {
        draw_scan_status(painter);
    }

    // Draw frequency information at top
    draw_frequency_info(painter);
}

void FullscreenTvView::focus() {
    // Set focus to this view
}

void FullscreenTvView::on_show() {
    initialize();
    baseband::spectrum_streaming_start();
    receiver_model.enable();
    
    // Start auto-scan by default
    start_auto_scan();
}

void FullscreenTvView::on_hide() {
    stop_auto_scan();
    baseband::spectrum_streaming_stop();
    receiver_model.disable();
}

void FullscreenTvView::set_parent_rect(const Rect new_parent_rect) {
    View::set_parent_rect(new_parent_rect);
}

void FullscreenTvView::initialize() {
    // Initialize hardware - run baseband image
    baseband::run_prepared_image(portapack::memory::map::m4_code.base());
    
    // Set default modulation and bandwidth
    receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    receiver_model.set_sampling_rate(2000000);
    receiver_model.set_baseband_bandwidth(2000000);
    
    // Clear video buffer
    video_processor_.clear_buffer();

    // Set initial state
    scan_state_ = ScanState::IDLE;
    current_scan_frequency_ = SCAN_START_HZ;
    video_active_ = false;
}

void FullscreenTvView::start_auto_scan() {
    if (scan_state_ == ScanState::SCANNING) {
        return;  // Already scanning
    }

    scan_state_ = ScanState::SCANNING;
    scan_mode_ = ScanMode::AUTO_SCAN;
    current_scan_frequency_ = SCAN_START_HZ;
    scan_timer_ = 0;
    dwell_timer_ = 0;
    carrier_detected_ = false;

    // Start scanning
    scan_next_frequency();
}

void FullscreenTvView::stop_auto_scan() {
    scan_state_ = ScanState::IDLE;
    scan_timer_ = 0;
    dwell_timer_ = 0;
}

void FullscreenTvView::scan_next_frequency() {
    if (scan_state_ != ScanState::SCANNING) {
        return;
    }

    // Tune to next frequency using receiver_model
    receiver_model.set_target_frequency(current_scan_frequency_);
    
    // Update displayed frequency
    displayed_frequency_ = current_scan_frequency_;
    
    // Reset dwell timer
    dwell_timer_ = 0;
}

void FullscreenTvView::on_carrier_found(uint64_t freq_hz) {
    // Stop scanning
    stop_auto_scan();
    
    // Lock onto carrier
    scan_state_ = ScanState::LOCKED;
    carrier_detected_ = true;
    
    // Tune to found frequency
    receiver_model.set_target_frequency(freq_hz);
    displayed_frequency_ = freq_hz;
    
    // Enable video display
    video_active_ = true;
}

void FullscreenTvView::update_scan_status() {
    // Handle scanning state
    if (scan_state_ == ScanState::SCANNING) {
        dwell_timer_ += UI_REFRESH_INTERVAL_MS;
        
        // Check if dwell time expired
        if (dwell_timer_ >= DWELL_TIME_MS) {
            // Move to next frequency
            current_scan_frequency_ += SCAN_STEP_HZ;
            if (current_scan_frequency_ > SCAN_END_HZ) {
                current_scan_frequency_ = SCAN_START_HZ;
            }
            scan_next_frequency();
        }
    }
}

void FullscreenTvView::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    // Guard clause: check if spectrum FIFO is valid
    if (!spectrum_fifo_) {
        return;
    }

    // Process spectrum data
    video_processor_.process_spectrum(spectrum);

    // Check for video carrier if scanning
    if (scan_state_ == ScanState::SCANNING) {
        if (video_processor_.detect_video_carrier(spectrum.db)) {
            on_carrier_found(current_scan_frequency_);
        }
    }

    // Update scan status
    update_scan_status();
}

void FullscreenTvView::render_video_frame() {
    // Check if frame is ready
    if (!video_processor_.is_frame_ready()) {
        return;
    }

    // Render video fullscreen (scaled to 240x280, leaving room for controls)
    // Original video is 128x104, we scale up ~1.875x horizontally and ~2.69x vertically
    const uint16_t video_display_height = 280;  // Leave 40px for controls
    const uint16_t video_display_width = 240;

    for (uint16_t y = 0; y < video_display_height; y++) {
        // Map display Y to source video Y (with scaling)
        const uint16_t src_y = (y * VIDEO_HEIGHT) / video_display_height;
        
        // Get line buffer from video processor
        ui::Color line_buffer[LINE_BUFFER_SIZE];
        video_processor_.render_video_line(src_y, line_buffer);

        // Scale horizontally: map 128 pixels to 240 pixels
        ui::Color scaled_line[240];
        for (uint16_t x = 0; x < video_display_width; x++) {
            const uint16_t src_x = (x * VIDEO_WIDTH) / video_display_width;
            scaled_line[x] = line_buffer[src_x];
        }

        // Render scaled line to display
        display.render_line({0, static_cast<Coord>(y)}, video_display_width, scaled_line);
    }

    // Reset frame ready flag
    video_processor_.reset_frame_ready();
}

void FullscreenTvView::draw_scan_status(Painter& painter) {
    // Draw scan status overlay
    const Rect status_rect{10, 10, 220, 30};
    
    // Semi-transparent background
    painter.fill_rectangle(status_rect, Color::black());
    
    // Draw status text
    const char* status_text = "";
    switch (scan_state_) {
        case ScanState::SCANNING:
            status_text = "Scanning...";
            break;
        case ScanState::CARRIER_FOUND:
            status_text = "Carrier Found!";
            break;
        case ScanState::LOCKED:
            status_text = "Locked";
            break;
        case ScanState::ERROR:
            status_text = "Error";
            break;
        default:
            status_text = "Ready";
            break;
    }
    
    painter.draw_string({15, 15}, *Theme::getInstance()->fg_green, status_text);
}

void FullscreenTvView::draw_frequency_info(Painter& painter) {
    // Draw frequency information at top of screen
    const Rect freq_rect{0, 0, DISPLAY_WIDTH, 24};
    
    // Semi-transparent background
    painter.fill_rectangle(freq_rect, Color::black());
    
    // Draw frequency
    const auto freq_mhz = displayed_frequency_ / 1000000;
    const auto freq_frac = (displayed_frequency_ % 1000000) / 10000;
    painter.draw_string({4, 4}, *Theme::getInstance()->fg_yellow, 
                        to_string_dec_uint(freq_mhz) + "." + to_string_dec_uint(freq_frac) + " MHz");
    
    // Draw scan mode
    const char* mode_str = (scan_mode_ == ScanMode::AUTO_SCAN) ? "AUTO" : "MANUAL";
    painter.draw_string({DISPLAY_WIDTH - 60, 4}, *Theme::getInstance()->fg_light, mode_str);

    // Draw scan state
    const char* state_str = "";
    switch (scan_state_) {
        case ScanState::SCANNING: state_str = "SCANNING"; break;
        case ScanState::LOCKED: state_str = "LOCKED"; break;
        case ScanState::ERROR: state_str = "ERROR"; break;
        default: state_str = "READY"; break;
    }
    painter.draw_string({120, 4}, *Theme::getInstance()->fg_green, state_str);
}

bool FullscreenTvView::on_key(const KeyEvent event) {
    if (event == KeyEvent::Left) {
        frequency_down();
        return true;
    }
    if (event == KeyEvent::Right) {
        frequency_up();
        return true;
    }
    if (event == KeyEvent::Up) {
        toggle_scan_mode();
        return true;
    }
    return false;
}

bool FullscreenTvView::on_encoder(const EncoderEvent delta) {
    if (scan_mode_ == ScanMode::AUTO_SCAN) {
        return false;
    }
    if (delta > 0) {
        frequency_up();
    } else {
        frequency_down();
    }
    return true;
}

void FullscreenTvView::toggle_scan_mode() {
    if (scan_mode_ == ScanMode::AUTO_SCAN) {
        scan_mode_ = ScanMode::MANUAL;
        stop_auto_scan();
        button_mode.set_text("Manual");
        button_scan.set_text("Scan");
    } else {
        scan_mode_ = ScanMode::AUTO_SCAN;
        button_mode.set_text("Auto");
    }
    set_dirty();
}

void FullscreenTvView::frequency_up() {
    if (scan_mode_ == ScanMode::AUTO_SCAN) {
        return;  // Don't allow manual tuning in auto mode
    }
    
    current_scan_frequency_ += SCAN_STEP_HZ;
    if (current_scan_frequency_ > SCAN_END_HZ) {
        current_scan_frequency_ = SCAN_START_HZ;
    }
    
    receiver_model.set_target_frequency(current_scan_frequency_);
    displayed_frequency_ = current_scan_frequency_;
    set_dirty();
}

void FullscreenTvView::frequency_down() {
    if (scan_mode_ == ScanMode::AUTO_SCAN) {
        return;  // Don't allow manual tuning in auto mode
    }
    
    if (current_scan_frequency_ < SCAN_START_HZ + SCAN_STEP_HZ) {
        current_scan_frequency_ = SCAN_END_HZ;
    } else {
        current_scan_frequency_ -= SCAN_STEP_HZ;
    }
    
    receiver_model.set_target_frequency(current_scan_frequency_);
    displayed_frequency_ = current_scan_frequency_;
    set_dirty();
}

}  // namespace ui::external_app::analogtv
