#include "database.hpp"
#include "file.hpp"

using namespace std::literals;

namespace drone_analyzer {

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

ErrorCode DatabaseManager::load_from_file_internal() noexcept {
    entry_count_ = 0;
    
    File file;
    const auto error = file.open(u"/FREQMAN/DRONES.TXT");
    
    if (error) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }
    
    constexpr size_t buffer_size = 256;
    char buffer[buffer_size];
    
    while (entry_count_ < MAX_DATABASE_ENTRIES) {
        auto read_result = file.read(buffer, buffer_size);
        
        if (!read_result.is_ok()) {
            break;
        }
        
        const size_t bytes_read = read_result.value();
        
        if (bytes_read == 0) {
            break;
        }
        
        for (size_t line_start = 0; line_start < bytes_read; ) {
            while (line_start < bytes_read && 
                   (buffer[line_start] == ' ' || buffer[line_start] == '\t' || 
                    buffer[line_start] == '\r')) {
                line_start++;
            }
            
            if (line_start >= bytes_read) {
                break;
            }
            
            if (buffer[line_start] == '#') {
                while (line_start < bytes_read && buffer[line_start] != '\n') {
                    line_start++;
                }
                continue;
            }
            
            size_t line_end = line_start;
            while (line_end < bytes_read && 
                   buffer[line_end] != '\n' && buffer[line_end] != '\r') {
                line_end++;
            }
            
            if (line_end >= bytes_read) {
                break;
            }
            
            FreqHz freq = 0;
            DroneType type = DroneType::DJI;
            
            size_t pos = line_start;
            
            while (pos < line_end) {
                while (pos < line_end && 
                       (buffer[pos] == ' ' || buffer[pos] == '\t')) {
                    pos++;
                }
                
                if (pos >= line_end) {
                    break;
                }
                
                size_t key_start = pos;
                
                while (pos < line_end && buffer[pos] != '=') {
                    pos++;
                }
                
                if (pos >= line_end) {
                    break;
                }
                
                char key_char = buffer[key_start];
                
                pos++;
                
                size_t value_start = pos;
                
                while (pos < line_end && 
                       buffer[pos] != ',' && 
                       buffer[pos] != '\n' && 
                       buffer[pos] != '\r') {
                    pos++;
                }
                
                size_t value_len = pos - value_start;
                
                if (key_char == 'f' && value_len > 0 && value_len < 16) {
                    uint64_t freq_u64 = 0;

                    for (size_t i = 0; i < value_len; i++) {
                        char c = buffer[value_start + i];
                        if (c >= '0' && c <= '9') {
                            freq_u64 = freq_u64 * 10 + (c - '0');
                        }
                    }

                    freq = freq_u64;
                } else if (key_char == 'd') {
                    if (value_len == 3) {
                        char c0 = buffer[value_start];
                        char c1 = buffer[value_start + 1];
                        char c2 = buffer[value_start + 2];

                        if ((c0 == 'F' || c0 == 'f') &&
                            (c1 == 'P' || c1 == 'p') &&
                            (c2 == 'V' || c2 == 'v')) {
                            type = DroneType::FPV;
                        } else if ((c0 == 'D' || c0 == 'd') &&
                            (c1 == 'J' || c1 == 'j') &&
                            (c2 == 'I' || c2 == 'i')) {
                            type = DroneType::DJI;
                        }
                    } else if (value_len == 6) {
                        char c0 = buffer[value_start];
                        char c1 = buffer[value_start + 1];
                        char c2 = buffer[value_start + 2];
                        char c3 = buffer[value_start + 3];
                        char c4 = buffer[value_start + 4];
                        char c5 = buffer[value_start + 5];

                        if ((c0 == 'P' || c0 == 'p') &&
                            (c1 == 'A' || c1 == 'a') &&
                            (c2 == 'R' || c2 == 'r') &&
                            (c3 == 'R' || c3 == 'r') &&
                            (c4 == 'O' || c4 == 'o') &&
                            (c5 == 'T' || c5 == 't')) {
                            type = DroneType::PARROT;
                        } else if ((c0 == 'Y' || c0 == 'y') &&
                            (c1 == 'U' || c1 == 'u') &&
                            (c2 == 'N' || c2 == 'n') &&
                            (c3 == 'E' || c3 == 'e') &&
                            (c4 == 'E' || c4 == 'e') &&
                            (c5 == 'C' || c5 == 'c')) {
                            type = DroneType::YUNEEC;
                        }
                    }
                }
                
                pos++;
            }
            
            if (freq >= MIN_FREQUENCY_HZ && freq <= MAX_FREQUENCY_HZ) {
                entries_[entry_count_] = FrequencyEntry(freq, type, 0);
                entry_count_++;
            }
            
            line_start = line_end + 1;
        }
    }
    
