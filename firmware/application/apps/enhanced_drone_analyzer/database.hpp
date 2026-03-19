#ifndef DATABASE_HPP
#define DATABASE_HPP

#include <cstdint>
#include <cstddef>
#include <array>
#include "ch.h"
#include "drone_types.hpp"
#include "freqman_db.hpp"
#include "locking.hpp"
#include "constants.hpp"

namespace drone_analyzer {

struct FrequencyEntry {
    FreqHz frequency;
    DroneType drone_type;
    uint8_t priority;
    uint16_t reserved;
    uint32_t flags;

    FrequencyEntry() noexcept
        : frequency(0)
        , drone_type(DroneType::UNKNOWN)
        , priority(0)
        , reserved(0)
        , flags(0) {}

    FrequencyEntry(FreqHz freq, DroneType type, uint8_t prio) noexcept
        : frequency(freq)
        , drone_type(type)
        , priority(prio)
        , reserved(0)
        , flags(0) {}

    [[nodiscard]] bool is_valid() const noexcept {
        return (frequency >= MIN_FREQUENCY_HZ) &&
               (frequency <= MAX_FREQUENCY_HZ) &&
               (drone_type != DroneType::UNKNOWN);
    }
};

class DatabaseManager {
public:
    DatabaseManager() noexcept;
    ~DatabaseManager() noexcept;

    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;
    DatabaseManager(DatabaseManager&&) = delete;
    DatabaseManager& operator=(DatabaseManager&&) = delete;

    [[nodiscard]] ErrorCode load_frequency_database() noexcept;
    [[nodiscard]] ErrorResult<FreqHz> get_next_frequency(FreqHz current_freq) noexcept;
    void reset_database() noexcept;
    [[nodiscard]] size_t get_database_size() const noexcept;
    [[nodiscard]] bool is_loaded() const noexcept;
    [[nodiscard]] ErrorResult<FrequencyEntry> get_entry(size_t index) const noexcept;
    [[nodiscard]] ErrorResult<FrequencyEntry> find_entry(FreqHz frequency) const noexcept;
    [[nodiscard]] ErrorCode add_entry(const FrequencyEntry& entry) noexcept;
    [[nodiscard]] ErrorCode remove_entry(FreqHz frequency) noexcept;
    void clear_entries() noexcept;
    [[nodiscard]] ErrorResult<size_t> get_current_index() const noexcept;
    [[nodiscard]] ErrorCode set_current_index(size_t index) noexcept;
    
private:
    
    [[nodiscard]] ErrorResult<FrequencyEntry> find_entry_internal(FreqHz frequency) const noexcept;
    [[nodiscard]] ErrorResult<size_t> find_entry_index_internal(FreqHz frequency) const noexcept;
    [[nodiscard]] ErrorCode load_from_file_internal() noexcept;

    std::array<FrequencyEntry, MAX_DATABASE_ENTRIES> entries_;
    
    size_t current_index_{0};
    size_t entry_count_{0};
    AtomicFlag loaded_;
    mutable Mutex mutex_;
};

} // namespace drone_analyzer

#endif // DATABASE_HPP
