#include "database.hpp"
#include "freqman_db.hpp"
#include <cctype>
#include <cstring>
#include <cstdlib>
#include <ctype.h>

namespace drone_analyzer {

static DroneType parse_drone_type_from_description(
    const char* buffer,
    size_t value_start,
    size_t value_len
) noexcept {
    if (value_len == 0 || buffer == nullptr) {
        return DroneType::UNKNOWN;
    }

    size_t first_word_len = 0;
    for (size_t i = 0; i < value_len && i < 16; i++) {
        char c = buffer[value_start + i];
        if (c == ' ' || c == '\t' || c == ',' || c == '\r' || c == '\n') {
            break;
        }
        first_word_len++;
    }

    if (first_word_len == 0) {
        return DroneType::UNKNOWN;
    }

    auto compare_word = [&](const char* word, size_t word_len) -> bool {
        if (first_word_len != word_len) {
            return false;
        }
        for (size_t i = 0; i < word_len; i++) {
            char c1 = buffer[value_start + i];
            char c2 = word[i];
            if (::tolower(static_cast<unsigned char>(c1)) !=
                ::tolower(static_cast<unsigned char>(c2))) {
                return false;
            }
        }
        return true;
    };

    if (compare_word("FPV", 3)) {
        return DroneType::FPV;
    }
    if (compare_word("DJI", 3)) {
        return DroneType::DJI;
    }
    if (compare_word("PARROT", 6)) {
        return DroneType::PARROT;
    }
    if (compare_word("YUNEEC", 6)) {
        return DroneType::YUNEEC;
    }
    if (compare_word("HOBBY", 5)) {
        return DroneType::HOBBY;
    }
    if (compare_word("AUTEL", 5)) {
        return DroneType::AUTEL;
    }
    if (compare_word("3DR", 3)) {
        return DroneType::DR_3DR;
    }
    if (compare_word("OTHER", 5)) {
        return DroneType::OTHER;
    }
    if (compare_word("CUSTOM", 6)) {
        return DroneType::CUSTOM;
    }

    return DroneType::UNKNOWN;
}

ErrorCode DatabaseManager::load_from_file_internal() noexcept {
    entry_count_ = 0;
    
    freqman_load_options options{
        .max_entries = static_cast<size_t>(MAX_DATABASE_ENTRIES),
        .load_freqs = true,
        .load_ranges = false,
        .load_hamradios = false,
        .load_repeaters = false
    };
    
    freqman_db temp_db;
    
    if (!load_freqman_file("DRONES", temp_db, options)) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }
    
    if (temp_db.empty()) {
        return ErrorCode::DATABASE_EMPTY;
    }
    
    for (const auto& entry_ptr : temp_db) {
        const auto& entry = *entry_ptr;
        
        if (entry.type != ::freqman_type::Single) {
            continue;
        }
        
        if (entry.frequency_a < 0) {
            continue;
        }
        
        const FreqHz freq = static_cast<FreqHz>(entry.frequency_a);
        if (freq < MIN_FREQUENCY_HZ || freq > MAX_FREQUENCY_HZ) {
            continue;
        }
        
        DroneType type = parse_drone_type_from_description(
            entry.description.c_str(),
            0,
            entry.description.length()
        );
        
        if (type == DroneType::UNKNOWN) {
            continue;
        }
        
        if (entry_count_ >= MAX_DATABASE_ENTRIES) {
            break;
        }
        
        entries_[entry_count_] = FrequencyEntry(freq, type, 0);
        entry_count_++;
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode DatabaseManager::load_frequency_database() noexcept {
    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }

    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }

    ErrorCode result = load_from_file_internal();

    if (result == ErrorCode::SUCCESS) {
        if (entry_count_ > 0) {
            loaded_.set();
        } else {
            result = ErrorCode::DATABASE_EMPTY;
        }
    }

    return result;
}

ErrorResult<FreqHz> DatabaseManager::get_next_frequency(FreqHz current_freq) noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    if (entry_count_ == 0) {
        return ErrorResult<FreqHz>::failure(ErrorCode::DATABASE_EMPTY);
    }
    
    if (current_freq == 0) {
        current_index_ = 0;
        return ErrorResult<FreqHz>::success(entries_[current_index_].frequency);
    }
    
    ErrorResult<size_t> index_result = find_entry_index_internal(current_freq);
    if (!index_result.has_value()) {
        current_index_ = 0;
    } else {
        current_index_ = index_result.value();
    }
    
    current_index_++;
    if (current_index_ >= entry_count_) {
        current_index_ = 0;
    }
    
    return ErrorResult<FreqHz>::success(entries_[current_index_].frequency);
}

ErrorResult<FrequencyEntry> DatabaseManager::find_entry(FreqHz frequency) const noexcept {
    ErrorResult<size_t> index_result = find_entry_index_internal(frequency);
    if (!index_result.has_value()) {
        return ErrorResult<FrequencyEntry>::failure(ErrorCode::INVALID_PARAMETER);
    }
    return ErrorResult<FrequencyEntry>::success(entries_[index_result.value()]);
}

ErrorResult<size_t> DatabaseManager::find_entry_index_internal(FreqHz frequency) const noexcept {
    for (size_t i = 0; i < entry_count_; ++i) {
        if (entries_[i].frequency == frequency) {
            return ErrorResult<size_t>::success(i);
        }
    }
    
    return ErrorResult<size_t>::failure(ErrorCode::INVALID_PARAMETER);
}

DatabaseManager::DatabaseManager() noexcept
    : entries_()
    , current_index_(0)
    , entry_count_(0)
    , loaded_()
    , mutex_() {
    chMtxInit(&mutex_);
}

DatabaseManager::~DatabaseManager() noexcept {
}

} 
