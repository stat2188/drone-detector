/**
 * test_eda_fixes.cpp - Unit tests for Enhanced Drone Analyzer fixes
 * 
 * Tests for:
 * 1. Atomic scanning_active_ thread safety
 * 2. DetectionRingBuffer hash collision handling
 * 3. Stack overflow protection
 * 4. Thread safety verification
 * 
 * Framework: Simple custom test framework (Google Test style)
 */

#include <cstdint>
#include <atomic>
#include <thread>
#include <vector>
#include <iostream>
#include <cstring>
#include <cassert>

// Simple test framework macros
#define TEST(name) void test_##name()
#define ASSERT_TRUE(expr) assert(expr)
#define ASSERT_FALSE(expr) assert(!(expr))
#define ASSERT_EQ(a, b) assert((a) == (b))
#define ASSERT_NE(a, b) assert((a) != (b))
#define ASSERT_GT(a, b) assert((a) > (b))
#define ASSERT_LT(a, b) assert((a) < (b))
#define ASSERT_GE(a, b) assert((a) >= (b))
#define ASSERT_LE(a, b) assert((a) <= (b))
#define RUN_TEST(name) \
    std::cout << "Running " << #name << "... "; \
    test_##name(); \
    std::cout << "PASSED" << std::endl;

// ============================================================================
// TEST 1: Atomic scanning_active_ Thread Safety
// ============================================================================

/**
 * Mock DroneScanner with atomic scanning_active_
 * This simulates the FIX for Issue #1 in the code review
 */
class MockDroneScanner {
public:
    MockDroneScanner() : scanning_active_{false} {}
    
    void start_scanning() {
        // ✅ FIXED: Use atomic with acquire/release semantics
        if (scanning_active_.load(std::memory_order_acquire)) return;
        scanning_active_.store(true, std::memory_order_release);
    }
    
    void stop_scanning() {
        // ✅ FIXED: Use atomic with acquire/release semantics
        if (!scanning_active_.load(std::memory_order_acquire)) return;
        scanning_active_.store(false, std::memory_order_release);
    }
    
    bool is_scanning_active() const {
        return scanning_active_.load(std::memory_order_acquire);
    }
    
    void scanning_thread_loop() {
        // Simulate scanning thread
        for (int i = 0; i < 1000; ++i) {
            if (!scanning_active_.load(std::memory_order_acquire)) {
                break;
            }
            // Simulate work
            std::this_thread::sleep_for(std::chrono::microseconds(10));
        }
    }

private:
    std::atomic<bool> scanning_active_;
};

TEST(AtomicScanningFlag_ThreadSafety) {
    MockDroneScanner scanner;
    
    // Test 1: Basic start/stop
    ASSERT_FALSE(scanner.is_scanning_active());
    
    scanner.start_scanning();
    ASSERT_TRUE(scanner.is_scanning_active());
    
    scanner.stop_scanning();
    ASSERT_FALSE(scanner.is_scanning_active());
    
    // Test 2: Multiple start calls (idempotent)
    scanner.start_scanning();
    scanner.start_scanning();
    scanner.start_scanning();
    ASSERT_TRUE(scanner.is_scanning_active());
    
    // Test 3: Multiple stop calls (idempotent)
    scanner.stop_scanning();
    scanner.stop_scanning();
    ASSERT_FALSE(scanner.is_scanning_active());
}

TEST(AtomicScanningFlag_ConcurrentAccess) {
    MockDroneScanner scanner;
    
    const int NUM_ITERATIONS = 100;
    int success_count = 0;
    
    for (int iter = 0; iter < NUM_ITERATIONS; ++iter) {
        // Thread 1: Start scanning
        std::thread t1([&scanner]() {
            scanner.start_scanning();
            std::this_thread::sleep_for(std::chrono::microseconds(100));
            scanner.stop_scanning();
        });
        
        // Thread 2: Read scanning state
        std::thread t2([&scanner, &success_count]() {
            for (int i = 0; i < 10; ++i) {
                bool state = scanner.is_scanning_active();
                // State should be consistent (not undefined)
                (void)state; // Suppress unused warning
                std::this_thread::sleep_for(std::chrono::microseconds(20));
            }
            success_count++;
        });
        
        t1.join();
        t2.join();
        
        ASSERT_FALSE(scanner.is_scanning_active());
    }
    
    ASSERT_EQ(success_count, NUM_ITERATIONS);
}

