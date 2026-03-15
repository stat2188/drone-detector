#include "database.hpp"

#include <cstring>
#include <cstdlib>

// ChibiOS headers (will be available when integrated)
// #include "ch.h"

namespace drone_analyzer {

// ============================================================================
// FrequencyEntry Implementation
// ============================================================================

FrequencyEntry::FrequencyEntry() noexcept
    : frequency(0)
    , drone_type(DroneType::UNKNOWN)
    , priority(0)
    , reserved(0)
    , flags(0) {
}

FrequencyEntry::FrequencyEntry(FreqHz freq, DroneType type, uint8_t prio) noexcept
    : frequency(freq)
    , drone_type(type)
    , priority(prio)
    , reserved(0)
    , flags(0) {
}

bool FrequencyEntry::is_valid() const noexcept {
    return (frequency >= MIN_FREQUENCY_HZ) &&
           (frequency <= MAX_FREQUENCY_HZ) &&
           (drone_type != DroneType::UNKNOWN);
}

// ============================================================================
// DatabaseManager Implementation
// ============================================================================

DatabaseManager::DatabaseManager() noexcept
    : entries_()
    , current_index_(0)
    , entry_count_(0)
    , loaded_()
    , mutex_(nullptr)
    , line_buffer_{} {
    
    // Initialize mutex (will be done with ChibiOS)
    // mutex_ = new mutex_t;
    // chMtxObjectInit(mutex_);
}

DatabaseManager::~DatabaseManager() noexcept {
    // Note: Do NOT call chMtxDeinit - it doesn't exist in ChibiOS
    // Mutex cleanup is handled by ChibiOS kernel
}

ErrorCode DatabaseManager::load_frequency_database() noexcept {
    // Check if already loaded (lock-free check)
    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }
    
    // Acquire mutex for thread safety
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    // Double-check after acquiring lock
    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }
    
    // Load database from file
    ErrorCode result = load_from_file_internal();
    
    if (result == ErrorCode::SUCCESS) {
        loaded_.set();
    } else if (result == ErrorCode::DATABASE_LOAD_TIMEOUT ||
               result == ErrorCode::DATABASE_CORRUPTED) {
        // Use built-in defaults as fallback
        add_default_frequencies_internal();
        loaded_.set();
        result = ErrorCode::SUCCESS;
    }
    
    return result;
}

ErrorCode DatabaseManager::load_from_file_internal() noexcept {
    // Placeholder for file loading
    // In actual implementation, this would:
    // 1. Open DATABASE_FILE_PATH
    // 2. Read line by line
    // 3. Parse each line into FrequencyEntry
    // 4. Validate and add to entries_
    //
    // IMPORTANT: When implementing actual file I/O, protect all file operations
    // with LockOrder::SD_CARD_MUTEX to ensure thread safety:
    //
    // MutexLock<LockOrder::SD_CARD_MUTEX> lock(*sd_card_mutex_);
    // // Open file, read lines, parse entries
    // // Mutex is automatically released when lock goes out of scope
    
    // For now, add built-in defaults
    add_default_frequencies_internal();
    
    return ErrorCode::SUCCESS;
}

