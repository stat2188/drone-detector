#ifndef DRONE_LEGEND_EDITOR_HPP
#define DRONE_LEGEND_EDITOR_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "ui.hpp"
#include "ui_painter.hpp"
#include "ui_widget.hpp"
#include "drone_types.hpp"
#include "constants.hpp"
#include "error_handler.hpp"
#include "database.hpp"

namespace drone_analyzer {

/**
 * @brief Drone legend entry structure (fixed-size, no heap)
 * @note Total: 40 bytes
 */
struct DroneLegendEntry {
    FreqHz frequency_hz;
    DroneType drone_type;
    ThreatLevel threat_level;
    char description[DRONE_TYPE_NAME_LENGTH];
    
    constexpr DroneLegendEntry() noexcept
        : frequency_hz(0)
        , drone_type(DroneType::UNKNOWN)
        , threat_level(ThreatLevel::NONE)
        , description{0} {
    }
    
    constexpr bool is_valid() const noexcept {
        return frequency_hz >= MIN_FREQUENCY_HZ &&
               frequency_hz <= MAX_FREQUENCY_HZ &&
               description[0] != '\0';
    }
};

static_assert(sizeof(DroneLegendEntry) == 32, "DroneLegendEntry size mismatch");

/**
 * @brief UI for editing drone legend
 * @note Inherits from ui::View
 * @note Diamond Code: no heap, fixed-size arrays only
 * @note Stack budget: <4KB total
 */
class DroneLegendEditorView : public ui::View {
public:
    explicit DroneLegendEditorView(NavigationView& nav) noexcept;
    ~DroneLegendEditorView() noexcept override;
    
    DroneLegendEditorView(const DroneLegendEditorView&) = delete;
    DroneLegendEditorView& operator=(const DroneLegendEditorView&) = delete;
    
    void paint(ui::Painter& painter) override;
    
private:
    NavigationView& nav_;
    
    // Legend database (stack-allocated, fixed-size)
    static constexpr size_t MAX_LEGEND_ENTRIES = 50;
    std::array<DroneLegendEntry, MAX_LEGEND_ENTRIES> legend_entries_;
    uint8_t legend_count_;
    
    // Current selection
    uint8_t selected_index_;
    bool editing_mode_;
    
    // Current entry being edited
    DroneLegendEntry editing_entry_;
    uint8_t editing_field_;  // 0=freq, 1=type, 2=threat, 3=desc
    
    // Constants for editing fields (instead of enum to avoid switch issues)
    static constexpr uint8_t FIELD_FREQUENCY = 0;
    static constexpr uint8_t FIELD_DRONE_TYPE = 1;
    static constexpr uint8_t FIELD_THREAT_LEVEL = 2;
    static constexpr uint8_t FIELD_DESCRIPTION = 3;
    
    // Line buffer for file I/O (stack-allocated)
    static constexpr size_t LINE_BUFFER_SIZE = 128;
    char line_buffer_[LINE_BUFFER_SIZE];
    
    // UI dimensions
    uint16_t header_height_;
    uint16_t list_height_;
    uint16_t editor_height_;
    uint16_t row_height_;
    
    // Drawing methods
    void draw_header(ui::Painter& painter) noexcept;
    void draw_legend_list(ui::Painter& painter, uint16_t start_y) noexcept;
    void draw_editor_panel(ui::Painter& painter, uint16_t start_y) noexcept;
    void draw_legend_entry(ui::Painter& painter, const DroneLegendEntry& entry,
                         uint16_t x, uint16_t y) noexcept;
    
    // Data methods
    ErrorCode load_legend_from_file() noexcept;
    ErrorCode save_legend_to_file() noexcept;
    ErrorCode parse_legend_line(const char* line, DroneLegendEntry& entry) const noexcept;
    void format_entry_line(const DroneLegendEntry& entry, char* buffer, size_t buffer_size) const noexcept;
    
    // Editing methods
    void start_editing(uint8_t index) noexcept;
    void finish_editing() noexcept;
    void cancel_editing() noexcept;
    void cycle_edit_field() noexcept;
    void increment_field_value() noexcept;
    void decrement_field_value() noexcept;
    
    // Navigation
    void select_next_entry() noexcept;
    void select_prev_entry() noexcept;
    
    // Validation
    bool validate_entry(const DroneLegendEntry& entry) const noexcept;
    DroneType cycle_drone_type(DroneType current) const noexcept;
    ThreatLevel cycle_threat_level(ThreatLevel current) const noexcept;
    const char* drone_type_to_string(DroneType type) const noexcept;
    const char* threat_level_to_string(ThreatLevel level) const noexcept;
    
    // Button handlers
    void on_save_clicked() noexcept;
    void on_add_clicked() noexcept;
    void on_delete_clicked() noexcept;
    void on_exit_clicked() noexcept;
};

} // namespace drone_analyzer

#endif // DRONE_LEGEND_EDITOR_HPP
