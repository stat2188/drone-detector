commit 8a1ceca4f7e6ce5962ccc34ec23e93bca28e27c4
Author: Max2188 <maxstatic2188@gmail.com>
Date:   Fri Apr 17 15:42:34 2026 +0500

    11

diff --git a/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp b/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp
index fc789b92..30eb0c6b 100644
--- a/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp
+++ b/firmware/application/apps/enhanced_drone_analyzer/scanner.hpp
@@ -1284,6 +1284,11 @@ private:
     // Neighbor margin checker for anti-false-positive detection
     NeighborMarginChecker neighbor_margin_checker_;
 
+    // Spectrum shape parameters (runtime override of config values)
+    uint8_t spectrum_shape_margin_{DEFAULT_SPECTRUM_MARGIN};
+    uint8_t spectrum_shape_min_width_{DEFAULT_SPECTRUM_MIN_WIDTH};
+    uint8_t spectrum_shape_max_width_{DEFAULT_SPECTRUM_MAX_WIDTH};
+
     // Mahalanobis detector for statistical outlier detection (Sweep mode only)
     MahalanobisDetector mahalanobis_detector_;
 };
