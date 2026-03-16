#include "drone_legend_editor.hpp"
#include "ui_navigation.hpp"

namespace drone_analyzer {

// ============================================================================
// Constants
// ============================================================================

constexpr uint16_t HEADER_HEIGHT = 30;
constexpr uint16_t LIST_HEIGHT = 150;
constexpr uint16_t EDITOR_HEIGHT = 100;
constexpr uint16_t ROW_HEIGHT = 20;

// ============================================================================
// Constructor / Destructor
// ============================================================================

DroneLegendEditorView::DroneLegendEditorView(NavigationView& nav) noexcept
    : ui::View()
    , nav_(nav)
    , legend_entries_()
    , legend_count_(0)
    , selected_index_(0)
    , editing_mode_(false)
    , editing_entry_()
    , editing_field_(0)
    , line_buffer_{0}
    , header_height_(HEADER_HEIGHT)
    , list_height_(LIST_HEIGHT)
    , editor_height_(EDITOR_HEIGHT)
    , row_height_(ROW_HEIGHT) {
    
    // Load legend from file on construction
    load_legend_from_file();
}

DroneLegendEditorView::~DroneLegendEditorView() noexcept {
    // Destructor - no dynamic memory to free
}

// ============================================================================
// Paint Method
// ============================================================================

void DroneLegendEditorView::paint(ui::Painter& painter) {
    (void)painter;
    // Draw header
    draw_header(painter);
    
    uint16_t y_offset = header_height_;
    
    if (editing_mode_) {
        // Draw editor panel
        draw_editor_panel(painter, y_offset);
    } else {
        // Draw legend list
        draw_legend_list(painter, y_offset);
    }
}

// ============================================================================
// Drawing Methods
// ============================================================================

void DroneLegendEditorView::draw_header(ui::Painter& painter) noexcept {
    (void)painter;
    // TODO: Implement using ui::Painter
    // Draw "Edit Drone Legend" text
}

void DroneLegendEditorView::draw_legend_list(ui::Painter& painter, uint16_t start_y) noexcept {
    (void)painter;
    (void)start_y;
    
    uint16_t y = start_y;
    
    // Draw each entry
    for (uint8_t i = 0; i < legend_count_ && i < 10; ++i) {
        const DroneLegendEntry& entry = legend_entries_[i];
        
        // Highlight selected entry
        uint32_t bg_color = (i == selected_index_) ? 0xFF333333 : 0xFF000000;
        
        // Draw entry background
        // draw_rectangle(painter, 10, y, 220, row_height_, bg_color, true);
        
        // Draw entry text
        // Format: "2.432G  DJI    HIGH   Mavic Pro"
        // draw_text(painter, freq_str, 15, y + 2, COLOR_TEXT);
        // draw_text(painter, type_str, 100, y + 2, COLOR_TEXT);
        // draw_text(painter, threat_str, 140, y + 2, COLOR_TEXT);
        // draw_text(painter, desc_str, 185, y + 2, COLOR_TEXT);
        
        y += row_height_;
    }
}

void DroneLegendEditorView::draw_editor_panel(ui::Painter& painter, uint16_t start_y) noexcept {
    (void)painter;
    (void)start_y;
    
    // Draw editor fields
    // Field 0: Frequency
    // Field 1: Drone Type
    // Field 2: Threat Level
    // Field 3: Description
    
    // Highlight current field
    uint32_t field_color[4] = {
        (editing_field_ == 0) ? 0xFFFFFF00 : 0xFFFFFFFF,
        (editing_field_ == 1) ? 0xFFFFFF00 : 0xFFFFFFFF,
        (editing_field_ == 2) ? 0xFFFFFF00 : 0xFFFFFFFF,
        (editing_field_ == 3) ? 0xFFFFFF00 : 0xFFFFFFFF
    };
    
    // Draw field labels
    uint16_t y = start_y + 20;
    // draw_text(painter, "Frequency:", 10, y, COLOR_TEXT);
    y += 25;
    // draw_text(painter, "Drone Type:", 10, y, COLOR_TEXT);
    y += 25;
    // draw_text(painter, "Threat Level:", 10, y, COLOR_TEXT);
    y += 25;
    // draw_text(painter, "Description:", 10, y, COLOR_TEXT);
    
    // Draw field values
    y = start_y + 20;
    
    // Format frequency
    char freq_buffer[32];
    uint64_t freq_mhz = editing_entry_.frequency_hz / 1000000ULL;
    uint64_t freq_khz = (editing_entry_.frequency_hz % 1000000ULL) / 1000ULL;
    // TODO: Format as "2.432GHz"
    
    // Draw drone type
    // draw_text(painter, drone_type_to_string(editing_entry_.drone_type), 100, y, field_color[1]);
    y += 25;
    
    // Draw threat level
    // draw_text(painter, threat_level_to_string(editing_entry_.threat_level), 100, y, field_color[2]);
    y += 25;
    
    // Draw description
    // draw_text(painter, editing_entry_.description, 100, y, field_color[3]);
}

void DroneLegendEditorView::draw_legend_entry(ui::Painter& painter, const DroneLegendEntry& entry,
                                      uint16_t x, uint16_t y) noexcept {
    (void)painter;
    (void)entry;
    (void)x;
    (void)y;
    // TODO: Implement entry drawing
}

// ============================================================================
// Data Methods
// ============================================================================

ErrorCode DroneLegendEditorView::load_legend_from_file() noexcept {
    // Open DRONES.TXT
    // TODO: Implement file I/O using FileWrapper
    
    // For now, create default entries
    legend_count_ = 0;
    
    // Add some default entries
    legend_entries_[legend_count_++] = DroneLegendEntry{};
    legend_entries_[legend_count_ - 1].frequency_hz = 2406500000ULL;
    legend_entries_[legend_count_ - 1].drone_type = DroneType::DJI;
    legend_entries_[legend_count_ - 1].threat_level = ThreatLevel::HIGH;
    
    const char* desc = "Mavic Pro";
    for (uint8_t i = 0; i < DRONE_TYPE_NAME_LENGTH - 1 && desc[i] != '\0'; ++i) {
        legend_entries_[legend_count_ - 1].description[i] = desc[i];
    }
    legend_entries_[legend_count_ - 1].description[DRONE_TYPE_NAME_LENGTH - 1] = '\0';
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneLegendEditorView::save_legend_to_file() noexcept {
    // Open DRONES.TXT for writing
    // TODO: Implement file I/O using FileWrapper
    
    // Write header
    // writeline("# Drone Legend Database");
    // writeline("# Format: f=frequency,dt=type,tl=threat,d=description");
    
    // Write entries
    for (uint8_t i = 0; i < legend_count_; ++i) {
        format_entry_line(legend_entries_[i], line_buffer_, sizeof(line_buffer_));
        // writeline(line_buffer_);
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode DroneLegendEditorView::parse_legend_line(const char* line, DroneLegendEntry& entry) const noexcept {
    if (line == nullptr || line[0] == '\0' || line[0] == '#') {
        return ErrorCode::BUFFER_EMPTY;
    }
    
    // Parse format: f=frequency,dt=type,tl=threat,d=description
    
    // Initialize entry
    entry.frequency_hz = 0;
    entry.drone_type = DroneType::UNKNOWN;
    entry.threat_level = ThreatLevel::NONE;
    entry.description[0] = '\0';
    
    // Parse key-value pairs
    const char* ptr = line;
    while (*ptr != '\0') {
        // Skip whitespace
        while (*ptr == ' ' || *ptr == '\t') ptr++;
        if (*ptr == '\0') break;
        
        // Find key
        const char* key_start = ptr;
        while (*ptr != '=' && *ptr != ',' && *ptr != '\0') ptr++;
        size_t key_len = ptr - key_start;
        
        if (*ptr != '=') {
            // Skip to next comma
            while (*ptr != ',' && *ptr != '\0') ptr++;
            if (*ptr == ',') ptr++;
            continue;
        }
        
        ptr++;  // Skip '='
        
        // Find value
        const char* value_start = ptr;
        while (*ptr != ',' && *ptr != '\0') ptr++;
        size_t value_len = ptr - value_start;
        
        // Parse key-value pairs
        if (key_len == 1 && key_start[0] == 'f') {
            // Parse frequency
            FreqHz freq = 0;
            for (size_t i = 0; i < value_len && i < 15; ++i) {
                char c = value_start[i];
                if (c >= '0' && c <= '9') {
                    freq = freq * 10 + (c - '0');
                }
            }
            entry.frequency_hz = freq * 1000000ULL;
        }
        else if (key_len == 2 && key_start[0] == 'd' && key_start[1] == 't') {
            // Parse drone type from description
            // Copy description to entry.description
            size_t desc_len = value_len < DRONE_TYPE_NAME_LENGTH ? value_len : DRONE_TYPE_NAME_LENGTH - 1;
            for (size_t i = 0; i < desc_len; ++i) {
                entry.description[i] = value_start[i];
            }
            entry.description[desc_len] = '\0';
            
            // Try to detect drone type from description
            if (desc_len >= 3 && (value_start[0] == 'D' || value_start[0] == 'd')) {
                entry.drone_type = DroneType::DJI;
            }
        }
        else if (key_len == 2 && key_start[0] == 't' && key_start[1] == 'l') {
            // Parse threat level
            if (value_len > 0) {
                uint8_t level = value_start[0] - '0';
                if (level <= 4) {
                    entry.threat_level = static_cast<ThreatLevel>(level);
                }
            }
        }
        
        if (*ptr == ',') ptr++;
    }
    
    return validate_entry(entry) ? ErrorCode::SUCCESS : ErrorCode::INVALID_PARAMETER;
}

void DroneLegendEditorView::format_entry_line(const DroneLegendEntry& entry, char* buffer, size_t buffer_size) const noexcept {
    if (buffer == nullptr || buffer_size == 0) return;
    
    size_t i = 0;
    
    // Format: f=2405000000,dt=DJI,tl=3,d=Mavic Pro
    
    // Write "f="
    buffer[i++] = 'f';
    buffer[i++] = '=';
    
    // Write frequency (as Hz, no decimal for simplicity)
    FreqHz freq = entry.frequency_hz / 1000000ULL;
    
    // Convert to string (simplified)
    char temp[16];
    uint8_t temp_len = 0;
    if (freq == 0) {
        temp[temp_len++] = '0';
    } else {
        char rev[16];
        while (freq > 0 && temp_len < 15) {
            rev[temp_len++] = '0' + (freq % 10);
            freq /= 10;
        }
        // Reverse
        for (uint8_t j = 0; j < temp_len; ++j) {
            buffer[i++] = rev[temp_len - 1 - j];
        }
    }
    
    // Write ",dt="
    buffer[i++] = ',';
    buffer[i++] = 'd';
    buffer[i++] = 't';
    buffer[i++] = '=';
    
    // Write drone type as integer
    buffer[i++] = '0' + static_cast<uint8_t>(entry.drone_type);
    
    // Write ",tl="
    buffer[i++] = ',';
    buffer[i++] = 't';
    buffer[i++] = 'l';
    buffer[i++] = '=';
    
    // Write threat level
    buffer[i++] = '0' + static_cast<uint8_t>(entry.threat_level);
    
    // Write ",d="
    buffer[i++] = ',';
    buffer[i++] = 'd';
    buffer[i++] = '=';
    
    // Write description
    for (size_t j = 0; j < DRONE_TYPE_NAME_LENGTH && entry.description[j] != '\0'; ++j) {
        if (i < buffer_size - 1) {
            buffer[i++] = entry.description[j];
        }
    }
    
    // Null-terminate
    if (i < buffer_size) {
        buffer[i] = '\0';
    } else {
        buffer[buffer_size - 1] = '\0';
    }
}

// ============================================================================
// Editing Methods
// ============================================================================

void DroneLegendEditorView::start_editing(uint8_t index) noexcept {
    if (index >= legend_count_) {
        return;
    }
    
    editing_entry_ = legend_entries_[index];
    editing_mode_ = true;
    editing_field_ = 0;
    selected_index_ = index;
}

void DroneLegendEditorView::finish_editing() noexcept {
    if (validate_entry(editing_entry_)) {
        legend_entries_[selected_index_] = editing_entry_;
        editing_mode_ = false;
        
        // Save to file
        save_legend_to_file();
    }
}

void DroneLegendEditorView::cancel_editing() noexcept {
    editing_mode_ = false;
}

void DroneLegendEditorView::cycle_edit_field() noexcept {
    editing_field_ = (editing_field_ + 1) % 4;
}

void DroneLegendEditorView::increment_field_value() noexcept {
    switch (editing_field_) {
        case FIELD_FREQUENCY:
            editing_entry_.frequency_hz += 1000000ULL;  // +1 MHz
            if (editing_entry_.frequency_hz > MAX_FREQUENCY_HZ) {
                editing_entry_.frequency_hz = MAX_FREQUENCY_HZ;
            }
            break;
            
        case FIELD_DRONE_TYPE:
            editing_entry_.drone_type = cycle_drone_type(editing_entry_.drone_type);
            break;
            
        case FIELD_THREAT_LEVEL:
            editing_entry_.threat_level = cycle_threat_level(editing_entry_.threat_level);
            break;
            
        case FIELD_DESCRIPTION:
            // Increment description (limited implementation)
            // TODO: Implement text editing
            break;
    }
}

void DroneLegendEditorView::decrement_field_value() noexcept {
    switch (editing_field_) {
        case FIELD_FREQUENCY:
            if (editing_entry_.frequency_hz >= 1000000ULL) {
                editing_entry_.frequency_hz -= 1000000ULL;  // -1 MHz
            }
            if (editing_entry_.frequency_hz < MIN_FREQUENCY_HZ) {
                editing_entry_.frequency_hz = MIN_FREQUENCY_HZ;
            }
            break;
            
        case FIELD_DRONE_TYPE:
            // Reverse cycle
            uint8_t type_val = static_cast<uint8_t>(editing_entry_.drone_type);
            type_val = (type_val == 0) ? 8 : type_val - 1;
            editing_entry_.drone_type = static_cast<DroneType>(type_val);
            break;
            
        case FIELD_THREAT_LEVEL:
            // Reverse cycle
            uint8_t threat_val = static_cast<uint8_t>(editing_entry_.threat_level);
            threat_val = (threat_val == 0) ? 4 : threat_val - 1;
            editing_entry_.threat_level = static_cast<ThreatLevel>(threat_val);
            break;
            
        case FIELD_DESCRIPTION:
            // Decrement description (limited implementation)
            // TODO: Implement text editing
            break;
    }
}

DroneType DroneLegendEditorView::cycle_drone_type(DroneType type_in) const noexcept {
    uint8_t val = static_cast<uint8_t>(type_in);
    val = (val + 1) % 9;  // 0-8 (UNKNOWN to OTHER, skipping CUSTOM)
    
    if (val >= 8) val = 0;  // Skip OTHER in cycle
    
    return static_cast<DroneType>(val);
}

ThreatLevel DroneLegendEditorView::cycle_threat_level(ThreatLevel level_in) const noexcept {
    uint8_t val = static_cast<uint8_t>(level_in);
    val = (val + 1) % 5;  // 0-4 (NONE to CRITICAL)
    return static_cast<ThreatLevel>(val);
}
            break;
            
        case EditField::DRONE_TYPE:
            editing_entry_.drone_type = cycle_drone_type(editing_entry_.drone_type);
            break;
            
        case EditField::THREAT_LEVEL:
            editing_entry_.threat_level = cycle_threat_level(editing_entry_.threat_level);
            break;
            
        case EditField::DESCRIPTION:
            // Increment description (limited implementation)
            // TODO: Implement text editing
            break;
    }
}

void DroneLegendEditorView::decrement_field_value() noexcept {
    switch (editing_field_) {
        case EditField::FREQUENCY:
            if (editing_entry_.frequency_hz >= 1000000ULL) {
                editing_entry_.frequency_hz -= 1000000ULL;  // -1 MHz
            }
            if (editing_entry_.frequency_hz < MIN_FREQUENCY_HZ) {
                editing_entry_.frequency_hz = MIN_FREQUENCY_HZ;
            }
            break;
            
        case EditField::DRONE_TYPE:
            // Reverse cycle
            uint8_t current = static_cast<uint8_t>(editing_entry_.drone_type);
            current = (current == 0) ? 8 : current - 1;
            editing_entry_.drone_type = static_cast<DroneType>(current);
            break;
            
        case EditField::THREAT_LEVEL:
            // Reverse cycle
            uint8_t current = static_cast<uint8_t>(editing_entry_.threat_level);
            current = (current == 0) ? 4 : current - 1;
            editing_entry_.threat_level = static_cast<ThreatLevel>(current);
            break;
            
        case EditField::DESCRIPTION:
            // Decrement description (limited implementation)
            // TODO: Implement text editing
            break;
    }
}

// ============================================================================
// Navigation
// ============================================================================

void DroneLegendEditorView::select_next_entry() noexcept {
    if (editing_mode_) {
        return;
    }
    
    if (legend_count_ > 0) {
        selected_index_ = (selected_index_ + 1) % legend_count_;
    }
}

void DroneLegendEditorView::select_prev_entry() noexcept {
    if (editing_mode_) {
        return;
    }
    
    if (legend_count_ > 0) {
        selected_index_ = (selected_index_ == 0) ? legend_count_ - 1 : selected_index_ - 1;
    }
}

// ============================================================================
// Validation
// ============================================================================

bool DroneLegendEditorView::validate_entry(const DroneLegendEntry& entry) const noexcept {
    if (entry.frequency_hz < MIN_FREQUENCY_HZ || entry.frequency_hz > MAX_FREQUENCY_HZ) {
        return false;
    }
    
    if (entry.drone_type > DroneType::OTHER) {
        return false;
    }
    
    if (entry.threat_level > ThreatLevel::CRITICAL) {
        return false;
    }
    
    return entry.description[0] != '\0';
}

DroneType DroneLegendEditorView::cycle_drone_type(DroneType current) const noexcept {
    uint8_t val = static_cast<uint8_t>(current);
    val = (val + 1) % 9;  // 0-8 (UNKNOWN to OTHER, skipping CUSTOM)
    
    if (val >= 8) val = 0;  // Skip OTHER in cycle
    
    return static_cast<DroneType>(val);
}

ThreatLevel DroneLegendEditorView::cycle_threat_level(ThreatLevel current) const noexcept {
    uint8_t val = static_cast<uint8_t>(current);
    val = (val + 1) % 5;  // 0-4 (NONE to CRITICAL)
    return static_cast<ThreatLevel>(val);
}

const char* DroneLegendEditorView::drone_type_to_string(DroneType type) const noexcept {
    switch (type) {
        case DroneType::DJI:
            return "DJI";
        case DroneType::PARROT:
            return "Parrot";
        case DroneType::YUNEEC:
            return "Yuneec";
        case DroneType::DR_3DR:
            return "3DR";
        case DroneType::AUTEL:
            return "Autel";
        case DroneType::HOBBY:
            return "Hobby";
        case DroneType::FPV:
            return "FPV";
        case DroneType::CUSTOM:
            return "Custom";
        case DroneType::OTHER:
            return "Other";
        case DroneType::UNKNOWN:
        default:
            return "Unknown";
    }
}

const char* DroneLegendEditorView::threat_level_to_string(ThreatLevel level) const noexcept {
    switch (level) {
        case ThreatLevel::NONE:
            return "None";
        case ThreatLevel::LOW:
            return "Low";
        case ThreatLevel::MEDIUM:
            return "Medium";
        case ThreatLevel::HIGH:
            return "High";
        case ThreatLevel::CRITICAL:
            return "Critical";
        default:
            return "Unknown";
    }
}

// ============================================================================
// Button Handlers
// ============================================================================

void DroneLegendEditorView::on_save_clicked() noexcept {
    if (editing_mode_) {
        finish_editing();
    } else {
        save_legend_to_file();
    }
}

void DroneLegendEditorView::on_add_clicked() noexcept {
    if (legend_count_ < MAX_LEGEND_ENTRIES) {
        // Create new entry with default values
        DroneLegendEntry new_entry;
        new_entry.frequency_hz = 2400000000ULL;
        new_entry.drone_type = DroneType::UNKNOWN;
        new_entry.threat_level = ThreatLevel::LOW;
        
        const char* desc = "New Entry";
        for (uint8_t i = 0; i < DRONE_TYPE_NAME_LENGTH - 1 && desc[i] != '\0'; ++i) {
            new_entry.description[i] = desc[i];
        }
        new_entry.description[DRONE_TYPE_NAME_LENGTH - 1] = '\0';
        
        // Add to list
        legend_entries_[legend_count_++] = new_entry;
        
        // Start editing new entry
        selected_index_ = legend_count_ - 1;
        start_editing(selected_index_);
    }
}

void DroneLegendEditorView::on_delete_clicked() noexcept {
    if (editing_mode_ || legend_count_ == 0) {
        return;
    }
    
    // Shift entries up
    for (uint8_t i = selected_index_; i < legend_count_ - 1; ++i) {
        legend_entries_[i] = legend_entries_[i + 1];
    }
    
    legend_count_--;
    
    // Adjust selected index
    if (selected_index_ >= legend_count_ && legend_count_ > 0) {
        selected_index_ = legend_count_ - 1;
    }
    
    // Save changes
    save_legend_to_file();
}

void DroneLegendEditorView::on_exit_clicked() noexcept {
    // TODO: Navigate back to settings
    // nav_.pop();
}

} // namespace drone_analyzer