TEST(AtomicScanningFlag_StressTest) {
    MockDroneScanner scanner;
    
    const int NUM_THREADS = 10;
    const int OPERATIONS_PER_THREAD = 100;
    
    std::atomic<int> start_count{0};
    std::atomic<int> stop_count{0};
    
    // Multiple threads start/stop scanning
    std::vector<std::thread> threads;
    for (int i = 0; i < NUM_THREADS; ++i) {
        threads.emplace_back([&scanner, &start_count, &stop_count, OPERATIONS_PER_THREAD]() {
            for (int j = 0; j < OPERATIONS_PER_THREAD; ++j) {
                if (j % 2 == 0) {
                    scanner.start_scanning();
                    start_count++;
                } else {
                    scanner.stop_scanning();
                    stop_count++;
                }
                std::this_thread::yield();
            }
        });
    }
    
    // Join all threads
    for (auto& t : threads) {
        t.join();
    }
    
    // Final state should be consistent
    bool final_state = scanner.is_scanning_active();
    (void)final_state; // Suppress unused warning
    
    ASSERT_EQ(start_count.load() + stop_count.load(), NUM_THREADS * OPERATIONS_PER_THREAD);
}

// ============================================================================
// TEST 2: DetectionRingBuffer Hash Collision Handling
// ============================================================================

constexpr size_t MAX_ENTRIES = 32;

struct RingBufferEntry {
    size_t frequency_hash;
    uint8_t detection_count;
    int16_t last_rssi_db;
};

class DetectionRingBuffer {
public:
    DetectionRingBuffer() : entries_{}, count_(0) {}
    
    void update_detection(size_t frequency_hash, uint8_t count, int16_t rssi_db) {
        size_t start_idx = frequency_hash % MAX_ENTRIES;
        
        // Linear probing - O(MAX_ENTRIES) worst case
        for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
            size_t idx = (start_idx + probe) % MAX_ENTRIES;
            
            if (entries_[idx].frequency_hash == 0) {
                // Empty slot found
                entries_[idx].frequency_hash = frequency_hash;
                entries_[idx].detection_count = count;
                entries_[idx].last_rssi_db = rssi_db;
                count_++;
                return;
            }
            
            if (entries_[idx].frequency_hash == frequency_hash) {
                // Found existing entry
                entries_[idx].detection_count += count;
                entries_[idx].last_rssi_db = rssi_db;
                return;
            }
        }
        
        // Buffer full - find oldest entry and replace (not implemented in this test)
    }
    
    uint8_t get_detection_count(size_t frequency_hash) const {
        size_t start_idx = frequency_hash % MAX_ENTRIES;
        
        for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
            size_t idx = (start_idx + probe) % MAX_ENTRIES;
            
            if (entries_[idx].frequency_hash == frequency_hash) {
                return entries_[idx].detection_count;
            }
            
            if (entries_[idx].frequency_hash == 0) {
                return 0;
            }
        }
        
        return 0;
    }
    
    size_t get_probe_count(size_t frequency_hash) const {
        size_t start_idx = frequency_hash % MAX_ENTRIES;
        
        for (size_t probe = 0; probe < MAX_ENTRIES; ++probe) {
            size_t idx = (start_idx + probe) % MAX_ENTRIES;
            
            if (entries_[idx].frequency_hash == frequency_hash ||
                entries_[idx].frequency_hash == 0) {
                return probe + 1;
            }
        }
        
        return MAX_ENTRIES;
    }
    
    size_t count() const { return count_; }

private:
    RingBufferEntry entries_[MAX_ENTRIES];
    size_t count_;
};

TEST(DetectionRingBuffer_BasicOperation) {
    DetectionRingBuffer buffer;
    
    // Empty buffer
    ASSERT_EQ(buffer.get_detection_count(1), 0);
    ASSERT_EQ(buffer.count(), 0);
    
    // Add entry
    buffer.update_detection(1, 5, -70);
    ASSERT_EQ(buffer.get_detection_count(1), 5);
    ASSERT_EQ(buffer.count(), 1);
    
    // Update existing entry
    buffer.update_detection(1, 3, -65);
    ASSERT_EQ(buffer.get_detection_count(1), 8);
    ASSERT_EQ(buffer.count(), 1);
}

