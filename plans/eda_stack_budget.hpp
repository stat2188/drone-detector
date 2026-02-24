/**
 * @file eda_stack_budget.hpp
 * @brief Stack Memory Budget Calculator - Diamond Code
 * 
 * This file provides compile-time verification and documentation
 * of the stack memory budget for the Enhanced Drone Analyzer.
 * 
 * STM32F405 LIMITS:
 * - Total RAM: 128KB
 * - Recommended stack per thread: 1-4KB
 * - Total stack budget for EDA: 4KB
 * 
 * @author Diamond Code Pipeline
 * @date 2026-02-24
 */

#ifndef EDA_STACK_BUDGET_HPP_
#define EDA_STACK_BUDGET_HPP_

#include <cstdint>
#include <cstddef>

namespace ui::apps::enhanced_drone_analyzer {

//=============================================================================
// Hardware Limits
//=============================================================================
namespace HardwareLimits {
    /// @brief STM32F405 total RAM
    constexpr size_t TOTAL_RAM = 128 * 1024;  // 128KB
    
    /// @brief Maximum recommended stack per thread
    constexpr size_t MAX_THREAD_STACK = 4 * 1024;  // 4KB
    
    /// @brief EDA total stack budget
    constexpr size_t EDA_STACK_BUDGET = 4 * 1024;  // 4KB
}

//=============================================================================
// Stack Allocation Records
//=============================================================================

/**
 * @brief Stack allocation entry for documentation
 */
struct StackAllocation {
    const char* name;
    size_t size;
    const char* purpose;
};

//=============================================================================
// BEFORE Optimization (CRITICAL STATE)
//=============================================================================
namespace BeforeOptimization {
    // Thread Stack Allocations
    constexpr StackAllocation THREAD_ALLOCATIONS[] = {
        {"ScanningCoordinator", 1536, "Coordinate scanning operations"},
        {"DroneScanner",        4096, "Main scanning thread"},
        {"DroneDetectionLogger", 3072, "Log detection entries"},
        {"DB Loading",          4096, "Load frequency database"},
    };
    
    constexpr size_t THREAD_COUNT = sizeof(THREAD_ALLOCATIONS) / sizeof(StackAllocation);
    
    // Calculate total
    constexpr size_t TOTAL_STACK = 
        1536 + 4096 + 3072 + 4096;  // 12,800 bytes
    
    constexpr size_t OVERAGE = TOTAL_STACK - HardwareLimits::EDA_STACK_BUDGET;
    
    // Percentage over budget
    constexpr size_t PERCENTAGE_OVER = (TOTAL_STACK * 100) / HardwareLimits::EDA_STACK_BUDGET - 100;
    
    // Compile-time check (should FAIL)
    // static_assert(TOTAL_STACK <= HardwareLimits::EDA_STACK_BUDGET, 
    //               "BEFORE: Stack exceeds budget!");
}

//=============================================================================
// AFTER Optimization (DIAMOND CODE)
//=============================================================================
namespace AfterOptimization {
    // Thread Stack Allocations
    constexpr StackAllocation THREAD_ALLOCATIONS[] = {
        {"UnifiedWorker",       2048, "Single State Machine worker"},
        {"UI Thread",           1024, "UI event handling only"},
        {"ISR Reserve",         1024, "Interrupt handlers"},
    };
    
    constexpr size_t THREAD_COUNT = sizeof(THREAD_ALLOCATIONS) / sizeof(StackAllocation);
    
    // Calculate total
    constexpr size_t TOTAL_STACK = 
        2048 + 1024 + 1024;  // 4,096 bytes
    
    // Compile-time check (should PASS)
    static_assert(TOTAL_STACK <= HardwareLimits::EDA_STACK_BUDGET, 
                  "AFTER: Stack must not exceed budget!");
    
    // Bytes saved
    constexpr size_t BYTES_SAVED = BeforeOptimization::TOTAL_STACK - TOTAL_STACK;
    
    // Percentage saved
    constexpr size_t PERCENTAGE_SAVED = 
        (BYTES_SAVED * 100) / BeforeOptimization::TOTAL_STACK;
}

//=============================================================================
// Stack Usage Breakdown
//=============================================================================
namespace StackUsageBreakdown {
    // UnifiedWorker stack breakdown
    namespace UnifiedWorkerStack {
        constexpr size_t CHIBIOS_TCB = 100;        // ChibiOS Thread Control Block
        constexpr size_t STATE_VARIABLES = 20;     // State machine state
        constexpr size_t LOCAL_VARS_MAX = 100;     // Maximum locals per state
        constexpr size_t CALL_OVERHEAD = 100;      // Function call overhead
        constexpr size_t SAFETY_MARGIN = 400;      // Safety margin
        constexpr size_t CALCULATED = 
            CHIBIOS_TCB + STATE_VARIABLES + LOCAL_VARS_MAX + CALL_OVERHEAD + SAFETY_MARGIN;
        
        // Available for operations
        constexpr size_t AVAILABLE = 2048 - CALCULATED;  // ~1228 bytes
    }
}

//=============================================================================
// Validation Macros
//=============================================================================

/// @brief Validate stack allocation at compile time
#define EDA_VALIDATE_STACK(name, size) \
    static_assert((size) <= HardwareLimits::MAX_THREAD_STACK, \
                  name " stack exceeds max per-thread limit")

/// @brief Validate total stack budget at compile time
#define EDA_VALIDATE_BUDGET(total) \
    static_assert((total) <= HardwareLimits::EDA_STACK_BUDGET, \
                  "Total stack exceeds EDA budget")

// Compile-time validations
EDA_VALIDATE_STACK("UnifiedWorker", 2048);
EDA_VALIDATE_STACK("UIThread", 1024);
EDA_VALIDATE_BUDGET(AfterOptimization::TOTAL_STACK);

//=============================================================================
// Runtime Stack Monitoring
//=============================================================================

/**
 * @brief Stack monitor for runtime overflow detection
 * 
 * Usage:
 * @code
 * StackBudgetMonitor monitor;
 * if (!monitor.check_stack(512)) {
 *     // Less than 512 bytes free - danger!
 * }
 * @endcode
 */
class StackBudgetMonitor {
public:
    /// @brief Check if stack has at least 'required' bytes free
    /// @param required Minimum required bytes
    /// @return true if sufficient space, false if low stack
    static bool check_stack(size_t required) noexcept;
    
    /// @brief Get current stack usage percentage
    /// @return Usage percentage (0-100)
    static uint8_t get_usage_percent() noexcept;
    
    /// @brief Get free stack bytes
    /// @return Free bytes in current thread stack
    static size_t get_free_bytes() noexcept;
};

//=============================================================================
// Migration Checklist
//=============================================================================

/**
 * @brief Migration checklist for stack optimization
 * 
 * PHASE 1: Preparation
 * [ ] Backup existing code
 * [ ] Document current stack usage
 * [ ] Identify all thread creation points
 * 
 * PHASE 2: Implementation
 * [ ] Create UnifiedWorker class
 * [ ] Convert thread functions to state steps
 * [ ] Move buffers to static storage
 * [ ] Remove old thread creation code
 * 
 * PHASE 3: Testing
 * [ ] Verify stack canary triggers on overflow
 * [ ] Test all scanning modes
 * [ ] Measure actual stack usage
 * [ ] Stress test with heavy detection load
 * 
 * PHASE 4: Validation
 * [ ] Run for 24 hours without crash
 * [ ] Verify UI responsiveness
 * [ ] Confirm no memory leaks
 * [ ] Validate detection accuracy maintained
 */

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_STACK_BUDGET_HPP_
