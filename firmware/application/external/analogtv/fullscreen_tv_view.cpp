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

#include <cstdio>

using namespace portapack;

namespace ui::external_app::analogtv {

FullscreenTvView::FullscreenTvView(NavigationView& nav)
    : nav_(nav) {
    set_parent_rect({0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT});
    add_children({&button_scan, &button_mode, &button_up, &button_down});

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
    // Render video fullscreen (320 rows, LUT-optimized)
    if (video_active_) {
        renderer_.render_frame();
    } else {
        painter.fill_rectangle(
            {0, 0, DISPLAY_WIDTH, DISPLAY_HEIGHT},
            Color::black());
    }

    // Draw frequency overlay on top of video
    draw_frequency_info(painter);
}

void FullscreenTvView::focus() {
}

void FullscreenTvView::on_show() {
    initialize();
    baseband::spectrum_streaming_start();
    receiver_model.enable();
    start_auto_scan();
}

void FullscreenTvView::on_hide() {
    stop_auto_scan();
    baseband::spectrum_streaming_stop();
    receiver_model.disable();
}

void FullscreenTvView::initialize() {
    baseband::run_prepared_image(portapack::memory::map::m4_code.base());

    receiver_model.set_modulation(ReceiverModel::Mode::WidebandFMAudio);
    receiver_model.set_sampling_rate(DEFAULT_SAMPLE_RATE_HZ);
    receiver_model.set_baseband_bandwidth(DEFAULT_BASEBAND_BANDWIDTH_HZ);

    renderer_.clear_buffer();

    scan_state_ = ScanState::IDLE;
    current_scan_frequency_ = SCAN_START_HZ;
    video_active_ = false;
}

void FullscreenTvView::start_auto_scan() {
    if (scan_state_ == ScanState::SCANNING) {
        return;
    }

    scan_state_ = ScanState::SCANNING;
    scan_mode_ = ScanMode::AUTO_SCAN;
    current_scan_frequency_ = SCAN_START_HZ;
    dwell_timer_ = 0;
    carrier_detected_ = false;

    scan_next_frequency();
}

void FullscreenTvView::stop_auto_scan() {
    scan_state_ = ScanState::IDLE;
    dwell_timer_ = 0;
}

void FullscreenTvView::scan_next_frequency() {
    if (scan_state_ != ScanState::SCANNING) {
        return;
    }

    receiver_model.set_target_frequency(current_scan_frequency_);
    displayed_frequency_ = current_scan_frequency_;
    dwell_timer_ = 0;
}

void FullscreenTvView::on_carrier_found(uint64_t freq_hz) {
    stop_auto_scan();
    scan_state_ = ScanState::LOCKED;
    carrier_detected_ = true;

    receiver_model.set_target_frequency(freq_hz);
    displayed_frequency_ = freq_hz;

    video_active_ = true;
}

void FullscreenTvView::update_scan_status() {
    if (scan_state_ == ScanState::SCANNING) {
        dwell_timer_ += UI_REFRESH_INTERVAL_MS;

        if (dwell_timer_ >= DWELL_TIME_MS) {
            current_scan_frequency_ += SCAN_STEP_HZ;
            if (current_scan_frequency_ > SCAN_END_HZ) {
                current_scan_frequency_ = SCAN_START_HZ;
            }
            scan_next_frequency();
        }
    }
}

void FullscreenTvView::on_channel_spectrum(const ChannelSpectrum& spectrum) {
    if (!spectrum_fifo_) {
        return;
    }

    renderer_.process_spectrum(spectrum);

    if (scan_state_ == ScanState::SCANNING) {
        if (renderer_.detect_video_carrier(spectrum.db)) {
            on_carrier_found(current_scan_frequency_);
        }
    }

    update_scan_status();
}

void FullscreenTvView::draw_frequency_info(Painter& painter) {
    // Stack-allocated frequency display — no heap allocation.
    // Format: "1234.56 MHz  [STATE]"
    char freq_buf[24];
    const auto freq_mhz = displayed_frequency_ / 1000000;
    const auto freq_frac = (displayed_frequency_ % 1000000) / 10000;

    std::snprintf(freq_buf, sizeof(freq_buf), "%lu.%02lu MHz",
                  static_cast<unsigned long>(freq_mhz),
                  static_cast<unsigned long>(freq_frac));

    // Background bar
    painter.fill_rectangle({0, 0, DISPLAY_WIDTH, 20}, Color::black());

    // Frequency text
    painter.draw_string({4, 4}, *Theme::getInstance()->fg_yellow, freq_buf);

    // Scan state indicator
    const char* state_str = "";
    switch (scan_state_) {
        case ScanState::SCANNING: state_str = "SCAN"; break;
        case ScanState::LOCKED: state_str = "LOCK"; break;
        case ScanState::CARRIER_FOUND: state_str = "FND!"; break;
        case ScanState::ERROR: state_str = "ERR"; break;
        default: state_str = "RDY"; break;
    }
    painter.draw_string({DISPLAY_WIDTH - 60, 4},
                        *Theme::getInstance()->fg_green, state_str);

    // Mode indicator
    const char* mode_str = (scan_mode_ == ScanMode::AUTO_SCAN) ? "A" : "M";
    painter.draw_string({DISPLAY_WIDTH - 20, 4},
                        *Theme::getInstance()->fg_light, mode_str);
}

bool FullscreenTvView::on_key(const KeyEvent event) {
    switch (event) {
        case KeyEvent::Left:
            frequency_down();
            return true;
        case KeyEvent::Right:
            frequency_up();
            return true;
        case KeyEvent::Up:
            toggle_scan_mode();
            return true;
        case KeyEvent::Select:
            if (scan_state_ == ScanState::SCANNING) {
                stop_auto_scan();
                button_scan.set_text("Scan");
            } else {
                start_auto_scan();
                button_scan.set_text("Stop");
            }
            return true;
        default:
            return false;
    }
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
        return;
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
        return;
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