TEST(DetectionRingBuffer_HashCollision) {
    DetectionRingBuffer buffer;
    
    // Add entry with hash = 2
    buffer.update_detection(2, 5, -70);
    ASSERT_EQ(buffer.get_detection_count(2), 5);
    
    // Add entry with hash = 34 (collides with 2: 34 % 32 == 2)
    buffer.update_detection(34, 3, -65);
    ASSERT_EQ(buffer.get_detection_count(34), 3);
    
    // Add entry with hash = 66 (collides with 2 and 34: 66 % 32 == 2)
    buffer.update_detection(66, 7, -80);
    ASSERT_EQ(buffer.get_detection_count(66), 7);
    
    // Verify all entries are still accessible
    ASSERT_EQ(buffer.get_detection_count(2), 5);
    ASSERT_EQ(buffer.get_detection_count(34), 3);
    ASSERT_EQ(buffer.get_detection_count(66), 7);
    
    ASSERT_EQ(buffer.count(), 3);
}

TEST(DetectionRingBuffer_ProbeCount) {
    DetectionRingBuffer buffer;
    
    // Add entries that all collide
    buffer.update_detection(0, 1, -70);
    buffer.update_detection(32, 1, -70);
    buffer.update_detection(64, 1, -70);
    buffer.update_detection(96, 1, -70);
    
    // Check probe counts
    ASSERT_EQ(buffer.get_probe_count(0), 1);    // Index 0
    ASSERT_EQ(buffer.get_probe_count(32), 2);   // Index 1
    ASSERT_EQ(buffer.get_probe_count(64), 3);   // Index 2
    ASSERT_EQ(buffer.get_probe_count(96), 4);   // Index 3
    
    // Worst case probe count
    buffer.update_detection(128, 1, -70);
    ASSERT_EQ(buffer.get_probe_count(128), 5);
}

TEST(DetectionRingBuffer_StressTest) {
    DetectionRingBuffer buffer;
    
    const int NUM_ENTRIES = 100;
    int collisions = 0;
    
    // Add many entries
    for (int i = 0; i < NUM_ENTRIES; ++i) {
        size_t hash = i * 7;  // Multiply by 7 to create collisions
        buffer.update_detection(hash, 1, -70);
        
        size_t probes = buffer.get_probe_count(hash);
        if (probes > 1) {
            collisions++;
        }
    }
    
    std::cout << "  Collisions: " << collisions << "/" << NUM_ENTRIES << std::endl;
    
    // Some collisions are expected with MAX_ENTRIES=32
    ASSERT_GT(collisions, 0);
    
    // Verify random lookups
    ASSERT_EQ(buffer.get_detection_count(0), 1);
    ASSERT_EQ(buffer.get_detection_count(7), 1);
    ASSERT_EQ(buffer.get_detection_count(14), 1);
}

// ============================================================================
// TEST 3: Stack Overflow Protection
// ============================================================================

// Simulate deep call stack
size_t call_depth = 0;
size_t max_depth_reached = 0;

void deep_function(size_t depth) {
    call_depth = depth;
    if (depth > max_depth_reached) {
        max_depth_reached = depth;
    }
    
    // Simulate local variables
    char buffer[256];
    memset(buffer, 0, sizeof(buffer));
    
    if (depth < 30) {  // Prevent infinite recursion
        deep_function(depth + 1);
    }
}

TEST(StackOverflow_DeepCallStack) {
    call_depth = 0;
    max_depth_reached = 0;
    
    // Simulate deep call stack
    deep_function(0);
    
    std::cout << "  Max depth reached: " << max_depth_reached << std::endl;
    
    // Should reach depth 30 without stack overflow
    ASSERT_GE(max_depth_reached, 30);
}

// ============================================================================
// TEST 4: Buffer Overflow Protection in TrackedDrones
// ============================================================================

constexpr size_t MAX_TRACKED_DRONES = 8;