ErrorCode DatabaseManager::parse_line_internal(
    const char* line,
    FrequencyEntry& entry
) const noexcept {
    if (line == nullptr || line[0] == '\0') {
        return ErrorCode::BUFFER_INVALID;
    }
    
    // Parse Freqman format: "f=2405000000,d=DJI,m=0,b=0,s=0,t=0"
    // We'll parse this directly without using parse_freqman_entry()
    
    // Initialize entry
    entry.frequency = 0;
    entry.drone_type = DroneType::UNKNOWN;
    entry.priority = 0;
    entry.flags = 0;
    
    // Parse key-value pairs separated by commas
    const char* ptr = line;
    char description[128] = {0};
    size_t desc_len = 0;
    
    while (*ptr != '\0') {
        // Skip whitespace
        while (*ptr == ' ' || *ptr == '\t') {
            ptr++;
        }
        
        if (*ptr == '\0') {
            break;
        }
        
        // Find key (before '=')
        const char* key_start = ptr;
        while (*ptr != '=' && *ptr != ',' && *ptr != '\0') {
            ptr++;
        }
        
        if (*ptr != '=') {
            // Invalid format, skip to next comma or end
            while (*ptr != ',' && *ptr != '\0') {
                ptr++;
            }
            if (*ptr == ',') {
                ptr++;
            }
            continue;
        }
        
        size_t key_len = ptr - key_start;
        ptr++;  // Skip '='
        
        // Find value (before ',' or '\0')
        const char* value_start = ptr;
        while (*ptr != ',' && *ptr != '\0') {
            ptr++;
        }
        
        size_t value_len = ptr - value_start;
        
        // Skip to next comma or end
        if (*ptr == ',') {
            ptr++;
        }
        
        // Parse key-value pairs
        // Check for 'f' (frequency)
        if (key_len == 1 && key_start[0] == 'f') {
            // Parse frequency value
            FreqHz freq = 0;
            for (size_t i = 0; i < value_len && i < 11; ++i) {
                char c = value_start[i];
                if (c >= '0' && c <= '9') {
                    freq = freq * 10 + (c - '0');
                }
            }
            entry.frequency = freq;
        }
        // Check for 'd' (description)
        else if (key_len == 1 && key_start[0] == 'd') {
            // Copy description
            size_t copy_len = value_len;
            if (copy_len > 127) {
                copy_len = 127;
            }
            for (size_t i = 0; i < copy_len; ++i) {
                description[i] = value_start[i];
            }
            desc_len = copy_len;
        }
        // Check for 'a' (frequency_a)
        else if (key_len == 1 && key_start[0] == 'a') {
            // Parse frequency_a value
            FreqHz freq = 0;
            for (size_t i = 0; i < value_len && i < 11; ++i) {
                char c = value_start[i];
                if (c >= '0' && c <= '9') {
                    freq = freq * 10 + (c - '0');
                }
            }
            if (entry.frequency == 0) {
                entry.frequency = freq;
            }
        }
        // Check for 'b' (frequency_b)
        else if (key_len == 1 && key_start[0] == 'b') {
            // Frequency_b is not used for drone detection
        }
        // Check for 'c' (channel)
        else if (key_len == 1 && key_start[0] == 'c') {
            // Channel is not used for drone detection
        }
        // Check for 'l' (location)
        else if (key_len == 1 && key_start[0] == 'l') {
            // Location is not used for drone detection
        }
        // Check for 'm' (modulation)
        else if (key_len == 1 && key_start[0] == 'm') {
            // Modulation is not used for drone detection
        }
        // Check for 'r' (repeater)
        else if (key_len == 1 && key_start[0] == 'r') {
            // Repeater is not used for drone detection
        }
        // Check for 's' (step)
        else if (key_len == 1 && key_start[0] == 's') {
            // Step is not used for drone detection
        }
        // Check for 't' (tone)
        else if (key_len == 1 && key_start[0] == 't') {
            // Tone is not used for drone detection
        }
        // Check for 'bw' (bandwidth)
        else if (key_len == 2 && key_start[0] == 'b' && key_start[1] == 'w') {
            // Bandwidth is not used for drone detection
        }
    }
    
    // Map description to DroneType
    // Check for DJI in description
    bool found_dji = false;
    bool found_parrot = false;
    
    if (desc_len > 0) {
        // Check for DJI
        for (size_t i = 0; i < desc_len - 2; ++i) {
            if (description[i] == 'd' && 
                description[i+1] == 'J' && 
                description[i+2] == 'I') {
                found_dji = true;
                break;
            }
        }
        
        // Check for Parrot
        if (!found_dji && desc_len > 5) {
            for (size_t i = 0; i < desc_len - 5; ++i) {
                if (description[i] == 'P' && 
                    description[i+1] == 'a' && 
                    description[i+2] == 'r' && 
                    description[i+3] == 'r' && 
                    description[i+4] == 'o' && 
                    description[i+5] == 't') {
                    found_parrot = true;
                    break;
                }
            }
        }
    }
    
    if (found_dji) {
        entry.drone_type = DroneType::DJI;
    } else if (found_parrot) {
        entry.drone_type = DroneType::PARROT;
    } else {
        entry.drone_type = DroneType::UNKNOWN;
    }
    
    // Extract priority from description
    // Look for priority keywords like "prio:1" or "priority:high"
    entry.priority = 0;
    
    if (desc_len > 0) {
        bool found_prio_1 = false;
        bool found_prio_2 = false;
        bool found_prio_3 = false;
        bool found_prio_high = false;
        bool found_prio_critical = false;
        
        // Check for "prio:" followed by a number
        for (size_t i = 0; i < desc_len - 5; ++i) {
            if (description[i] == 'p' && 
                description[i+1] == 'r' && 
                description[i+2] == 'i' && 
                description[i+3] == 'o' && 
                description[i+4] == ':') {
                if (i + 5 < desc_len) {
                    char c = description[i+5];
                    if (c == '1') {
                        found_prio_1 = true;
                    } else if (c == '2') {
                        found_prio_2 = true;
                    } else if (c == '3') {
                        found_prio_3 = true;
                    }
                }
                break;
            }
        }
        
        // Check for "priority:" followed by a number
        for (size_t i = 0; i < desc_len - 9; ++i) {
            if (description[i] == 'p' && 
                description[i+1] == 'r' && 
                description[i+2] == 'i' && 
                description[i+3] == 'o' && 
                description[i+4] == 'r' && 
                description[i+5] == 'i' && 
                description[i+6] == 't' && 
                description[i+7] == 'y' && 
                description[i+8] == ':') {
                if (i + 9 < desc_len) {
                    char c = description[i+9];
                    if (c == '1') {
                        found_prio_1 = true;
                    } else if (c == '2') {
                        found_prio_2 = true;
                    } else if (c == '3') {
                        found_prio_3 = true;
                    }
                }
                break;
            }
        }
        
        // Check for "prio:high"
        for (size_t i = 0; i < desc_len - 8; ++i) {
            if (description[i] == 'p' && 
                description[i+1] == 'r' && 
                description[i+2] == 'i' && 
                description[i+3] == 'o' && 
                description[i+4] == ':' && 
                description[i+5] == 'h' && 
                description[i+6] == 'i' && 
                description[i+7] == 'g' && 
                description[i+8] == 'h') {
                found_prio_high = true;
                break;
            }
        }
        
        // Check for "priority:high"
        for (size_t i = 0; i < desc_len - 12; ++i) {
            if (description[i] == 'p' && 
                description[i+1] == 'r' && 
                description[i+2] == 'i' && 
                description[i+3] == 'o' && 
                description[i+4] == 'r' && 
                description[i+5] == 'i' && 
                description[i+6] == 't' && 
                description[i+7] == 'y' && 
                description[i+8] == ':' && 
                description[i+9] == 'h' && 
                description[i+10] == 'i' && 
                description[i+11] == 'g' && 
                description[i+12] == 'h') {
                found_prio_high = true;
                break;
            }
        }
        
        // Check for "prio:critical"
        for (size_t i = 0; i < desc_len - 11; ++i) {
            if (description[i] == 'p' && 
                description[i+1] == 'r' && 
                description[i+2] == 'i' && 
                description[i+3] == 'o' && 
                description[i+4] == ':' && 
                description[i+5] == 'c' && 
                description[i+6] == 'r' && 
                description[i+7] == 'i' && 
                description[i+8] == 't' && 
                description[i+9] == 'i' && 
                description[i+10] == 'c' && 
                description[i+11] == 'a' && 
                description[i+12] == 'l') {
                found_prio_critical = true;
                break;
            }
        }
        
        // Check for "priority:critical"
        for (size_t i = 0; i < desc_len - 15; ++i) {
            if (description[i] == 'p' && 
                description[i+1] == 'r' && 
                description[i+2] == 'i' && 
                description[i+3] == 'o' && 
                description[i+4] == 'r' && 
                description[i+5] == 'i' && 
                description[i+6] == 't' && 
                description[i+7] == 'y' && 
                description[i+8] == ':' && 
                description[i+9] == 'c' && 
                description[i+10] == 'r' && 
                description[i+11] == 'i' && 
                description[i+12] == 't' && 
                description[i+13] == 'i' && 
                description[i+14] == 'c' && 
                description[i+15] == 'a' && 
                description[i+16] == 'l') {
                found_prio_critical = true;
                break;
            }
        }
        
        // Map priority keywords to priority value
        if (found_prio_1 || found_prio_2 || found_prio_3) {
            entry.priority = 1;
        } else if (found_prio_high) {
            entry.priority = 3;
        } else if (found_prio_critical) {
            entry.priority = 4;
        }
    }
    
    // Validate entry
    return validate_entry_internal(entry);
}

