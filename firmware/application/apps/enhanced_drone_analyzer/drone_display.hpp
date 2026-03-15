#ifndef DRONE_DISPLAY_HPP
#define DRONE_DISPLAY_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "drone_types.hpp"
#include "constants.hpp"
#include "error_handler.hpp"

// Forward declaration for UI framework
namespace ui {
    class View;
    class Painter;
    class Rect;
}

namespace drone_analyzer {

/**
 * @brief Display component for drone analyzer UI
 * @note Inherits from ui::View, renders spectrum, histogram, drone list, status bar
 * @note Uses static storage for large buffers to reduce stack usage
 * @note Removed features: mini spectrum, RSSI history, frequency ruler, detailed info
 */
class DroneDisplay : public ui::View {
public:
    /**
     * @brief Default constructor
     */
    DroneDisplay() noexcept;

    /**
     * @brief Destructor
     */
    ~DroneDisplay() noexcept override;

    /**
     * @brief Paint method - main rendering entry point
     * @note Uses static storage for large buffers to reduce stack usage
     * @note Stack optimized: large buffers in static storage
     */
    void paint(Painter& painter) override;

    /**
     * @brief Render spectrum display
     * @param painter Painter instance for drawing
     * @param spectrum_data Spectrum data buffer
     * @param spectrum_size Size of spectrum data
     * @param start_x Starting X coordinate
     * @param start_y Starting Y coordinate
     * @param width Display width
     * @param height Display height
     */
    void render_spectrum(
        Painter& painter,
        const uint8_t* spectrum_data,
        size_t spectrum_size,
        uint16_t start_x,
        uint16_t start_y,
        uint16_t width,
        uint16_t height
    ) noexcept;

    /**
     * @brief Render histogram display
     * @param painter Painter instance for drawing
     * @param histogram_data Histogram data buffer
     * @param histogram_size Size of histogram data
     * @param start_x Starting X coordinate
     * @param start_y Starting Y coordinate
     * @param width Display width
     * @param height Display height
     */
    void render_histogram(
        Painter& painter,
        const uint16_t* histogram_data,
        size_t histogram_size,
        uint16_t start_x,
        uint16_t start_y,
        uint16_t width,
        uint16_t height
    ) noexcept;

    /**
     * @brief Render drone list display
     * @param painter Painter instance for drawing
     * @param drones Array of drone entries to display
     * @param drone_count Number of drones to display
     * @param start_x Starting X coordinate
     * @param start_y Starting Y coordinate
     * @param width Display width
     * @param height Display height
     */
    void render_drone_list(
        Painter& painter,
        const DisplayDroneEntry* drones,
        size_t drone_count,
        uint16_t start_x,
        uint16_t start_y,
        uint16_t width,
        uint16_t height
    ) noexcept;

    /**
     * @brief Render status bar
     * @param painter Painter instance for drawing
     * @param status_text Status text to display
     * @param start_x Starting X coordinate
     * @param start_y Starting Y coordinate
     * @param width Display width
     * @param height Display height
     */
    void render_status_bar(
        Painter& painter,
        const char* status_text,
        uint16_t start_x,
        uint16_t start_y,
        uint16_t width,
        uint16_t height
    ) noexcept;

    /**
     * @brief Update display data
     * @param display_data New display data
     * @return ErrorCode::SUCCESS if updated, error code otherwise
     */
    ErrorCode update_display_data(const DisplayData& display_data) noexcept;

    /**
     * @brief Get current display data
     * @return Reference to current display data
     */
    [[nodiscard]] const DisplayData& get_display_data() const noexcept;

    /**
     * @brief Clear display
     * @param painter Painter instance for drawing
     */
    void clear_display(Painter& painter) noexcept;

    /**
     * @brief Set spectrum data for rendering
     * @param spectrum_data Spectrum data buffer
     * @param spectrum_size Size of spectrum data
     * @return ErrorCode::SUCCESS if set, error code otherwise
     */
    ErrorCode set_spectrum_data(
        const uint8_t* spectrum_data,
        size_t spectrum_size
    ) noexcept;

    /**
     * @brief Set histogram data for rendering
     * @param histogram_data Histogram data buffer
     * @param histogram_size Size of histogram data
     * @return ErrorCode::SUCCESS if set, error code otherwise
     */
    ErrorCode set_histogram_data(
        const uint16_t* histogram_data,
        size_t histogram_size
    ) noexcept;

    /**
     * @brief Set status text
     * @param status_text Status text to display
     */
    void set_status_text(const char* status_text) noexcept;