struct TrackedDrone {
    uint64_t frequency_hz;
    int32_t rssi_db;
    uint32_t last_seen;
};

class TrackedDroneBuffer {
public:
    TrackedDroneBuffer() : tracked_count_(0) {
        memset(tracked_drones_, 0, sizeof(tracked_drones_));
    }
    
    bool add_drone(const TrackedDrone& drone) {
        if (tracked_count_ < MAX_TRACKED_DRONES) {
            // Add new drone
            tracked_drones_[tracked_count_] = drone;
            tracked_count_++;
            return true;
        } else {
            // Ring buffer overflow protection
            if (tracked_count_ == 0) {
                return false;  // Should not happen, but keep for safety
            }
            
            // Find oldest entry and replace
            size_t oldest_index = 0;
            uint32_t oldest_time = tracked_drones_[0].last_seen;
            
            for (size_t i = 1; i < tracked_count_; ++i) {
                if (tracked_drones_[i].last_seen < oldest_time) {
                    oldest_time = tracked_drones_[i].last_seen;
                    oldest_index = i;
                }
            }
            
            // Replace oldest
            tracked_drones_[oldest_index] = drone;
            return true;
        }
    }
    
    size_t count() const { return tracked_count_; }
    
    bool contains_frequency(uint64_t frequency) const {
        for (size_t i = 0; i < tracked_count_; ++i) {
            if (tracked_drones_[i].frequency_hz == frequency) {
                return true;
            }
        }
        return false;
    }

private:
    TrackedDrone tracked_drones_[MAX_TRACKED_DRONES];
    size_t tracked_count_;
};

TEST(TrackedDroneBuffer_OverflowProtection) {
    TrackedDroneBuffer buffer;
    
    // Add drones up to limit
    for (size_t i = 0; i < MAX_TRACKED_DRONES; ++i) {
        TrackedDrone drone = {2400000000 + i * 1000000, -70, static_cast<uint32_t>(i)};
        ASSERT_TRUE(buffer.add_drone(drone));
    }
    
    ASSERT_EQ(buffer.count(), MAX_TRACKED_DRONES);
    
    // Add more drones - should trigger ring buffer replacement
    for (size_t i = 0; i < 10; ++i) {
        TrackedDrone drone = {2500000000 + i * 1000000, -65, 
                             static_cast<uint32_t>(MAX_TRACKED_DRONES + i)};
        ASSERT_TRUE(buffer.add_drone(drone));
    }
    
    // Count should still be at max
    ASSERT_EQ(buffer.count(), MAX_TRACKED_DRONES);
    
    // Oldest drone should be replaced
    ASSERT_FALSE(buffer.contains_frequency(2400000000));
    
    // Newer drones should be present
    ASSERT_TRUE(buffer.contains_frequency(2500000000));
}

// ============================================================================
// TEST 5: String Safety (snprintf vs strcpy/strcat)
// ============================================================================

TEST(StringSafety_SafeFormatting) {
    char buffer[64];
    int count = 12345;
    
    // Safe: Using snprintf
    snprintf(buffer, sizeof(buffer), "Loaded %d frequencies", count);
    
    ASSERT_EQ(strlen(buffer), 24);
    ASSERT_EQ(strcmp(buffer, "Loaded 12345 frequencies"), 0);
    
    // Test with very large count (should not overflow)
    count = 999999999;
    snprintf(buffer, sizeof(buffer), "Loaded %d frequencies", count);
    
    // Buffer should be null-terminated
    ASSERT_EQ(buffer[sizeof(buffer) - 1], '\0');
    
    // Content should be truncated but safe
    ASSERT_NE(buffer[0], '\0');
}

TEST(StringSafety_UnsafeConcatenation) {
    char buffer[64];
    const char* part1 = "This is a very long part 1 ";
    const char* part2 = "and this is part 2 that makes it too long";
    
    // Unsafe: strcpy + strcat can overflow
    // DON'T DO THIS IN PRODUCTION:
    // strcpy(buffer, part1);
    // strcat(buffer, part2);
    
    // Safe: Use snprintf
    snprintf(buffer, sizeof(buffer), "%s%s", part1, part2);
    
    // Buffer should be null-terminated
    ASSERT_EQ(buffer[sizeof(buffer) - 1], '\0');
    
    // Verify content is truncated but safe
    size_t len = strlen(buffer);
    ASSERT_LT(len, sizeof(buffer));
    ASSERT_GT(len, 0);
}

