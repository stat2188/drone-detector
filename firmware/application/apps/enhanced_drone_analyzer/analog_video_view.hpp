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

#ifndef ANALOG_VIDEO_VIEW_HPP
#define ANALOG_VIDEO_VIEW_HPP

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>

#include "ui_widget.hpp"
#include "ui_navigation.hpp"
#include "message.hpp"
#include "drone_types.hpp"
#include "constants.hpp"

namespace drone_analyzer {

/**
 * @brief Analog video rendering widget — memory-optimized for 128KB SRAM.
 *
 * Accumulates 13 × 256-byte ChannelSpectrum frames (3.3KB buffer) into
 * 26 native lines, line-doubled to 52 display lines.
 * Memory reduced 75% vs the 51-frame approach; FPS sacrificed for RAM.
 *
 * Memory:
 *   Instance: ~3.3KB (video_buffer_[3328] + state ~32B)
 *   Stack per render_frame(): ~520 bytes (line_buffer on stack)
 *   Flash: ~512 bytes (code)
 */
class VideoWidget : public ui::Widget {
public:
    VideoWidget();

    VideoWidget(const VideoWidget&) = delete;
    VideoWidget& operator=(const VideoWidget&) = delete;
    VideoWidget(VideoWidget&&) = delete;
    VideoWidget& operator=(VideoWidget&&) = delete;

    void on_show() override;
    void on_hide() override;
    void paint(Painter& painter) override;
    void focus() override;

    /**
     * @brief Push a ChannelSpectrum sample for video accumulation
     * @param spectrum Incoming 256-byte spectrum data
     * @note Stack: 0 bytes
     * @note Called from DisplayFrameSync handler (UI thread)
     * @note Accumulates 13 frames (26 native lines), then renders
     */
    void on_channel_spectrum(const ChannelSpectrum& spectrum) noexcept;

    void x_correction_increment(int8_t delta) noexcept;
    [[nodiscard]] uint8_t x_correction() const noexcept { return x_correction_; }

    void show_audio_spectrum_view(bool) const noexcept {}

private:
    // 13 frames × 256 = 3328 → 26 native lines of 128px each → line-doubled to 52 display lines
    static constexpr uint16_t VIDEO_LINES = 52;          // Display lines after line-doubling
    static constexpr uint16_t VIDEO_LINES_HALF = 26;     // Native lines in buffer
    static constexpr uint8_t ACCUMULATED_FRAMES = 13;    // Frames per video field (min RAM, FPS sacrificed)
    static constexpr size_t VIDEO_BUFFER_SIZE = 3328;    // 13 × 256 = 26 × 128 (no waste)
    static constexpr uint16_t LINE_WIDTH = 128;           // Pixels per video line

    static constexpr uint8_t DEFAULT_X_CORRECTION = 10;
    static constexpr uint8_t MAX_X_CORRECTION = 31;

    /** @brief Frame buffer — instance member, ~3.3KB */
    uint8_t video_buffer_[VIDEO_BUFFER_SIZE]{};

    uint32_t frame_count_{0};    //!< Number of spectra accumulated (0..12)
    bool active_{false};          //!< Rendering active
    uint8_t x_correction_{DEFAULT_X_CORRECTION};   //!< Horizontal correction offset

    /**
     * @brief Render accumulated frame to display using line-doubling
     * @note Stack: ~520 bytes (line_buffer on stack, 512B + locals)
     * @note Uses display.render_line() for direct pixel write
     * @note 26 native lines → 52 display lines (each line ×2)
     */
    void render_frame() noexcept;
};

/**
 * @brief Simplified analog video view with frequency header
 *
 * Loads AM TV baseband, configures receiver, registers ChannelSpectrum handlers,
 * and forwards data to VideoWidget for rendering.
 *
 * Memory:
 *   Instance: ~3,900 bytes (VideoWidget ~3.3KB + spectrum_buffer_ ~272B + handler_storage ~128B + state ~100B)
 *   Stack per paint(): ~48 bytes (freq_str[16] + locals)
 *   Stack per frame_sync handler: ~520 bytes (line_buffer in render_frame)
 *   Flash: ~768 bytes (code)
 *
 * @note No audio, no gain controls
 * @note 52 display lines (26 native ×2) — low-res, memory-optimized
 * @note Press BACK to exit
 */
class AnalogVideoView : public ui::View {
public:
    static constexpr uint16_t HEADER_H = 16;

    explicit AnalogVideoView(NavigationView& nav, FreqHz frequency) noexcept;
    ~AnalogVideoView() noexcept override;

    AnalogVideoView(const AnalogVideoView&) = delete;
    AnalogVideoView& operator=(const AnalogVideoView&) = delete;

    void on_show() override;
    void on_hide() override;
    void paint(Painter& painter) override;
    void set_parent_rect(const Rect new_parent_rect) override;
    void focus() override;
    bool on_key(const KeyEvent key) override;
    bool on_encoder(const EncoderEvent delta) override;

    std::string title() const override {
        return "FPV Video";
    }

    [[nodiscard]] bool is_valid() const noexcept;

private:
    NavigationView& nav_;
    FreqHz frequency_{0};

    // Video rendering widget (~3.3KB)
    VideoWidget video_widget_{};

    bool receiver_active_{false};

    /** @brief Storage for MessageHandlerRegistration — placement new for manual lifetime */
    struct HandlerStorage {
        MessageHandlerRegistration spectrum_config;
        MessageHandlerRegistration frame_sync;
    };
    static_assert(sizeof(HandlerStorage) <= 128, "HandlerStorage > 128 bytes");
    alignas(alignof(HandlerStorage)) uint8_t handler_storage_[sizeof(HandlerStorage)];
    bool handlers_active_{false};
    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};

    // Reusable buffer to prevent 272-byte stack allocation in frame_sync handler.
    // Same pattern as DroneScannerUI::spectrum_buffer_ — class member instead of local.
    ChannelSpectrum spectrum_buffer_{};

    void register_handlers() noexcept;
    void unregister_handlers() noexcept;
    void setup_video_receiver() noexcept;
    void restore_receiver() noexcept;
    [[nodiscard]] static bool is_frequency_valid(FreqHz freq) noexcept;
};

}  // namespace drone_analyzer

#endif  // ANALOG_VIDEO_VIEW_HPP
