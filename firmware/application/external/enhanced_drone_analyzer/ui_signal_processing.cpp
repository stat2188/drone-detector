#include "ui_signal_processing.hpp"

namespace ui::external_app::enhanced_drone_analyzer {

// Global DetectionRingBuffer instance for thread-safe access across components
DetectionRingBuffer global_detection_ring;
// Backward compatibility alias
DetectionRingBuffer& local_detection_ring = global_detection_ring;

} // namespace ui::external_app::enhanced_drone_analyzer