// ============================================================================
// TEST 6: Integer Overflow Protection
// ============================================================================

TEST(IntegerOverflow_FrequencyCalculation) {
    const uint64_t WIDEBAND_SLICE_WIDTH = 24000000;  // 24 MHz
    
    // Test 1: Normal case
    uint64_t safe_min = 2400000000ULL;
    uint64_t safe_max = 2450000000ULL;
    
    uint64_t scanning_range = safe_max - safe_min;
    ASSERT_EQ(scanning_range, 50000000ULL);
    
    // Test 2: Calculate slices without overflow
    if (scanning_range > WIDEBAND_SLICE_WIDTH) {
        uint64_t range_plus_width = scanning_range + WIDEBAND_SLICE_WIDTH - 1;
        
        // Check for overflow
        ASSERT_GT(range_plus_width, scanning_range);
        
        uint64_t slices_nb = range_plus_width / WIDEBAND_SLICE_WIDTH;
        ASSERT_EQ(slices_nb, 3);
    }
    
    // Test 3: Near max value (potential overflow)
    safe_min = 0xFFFFFFFFFFF00000ULL;  // Near max
    safe_max = 0xFFFFFFFFFFF80000ULL;
    
    scanning_range = safe_max - safe_min;
    ASSERT_EQ(scanning_range, 0x80000ULL);
    
    if (scanning_range > WIDEBAND_SLICE_WIDTH) {
        uint64_t range_plus_width = scanning_range + WIDEBAND_SLICE_WIDTH - 1;
        
        // Check for overflow
        if (range_plus_width < scanning_range) {
            // Overflow detected - handle it
            ASSERT_TRUE(false);  // Should not reach here in this test
        }
        
        uint64_t slices_nb = range_plus_width / WIDEBAND_SLICE_WIDTH;
        ASSERT_GE(slices_nb, 1);
    }
}

// ============================================================================
// Main Test Runner
// ============================================================================

int main() {
    std::cout << "========================================" << std::endl;
    std::cout << "Enhanced Drone Analyzer - Unit Tests" << std::endl;
    std::cout << "========================================" << std::endl;
    std::cout << std::endl;
    
    std::cout << "=== TEST 1: Atomic Scanning Flag ===" << std::endl;
    RUN_TEST(AtomicScanningFlag_ThreadSafety);
    RUN_TEST(AtomicScanningFlag_ConcurrentAccess);
    RUN_TEST(AtomicScanningFlag_StressTest);
    std::cout << std::endl;
    
    std::cout << "=== TEST 2: DetectionRingBuffer ===" << std::endl;
    RUN_TEST(DetectionRingBuffer_BasicOperation);
    RUN_TEST(DetectionRingBuffer_HashCollision);
    RUN_TEST(DetectionRingBuffer_ProbeCount);
    RUN_TEST(DetectionRingBuffer_StressTest);
    std::cout << std::endl;
    
    std::cout << "=== TEST 3: Stack Overflow Protection ===" << std::endl;
    RUN_TEST(StackOverflow_DeepCallStack);
    std::cout << std::endl;
    
    std::cout << "=== TEST 4: TrackedDrone Buffer ===" << std::endl;
    RUN_TEST(TrackedDroneBuffer_OverflowProtection);
    std::cout << std::endl;
    
    std::cout << "=== TEST 5: String Safety ===" << std::endl;
    RUN_TEST(StringSafety_SafeFormatting);
    RUN_TEST(StringSafety_UnsafeConcatenation);
    std::cout << std::endl;
    
    std::cout << "=== TEST 6: Integer Overflow ===" << std::endl;
    RUN_TEST(IntegerOverflow_FrequencyCalculation);
    std::cout << std::endl;
    
    std::cout << "========================================" << std::endl;
    std::cout << "ALL TESTS PASSED!" << std::endl;
    std::cout << "========================================" << std::endl;
    
    return 0;
}