/*
 * Enhanced Drone Analyzer - Memory Pool Implementation
 * Phase 3.1: Fixed-size memory pools for frequent allocations
 *
 * Scott Meyers Effective C++ Item 29: Object pools for frequent allocations
 * Scott Meyers Effective C++ Item 11: Prevent self-assignment
 * Scott Meyers Effective C++ Item 4: Lazy initialization of singletons
 */

#ifndef UI_ENHANCED_DRONE_MEMORY_POOL_HPP_
#define UI_ENHANCED_DRONE_MEMORY_POOL_HPP_

#include <ch.h>
#include <cstdint>
#include <array>
#include <algorithm>

// Forward declaration (defined in ui_enhanced_drone_analyzer.hpp)
namespace ui::apps::enhanced_drone_analyzer {
    struct DisplayDroneEntry;
}

#include "ui_drone_common_types.hpp"

namespace ui::apps::enhanced_drone_analyzer {

// ============================================
// ФАЗА 3.2: Универсальный Memory Pool (Fixed Size)
// ============================================

template<typename T, size_t PoolSize>
class FixedSizeMemoryPool {
public:
    static constexpr size_t ELEMENT_SIZE = sizeof(T);
    static constexpr size_t POOL_SIZE_BYTES = PoolSize * ELEMENT_SIZE;

    // Constructor
    FixedSizeMemoryPool()
        : allocation_count_(0),
          free_count_(PoolSize) {
        chMtxInit(&mutex_);

        // Инициализация free list
        for (size_t i = 0; i < PoolSize; ++i) {
            free_list_[i] = static_cast<T*>(pool_ + i * ELEMENT_SIZE);
        }
    }

    ~FixedSizeMemoryPool() {
        chMtxLock(&mutex_);
        // Pool автоматически уничтожается при выходе из области видимости
        chMtxUnlock(&mutex_);
    }

    // 🔴 ФАЗА 3.3: Thread-safe allocation
    T* allocate() {
        chMtxLock(&mutex_);

        if (free_count_ == 0) {
            // Pool exhausted - return nullptr
            chMtxUnlock(&mutex_);
            return nullptr;
        }

        T* ptr = free_list_[--free_count_];
        ++allocation_count_;

        chMtxUnlock(&mutex_);

        // Placement new для вызова конструктора
        new(ptr) T();

        return ptr;
    }

    // 🔴 ФАЗА 3.4: Thread-safe deallocation
    void deallocate(T* ptr) {
        if (!ptr) return;

        chMtxLock(&mutex_);

        // Вызов деструктора
        ptr->~T();

        // Возврат в free list
        if (free_count_ < PoolSize) {
            free_list_[free_count_++] = ptr;
        }

        --allocation_count_;

        chMtxUnlock(&mutex_);
    }

    // 🔴 ФАЗА 3.5: Status functions
    size_t allocated_count() const {
        return allocation_count_;
    }

    size_t free_count() const {
        return free_count_;
    }

    size_t total_count() const {
        return PoolSize;
    }

    float utilization() const {
        return static_cast<float>(allocation_count_) / PoolSize;
    }

    bool is_exhausted() const {
        return free_count_ == 0;
    }

    // Prevent copying
    FixedSizeMemoryPool(const FixedSizeMemoryPool&) = delete;
    FixedSizeMemoryPool& operator=(const FixedSizeMemoryPool&) = delete;

private:
    alignas(T) char pool_[POOL_SIZE_BYTES];
    T* free_list_[PoolSize];
    size_t allocation_count_;
    size_t free_count_;
    mutable Mutex mutex_;
};

// ============================================
// ФАЗА 3.6: RAII Wrapper для pool-allocated объектов
// ============================================

template<typename T, size_t PoolSize>
class PoolPtr {
public:
    using PoolType = FixedSizeMemoryPool<T, PoolSize>;

    explicit PoolPtr(PoolType& pool)
        : ptr_(pool.allocate()),
          pool_(&pool) {
    }

    ~PoolPtr() {
        if (ptr_ && pool_) {
            pool_->deallocate(ptr_);
        }
    }

    // Scott Meyers Item 11: Prevent self-assignment
    PoolPtr(const PoolPtr&) = delete;
    PoolPtr& operator=(const PoolPtr&) = delete;

    // Move semantics (Scott Meyers Item 11)
    PoolPtr(PoolPtr&& other) noexcept
        : ptr_(other.ptr_),
          pool_(other.pool_) {
        other.ptr_ = nullptr;
        other.pool_ = nullptr;
    }

    PoolPtr& operator=(PoolPtr&& other) noexcept {
        if (this != &other) {
            if (ptr_ && pool_) {
                pool_->deallocate(ptr_);
            }
            ptr_ = other.ptr_;
            pool_ = other.pool_;
            other.ptr_ = nullptr;
            other.pool_ = nullptr;
        }
        return *this;
    }

    T* get() const { return ptr_; }
    T* operator->() const { return ptr_; }
    T& operator*() const { return *ptr_; }
    explicit operator bool() const { return ptr_ != nullptr; }

private:
    T* ptr_;
    PoolType* pool_;
};

// ============================================
// ФАЗА 3.7: Singleton pools для EDA
// ============================================

class EDAMemoryPools {
public:
    // Singleton access (Scott Meyers Item 4)
    static EDAMemoryPools& instance() {
        static EDAMemoryPools pools;
        return pools;
    }

    // Pool для DetectionLogEntry (10 entries = ~2KB)
    static constexpr size_t DETECTION_LOG_POOL_SIZE = 10;
    using DetectionLogPool = FixedSizeMemoryPool<DetectionLogEntry, DETECTION_LOG_POOL_SIZE>;

    // Pool для DisplayDroneEntry (8 entries = ~1.6KB)
    static constexpr size_t DISPLAY_DRONE_POOL_SIZE = 8;
    using DisplayDronePool = FixedSizeMemoryPool<DisplayDroneEntry, DISPLAY_DRONE_POOL_SIZE>;

    DetectionLogPool& detection_log_pool() { return detection_log_pool_; }
    DisplayDronePool& display_drone_pool() { return display_drone_pool_; }

    // Prevent copying
    EDAMemoryPools(const EDAMemoryPools&) = delete;
    EDAMemoryPools& operator=(const EDAMemoryPools&) = delete;

private:
    EDAMemoryPools() = default;
    ~EDAMemoryPools() = default;

    DetectionLogPool detection_log_pool_;
    DisplayDronePool display_drone_pool_;
};

// ============================================
// ФАЗА 3.8: Convenience functions
// ============================================

inline EDAMemoryPools& get_eda_pools() {
    return EDAMemoryPools::instance();
}

}  // namespace ui::apps::enhanced_drone_analyzer

#endif  // UI_ENHANCED_DRONE_MEMORY_POOL_HPP_