ErrorCode DatabaseManager::validate_entry_internal(
    const FrequencyEntry& entry
) const noexcept {
    if (entry.frequency < MIN_FREQUENCY_HZ ||
        entry.frequency > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (entry.drone_type == DroneType::UNKNOWN) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

void DatabaseManager::add_default_frequencies_internal() noexcept {
    // Add common drone frequencies (2.4 GHz ISM band)
    // These are typical frequencies used by DJI, Parrot, etc.
    
    constexpr FreqHz DEFAULT_FREQUENCIES[] = {
        2'400'000'000ULL,  // 2.400 GHz
        2'405'000'000ULL,  // 2.405 GHz
        2'410'000'000ULL,  // 2.410 GHz
        2'415'000'000ULL,  // 2.415 GHz
        2'420'000'000ULL,  // 2.420 GHz
        2'425'000'000ULL,  // 2.425 GHz
        2'430'000'000ULL,  // 2.430 GHz
        2'435'000'000ULL,  // 2.435 GHz
        2'440'000'000ULL,  // 2.440 GHz
        2'445'000'000ULL,  // 2.445 GHz
        2'450'000'000ULL,  // 2.450 GHz
        2'455'000'000ULL,  // 2.455 GHz
        2'460'000'000ULL,  // 2.460 GHz
        2'465'000'000ULL,  // 2.465 GHz
        2'470'000'000ULL,  // 2.470 GHz
        2'475'000'000ULL,  // 2.475 GHz
        2'480'000'000ULL,  // 2.480 GHz
    };
    
    constexpr size_t DEFAULT_COUNT = sizeof(DEFAULT_FREQUENCIES) / sizeof(DEFAULT_FREQUENCIES[0]);
    
    for (size_t i = 0; i < DEFAULT_COUNT && entry_count_ < MAX_DATABASE_ENTRIES; ++i) {
        entries_[entry_count_] = FrequencyEntry(
            DEFAULT_FREQUENCIES[i],
            DroneType::UNKNOWN,
            0
        );
        entry_count_++;
    }
}

ErrorResult<FreqHz> DatabaseManager::get_next_frequency(FreqHz current_freq) noexcept {
    // Ensure database is loaded
    ErrorCode load_result = load_frequency_database();
    if (load_result != ErrorCode::SUCCESS) {
        return ErrorResult<FreqHz>::failure(load_result);
    }
    
    // Acquire mutex for thread safety
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    // If current_freq is 0, return first entry
    if (current_freq == 0) {
        if (entry_count_ == 0) {
            return ErrorResult<FreqHz>::failure(ErrorCode::DATABASE_EMPTY);
        }
        current_index_ = 0;
        return ErrorResult<FreqHz>::success(entries_[current_index_].frequency);
    }
    
    // Find current frequency in database
    ErrorResult<size_t> index_result = find_entry_index_internal(current_freq);
    if (!index_result.has_value()) {
        // Not found, start from beginning
        current_index_ = 0;
    } else {
        current_index_ = index_result.value();
    }
    
    // Move to next entry
    current_index_++;
    if (current_index_ >= entry_count_) {
        current_index_ = 0;  // Wrap around
    }
    
    return ErrorResult<FreqHz>::success(entries_[current_index_].frequency);
}

void DatabaseManager::reset_database() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    current_index_ = 0;
}

size_t DatabaseManager::get_database_size() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    return entry_count_;
}

