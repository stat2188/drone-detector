#ifndef PATTERN_MANAGER_VIEW_HPP
#define PATTERN_MANAGER_VIEW_HPP

#include <cstdint>
#include <cstddef>
#include <cstring>

#include "ui_painter.hpp"
#include "ui_widget.hpp"
#include "ui_navigation.hpp"

#include "pattern_manager.hpp"
#include "pattern_types.hpp"
#include "scanner.hpp"
#include "constants.hpp"

namespace drone_analyzer {

class PatternManagerView : public ui::View {
public:
    explicit PatternManagerView(NavigationView& nav) noexcept;
    ~PatternManagerView() noexcept override;

    PatternManagerView(const PatternManagerView&) = delete;
    PatternManagerView& operator=(const PatternManagerView&) = delete;

    void paint(ui::Painter& painter) override;
    void focus() override;
    void on_show() override;
    void on_hide() override;

    std::string title() const override { return "PTR Pattern"; }

private:
    static constexpr uint16_t SPECTRUM_Y = 40;
    static constexpr uint16_t SPECTRUM_HEIGHT = 100;
    static constexpr uint16_t LIST_Y = 150;
    static constexpr uint16_t LIST_HEIGHT = 120;

    static constexpr uint8_t AVG_PASSES = 5;
    static constexpr uint8_t MAX_SPECTRUM_FIFO = 8;

    enum class ViewState : uint8_t {
        IDLE,
        CAPTURING,
        SAVING
    };

    NavigationView& nav_;

    PatternManager* pattern_manager_ptr_{nullptr};

    ui::Labels labels_;
    ui::OptionsField field_patterns_;
    ui::Button button_add_;
    ui::Button button_save_;
    ui::Button button_edit_;
    ui::Button button_delete_;
    ui::Button button_clear_all_;
    ui::Button button_back_;

    ui::Button button_start_capture_;
    ui::Labels label_status_;

    uint8_t selected_index_{0};
    ViewState view_state_{ViewState::IDLE};

    FreqHz capture_frequency_{0};
    uint8_t capture_spectrum_[FFT_BIN_COUNT]{};
    uint8_t capture_spectrum_avg_[FFT_BIN_COUNT]{};
    uint8_t spectrum_fifo_[MAX_SPECTRUM_FIFO][FFT_BIN_COUNT]{};
    uint8_t fifo_index_{0};
    uint8_t fifo_count_{0};
    bool capture_active_{false};
    uint8_t capture_pass_{0};
    SystemTime capture_start_time_{0};

    int16_t selected_bin_{-1};
    bool bin_selected_{false};

    ErrorCode perform_capture(FreqHz freq) noexcept;
    void update_spectrum_display() noexcept;
    void draw_spectrum_with_selection(ui::Painter& painter, const uint8_t* spectrum, int16_t sel_bin) noexcept;
    ErrorCode save_current_pattern(const char* name) noexcept;
    void show_pattern_details() noexcept;
    void delete_selected_pattern() noexcept;
    void clear_all_patterns() noexcept;
    void start_capture_sequence() noexcept;
    void on_capture_complete() noexcept;

    MessageHandlerRegistration message_handler_spectrum_config;
    MessageHandlerRegistration message_handler_frame_sync;
    ChannelSpectrumFIFO* spectrum_fifo_{nullptr};

    void on_channel_spectrum_config(ChannelSpectrumFIFO* fifo) noexcept;
    void on_frame_sync() noexcept;
};

} // namespace drone_analyzer

#endif