    if (entry_count_ == 0) {
        constexpr FreqHz DEFAULT_FREQUENCIES[] = {
            2406500000ULL, 
            5658000000ULL
        };
        
        for (size_t i = 0; i < 2 && entry_count_ < MAX_DATABASE_ENTRIES; ++i) {
            entries_[entry_count_] = FrequencyEntry(
                DEFAULT_FREQUENCIES[i],
                (i == 0) ? DroneType::DJI : DroneType::FPV,
                0
            );
            entry_count_++;
        }
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode DatabaseManager::validate_entry_internal(const FrequencyEntry& entry) const noexcept {
    if (entry.frequency < MIN_FREQUENCY_HZ ||
        entry.frequency > MAX_FREQUENCY_HZ) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    if (entry.drone_type == DroneType::UNKNOWN) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    return ErrorCode::SUCCESS;
}

ErrorResult<FreqHz> DatabaseManager::get_next_frequency(FreqHz current_freq) noexcept {
    ErrorCode load_result = load_frequency_database();
    if (load_result != ErrorCode::SUCCESS) {
        return ErrorResult<FreqHz>::failure(load_result);
    }
    
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    if (current_freq == 0) {
        if (entry_count_ == 0) {
            return ErrorResult<FreqHz>::failure(ErrorCode::DATABASE_EMPTY);
        }
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

void DatabaseManager::reset_database() noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    current_index_ = 0;
}

size_t DatabaseManager::get_database_size() const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    return entry_count_;
}

bool DatabaseManager::is_loaded() const noexcept {
    return loaded_.test();
}

ErrorResult<FrequencyEntry> DatabaseManager::get_entry(size_t index) const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    if (index >= entry_count_) {
        return ErrorResult<FrequencyEntry>::failure(ErrorCode::INVALID_PARAMETER);
    }
    
    return ErrorResult<FrequencyEntry>::success(entries_[index]);
}

ErrorResult<FrequencyEntry> DatabaseManager::find_entry(FreqHz frequency) const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    ErrorResult<size_t> index_result = find_entry_index_internal(frequency);
    if (!index_result.has_value()) {
        return ErrorResult<FrequencyEntry>::failure(index_result.error());
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

ErrorCode DatabaseManager::add_entry(const FrequencyEntry& entry) noexcept {
    ErrorCode validate_result = validate_entry_internal(entry);
    if (validate_result != ErrorCode::SUCCESS) {
        return validate_result;
    }
    
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    if (entry_count_ >= MAX_DATABASE_ENTRIES) {
        return ErrorCode::BUFFER_FULL;
    }
    
    ErrorResult<size_t> index_result = find_entry_index_internal(entry.frequency);
    if (index_result.has_value()) {
        entries_[index_result.value()] = entry;
        return ErrorCode::SUCCESS;
    }
    
    entries_[entry_count_] = entry;
    entry_count_++;
    
    return ErrorCode::SUCCESS;
}

ErrorCode DatabaseManager::remove_entry(FreqHz frequency) noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    ErrorResult<size_t> index_result = find_entry_index_internal(frequency);
    if (!index_result.has_value()) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    size_t index = index_result.value();
    
    for (size_t i = index; i < entry_count_ - 1; ++i) {
        entries_[i] = entries_[i + 1];
    }
    entry_count_--;
    
    if (index < current_index_) {
        current_index_--;
    } else if (current_index_ >= entry_count_ && entry_count_ > 0) {
        current_index_ = entry_count_ - 1;
    }
    
    return ErrorCode::SUCCESS;
}

void DatabaseManager::clear_entries() noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    entry_count_ = 0;
    current_index_ = 0;
}

ErrorResult<size_t> DatabaseManager::get_current_index() const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    return ErrorResult<size_t>::success(current_index_);
}

ErrorCode DatabaseManager::set_current_index(size_t index) noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    if (index >= entry_count_) {
        return ErrorCode::INVALID_PARAMETER;
    }

    current_index_ = index;
    return ErrorCode::SUCCESS;
}

} 
