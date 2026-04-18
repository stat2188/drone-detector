commit f9272bfd9a3188764efd62c14a456b79d89bd4f7
Author: Max2188 <maxstatic2188@gmail.com>
Date:   Fri Apr 17 13:47:36 2026 +0500

    pattern comparsion methods

diff --git a/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp b/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp
index 78182a4a..fc789b92 100644
--- a/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp
+++ b/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp
@@ -3,6 +3,9 @@
 
 #include <cstdint>
 #include <cstddef>
+#include <cstring>
+#include <cstdlib>
+#include <cstdio>
 #include <array>
 #include "ch.h"
 #include "drone_types.hpp"
@@ -17,6 +20,9 @@
 #include "message.hpp"
 #include "spectrum_shape.hpp"
 #include "mahalanobis_gate.hpp"
+#include "pattern_types.hpp"
+#include "pattern_matcher.hpp"
+#include "pattern_manager.hpp"
 
 namespace drone_analyzer {
 
@@ -78,7 +84,11 @@ struct ScanConfig {
     int32_t neighbor_margin_db{DEFAULT_NEIGHBOR_MARGIN_DB};  // 0=disabled, 3=default
     bool rssi_variance_enabled{false};                        // RSSI variance noise rejection
     uint8_t confirm_count{DEFAULT_CONFIRM_COUNT};             // Configurable confirm count
-
+    
+    // Pattern matching features (frequency-independent signal recognition)
+    bool pattern_matching_enabled{false};                      // Enable pattern-based detection
+    uint8_t pattern_min_correlation{150};                      // Minimum correlation to match (0-255)
+    
     // CFAR detection (Constant False Alarm Rate)
     CFARMode cfar_mode{DEFAULT_CFAR_MODE};                    // CFAR mode (OFF/CA/GO/SO/HYBRID/OS/VI)
     uint8_t cfar_ref_cells{DEFAULT_CFAR_REF_CELLS};          // Reference cells (8-64)
@@ -89,7 +99,7 @@ struct ScanConfig {
     uint8_t cfar_hybrid_gamma{DEFAULT_CFAR_HYBRID_GAMMA};    // SO weight (0-100)
     uint8_t os_cfar_k_percent{DEFAULT_OS_CFAR_K_PERCENT};    // OS-CFAR k-th order (50-90%)
     uint8_t vi_cfar_threshold_x10{DEFAULT_VI_CFAR_THRESHOLD_X10};  // VI-CFAR threshold ×10 (5-50)
-
+    
     // Sweep exception frequencies (per window, 0 = unused slot)
     FreqHz sweep_exceptions[4][EXCEPTIONS_PER_WINDOW]{};
     uint8_t exception_radius_mhz{DEFAULT_EXCEPTION_RADIUS_MHZ};  // 1-100, configurable exclusion radius
@@ -1013,17 +1023,66 @@ public:
     }
 
     /**
-     * @brief Get lock timeout counter (for monitoring and debugging)
-     * @return Number of times scanner force-resumed due to lock timeout
-     * @note Thread-safe: acquires mutex (LockOrder::DATA_MUTEX)
-     * @note Useful for detecting noisy frequency conditions (e.g., 2400 MHz)
+     * @brief Set spectrum shape filter parameters (margin, min_width, max_width)
+     * @param margin Minimum peak margin above noise floor (bins below this are suppressed)
+     * @param min_width Minimum signal width in bins (isolated spikes removed)
+     * @param max_width Maximum signal width (flat noise rejected)
      */
-    [[nodiscard]] uint32_t get_lock_timeout_count() const noexcept {
-        MutexTryLock<LockOrder::DATA_MUTEX> lock(mutex_);
-        if (!lock.is_locked()) {
-            return lock_timeout_count_;
+    void set_spectrum_shape_params(uint8_t margin, uint8_t min_width, uint8_t max_width) noexcept {
+        spectrum_shape_margin_ = margin;
+        spectrum_shape_min_width_ = min_width;
+        spectrum_shape_max_width_ = max_width;
+    }
+    
+    // ========================================================================
+    // Pattern Matching Methods
+    // ========================================================================
+    
+    /**
+     * @brief Get reference to pattern matcher
+     * @return Reference to PatternMatcher instance
+     */
+    [[nodiscard]] PatternMatcher& get_pattern_matcher() noexcept {
+        return pattern_matcher_;
+    }
+    
+    /**
+     * @brief Save a pattern to storage
+     * @param pattern Pattern to save
+     * @return ErrorCode::SUCCESS if saved, error code otherwise
+     */
+    [[nodiscard]] ErrorCode save_pattern(const SignalPattern& pattern) noexcept {
+        return pattern_manager_.save_pattern(pattern);
+    }
+    
+    /**
+     * @brief Load all patterns from storage
+     * @return ErrorCode::SUCCESS if loaded, error code otherwise
+     */
+    [[nodiscard]] ErrorCode load_patterns() noexcept {
+        const ErrorCode err = pattern_manager_.load_patterns();
+        if (err == ErrorCode::SUCCESS) {
+            const SignalPattern* patterns = pattern_manager_.get_patterns_array();
+            const size_t count = pattern_manager_.get_pattern_count();
+            pattern_matcher_.set_patterns(patterns, count);
         }
-        return lock_timeout_count_;
+        return err;
+    }
+    
+    /**
+     * @brief Get loaded patterns array
+     * @return Pointer to patterns array
+     */
+    [[nodiscard]] const SignalPattern* get_patterns() const noexcept {
+        return pattern_manager_.get_patterns_array();
+    }
+    
+    /**
+     * @brief Get number of loaded patterns
+     * @return Pattern count
+     */
+    [[nodiscard]] size_t get_pattern_count() const noexcept {
+        return pattern_manager_.get_pattern_count();
     }
 
 private:
@@ -1120,6 +1179,11 @@ private:
     DatabaseManager& database_;
     HardwareController& hardware_;
     
+    // Pattern matching support
+    PatternManager pattern_manager_;
+    PatternMatcher pattern_matcher_;
+    uint8_t pattern_sort_buf_[256];  // Sort buffer for pattern extraction
+    
     // Scanner state
     ScannerState state_;
     