bool DatabaseManager::is_loaded() const noexcept {
    return loaded_.test();
}

ErrorResult<FrequencyEntry> DatabaseManager::get_entry(size_t index) const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    if (index >= entry_count_) {
        return ErrorResult<FrequencyEntry>::failure(ErrorCode::INVALID_PARAMETER);
    }
    
    return ErrorResult<FrequencyEntry>::success(entries_[index]);
}

ErrorResult<FrequencyEntry> DatabaseManager::find_entry(FreqHz frequency) const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    ErrorResult<size_t> index_result = find_entry_index_internal(frequency);
    if (!index_result.has_value()) {
        return ErrorResult<FrequencyEntry>::failure(index_result.error());
    }
    
    return ErrorResult<FrequencyEntry>::success(entries_[index_result.value()]);
}

ErrorResult<size_t> DatabaseManager::find_entry_index_internal(
    FreqHz frequency
) const noexcept {
    for (size_t i = 0; i < entry_count_; ++i) {
        if (entries_[i].frequency == frequency) {
            return ErrorResult<size_t>::success(i);
        }
    }
    
    return ErrorResult<size_t>::failure(ErrorCode::INVALID_PARAMETER);
}

ErrorCode DatabaseManager::add_entry(const FrequencyEntry& entry) noexcept {
    // Validate entry first
    ErrorCode validate_result = validate_entry_internal(entry);
    if (validate_result != ErrorCode::SUCCESS) {
        return validate_result;
    }
    
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    // Check if database is full
    if (entry_count_ >= MAX_DATABASE_ENTRIES) {
        return ErrorCode::BUFFER_FULL;
    }
    
    // Check if entry already exists
    ErrorResult<size_t> index_result = find_entry_index_internal(entry.frequency);
    if (index_result.has_value()) {
        // Update existing entry
        entries_[index_result.value()] = entry;
        return ErrorCode::SUCCESS;
    }
    
    // Add new entry
    entries_[entry_count_] = entry;
    entry_count_++;
    
    return ErrorCode::SUCCESS;
}

ErrorCode DatabaseManager::remove_entry(FreqHz frequency) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    // Find entry index
    ErrorResult<size_t> index_result = find_entry_index_internal(frequency);
    if (!index_result.has_value()) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    // Remove entry by shifting remaining entries
    size_t index = index_result.value();
    for (size_t i = index; i < entry_count_ - 1; ++i) {
        entries_[i] = entries_[i + 1];
    }
    entry_count_--;
    
    // Adjust current index if needed
    if (current_index_ >= entry_count_ && entry_count_ > 0) {
        current_index_ = entry_count_ - 1;
    }
    
    return ErrorCode::SUCCESS;
}

void DatabaseManager::clear_entries() noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    entry_count_ = 0;
    current_index_ = 0;
}

ErrorResult<size_t> DatabaseManager::get_current_index() const noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    return ErrorResult<size_t>::success(current_index_);
}

ErrorCode DatabaseManager::set_current_index(size_t index) noexcept {
    MutexLock<LockOrder::DATA_MUTEX> lock(*mutex_);
    
    if (index >= entry_count_) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    current_index_ = index;
    return ErrorCode::SUCCESS;
}

} // namespace drone_analyzer