    /**
     * @brief Get current status text
     * @return Current status text
     */
    [[nodiscard]] const char* get_status_text() const noexcept;

private:
    /**
     * @brief Draw spectrum line
     * @param painter Painter instance for drawing
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Line color
     */
    void draw_spectrum_line(
        Painter& painter,
        uint16_t x,
        uint16_t y,
        uint32_t color
    ) noexcept;

    /**
     * @brief Draw histogram bar
     * @param painter Painter instance for drawing
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Bar width
     * @param height Bar height
     * @param color Bar color
     */
    void draw_histogram_bar(
        Painter& painter,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height,
        uint32_t color
    ) noexcept;

    /**
     * @brief Draw drone entry
     * @param painter Painter instance for drawing
     * @param drone Drone entry to draw
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Entry width
     * @param height Entry height
     */
    void draw_drone_entry(
        Painter& painter,
        const DisplayDroneEntry& drone,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height
    ) noexcept;

    /**
     * @brief Draw text
     * @param painter Painter instance for drawing
     * @param text Text to draw
     * @param x X coordinate
     * @param y Y coordinate
     * @param color Text color
     */
    void draw_text(
        Painter& painter,
        const char* text,
        uint16_t x,
        uint16_t y,
        uint32_t color
    ) noexcept;

    /**
     * @brief Draw rectangle
     * @param painter Painter instance for drawing
     * @param x X coordinate
     * @param y Y coordinate
     * @param width Rectangle width
     * @param height Rectangle height
     * @param color Rectangle color
     * @param fill Fill rectangle if true, outline only if false
     */
    void draw_rectangle(
        Painter& painter,
        uint16_t x,
        uint16_t y,
        uint16_t width,
        uint16_t height,
        uint32_t color,
        bool fill = true
    ) noexcept;

    /**
     * @brief Map RSSI value to display height
     * @param rssi RSSI value in dBm
     * @param max_height Maximum display height
     * @return Mapped height in pixels
     */
    [[nodiscard]] uint16_t map_rssi_to_height(
        RssiValue rssi,
        uint16_t max_height
    ) const noexcept;

    /**
     * @brief Get color for threat level
     * @param threat Threat level
     * @return Color value
     */
    [[nodiscard]] uint32_t get_threat_color(ThreatLevel threat) const noexcept;

    /**
     * @brief Format frequency for display
     * @param frequency Frequency in Hz
     * @param buffer Buffer to store formatted string
     * @param buffer_size Buffer size
     */
    void format_frequency(
        FreqHz frequency,
        char* buffer,
        size_t buffer_size
    ) const noexcept;

    /**
     * @brief Format RSSI for display
     * @param rssi RSSI value in dBm
     * @param buffer Buffer to store formatted string
     * @param buffer_size Buffer size
     */
    void format_rssi(
        RssiValue rssi,
        char* buffer,
        size_t buffer_size
    ) const noexcept;

    /**
     * @brief Validate spectrum data
     * @param spectrum_data Spectrum data buffer
     * @param spectrum_size Size of spectrum data
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_spectrum_data(
        const uint8_t* spectrum_data,
        size_t spectrum_size
    ) const noexcept;

    /**
     * @brief Validate histogram data
     * @param histogram_data Histogram data buffer
     * @param histogram_size Size of histogram data
     * @return ErrorCode::SUCCESS if valid, error code otherwise
     */
    [[nodiscard]] ErrorCode validate_histogram_data(
        const uint16_t* histogram_data,
        size_t histogram_size
    ) const noexcept;

    /**
     * @brief Clamp value to range
     * @param value Value to clamp
     * @param min Minimum value
     * @param max Maximum value
     * @return Clamped value
     */
    [[nodiscard]] uint16_t clamp(
        int32_t value,
        int32_t min,
        int32_t max
    ) const noexcept;

private:
    // Display data (static storage, no heap allocation)
    DisplayData display_data_;

    // Spectrum data buffer (static storage for stack optimization)
    std::array<uint8_t, SPECTRUM_BUFFER_SIZE> spectrum_buffer_;

    // Histogram data buffer (static storage for stack optimization)
    std::array<uint16_t, HISTOGRAM_BUFFER_SIZE> histogram_buffer_;

    // Status text buffer
    char status_text_[MAX_TEXT_LENGTH];

    // Spectrum data size
    size_t spectrum_data_size_;

    // Histogram data size
    size_t histogram_data_size_;

    // Display flags
    bool spectrum_visible_;
    bool histogram_visible_;
    bool drone_list_visible_;
    bool status_bar_visible_;

    // Display dimensions
    uint16_t spectrum_height_;
    uint16_t histogram_height_;
    uint16_t drone_list_height_;
    uint16_t status_bar_height_;
};

} // namespace drone_analyzer

#endif // DRONE_DISPLAY_HPP
