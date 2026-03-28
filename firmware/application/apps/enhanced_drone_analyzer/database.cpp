#include <cstdint>
#include <cstring>
#include <cctype>
#include <array>
#include <string_view>

#include "database.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "freqman_db.hpp"
#include "freqman.hpp"

namespace drone_analyzer {

static DroneType parse_drone_type_from_description(
    const char* buffer,
    size_t value_len
) noexcept {
    if (value_len == 0 || buffer == nullptr) {
        return DroneType::UNKNOWN;
    }

    size_t first_word_len = 0;
    for (size_t i = 0; i < value_len && i < 16; i++) {
        const char c = buffer[i];
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
            const char c1 = buffer[i];
            const char c2 = word[i];
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

/**
 * @brief Streaming parser: read database directly without heap allocation
 * @note Reads file line-by-line using File class, no std::vector or heap allocation
 * @note Reuses parse_freqman_entry() from freqman_db.hpp
 */
ErrorCode DatabaseManager::load_from_file_internal() noexcept {
    entry_count_ = 0;
    current_index_ = 0;

    File file;
    auto open_result = file.open(get_freqman_path(database_file_));
    if (!open_result) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }

    constexpr size_t LINE_BUFFER_SIZE = 128;
    std::array<char, LINE_BUFFER_SIZE> line_buffer{};
    size_t line_pos = 0;
    auto file_size = file.size();

    if (file_size == 0) {
        return ErrorCode::DATABASE_EMPTY;
    }

    std::array<uint8_t, 256> read_buffer{};
    size_t buf_pos = 0;
    size_t buf_len = 0;

    while (entry_count_ < MAX_DATABASE_ENTRIES) {
        if (buf_pos >= buf_len) {
            auto read_result = file.read(read_buffer.data(), 256);
            if (!read_result.is_ok() || read_result.value() == 0) {
                break;
            }
            buf_len = read_result.value();
            buf_pos = 0;
            if (buf_len == 0) {
                break;
            }
        }

        uint8_t c = read_buffer[buf_pos++];

        if (c == '\n' || c == '\r' || line_pos >= (LINE_BUFFER_SIZE - 1)) {
            if (line_pos > 0) {
                line_buffer[line_pos] = '\0';

                freqman_entry entry;
                if (parse_freqman_entry(
                        std::string_view(line_buffer.data(), line_pos), entry)) {
                    if (entry.type == freqman_type::Single &&
                        entry.frequency_a >= MIN_FREQUENCY_HZ &&
                        entry.frequency_a <= MAX_FREQUENCY_HZ) {

                        DroneType type = DroneType::OTHER;
                        if (entry.description.size() > 0) {
                            const DroneType parsed = parse_drone_type_from_description(
                                entry.description.c_str(),
                                entry.description.size());
                            if (parsed != DroneType::UNKNOWN) {
                                type = parsed;
                            }
                        }

                        entries_[entry_count_++] = FrequencyEntry(
                            static_cast<FreqHz>(entry.frequency_a), type);
                    }
                }
            }
            line_pos = 0;
            if (c == '\r' && buf_pos < buf_len && read_buffer[buf_pos] == '\n') {
                buf_pos++;
            }
            continue;
        }

        if (c >= 32) {
            line_buffer[line_pos++] = static_cast<char>(c);
        }
    }

    file.close();

    if (entry_count_ == 0) {
        return ErrorCode::DATABASE_EMPTY;
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

    const ErrorCode result = load_from_file_internal();

    if (result == ErrorCode::SUCCESS) {
        if (entry_count_ > 0) {
            loaded_.set();
        } else {
            return ErrorCode::DATABASE_EMPTY;
        }
    }

    return result;
}

ErrorResult<FreqHz> DatabaseManager::get_next_frequency(FreqHz current_freq) noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    
    if (entry_count_ == 0) {
        return ErrorResult<FreqHz>::failure(ErrorCode::DATABASE_EMPTY);
    }
    
    // If current_freq is 0, return first frequency (initial case)
    if (current_freq == 0) {
        current_index_ = 0;
        return ErrorResult<FreqHz>::success(entries_[current_index_].frequency);
    }
    
    // Find current frequency in database
    bool found = false;
    size_t found_index = 0;
    for (size_t i = 0; i < entry_count_; ++i) {
        if (entries_[i].frequency == current_freq) {
            found_index = i;
            found = true;
            break;
        }
    }
    
    if (found) {
        // Advance to next frequency (wrap around if at end)
        current_index_ = found_index + 1;
        if (current_index_ >= entry_count_) {
            current_index_ = 0;
        }
    } else {
        // Current frequency not in database — return first entry
        // This ensures we always move forward, never stay stuck
        current_index_ = 0;
    }
    
    return ErrorResult<FreqHz>::success(entries_[current_index_].frequency);
}

ErrorResult<FrequencyEntry> DatabaseManager::find_entry(FreqHz frequency) const noexcept {
    if (!loaded_.test()) {
        return ErrorResult<FrequencyEntry>::failure(ErrorCode::DATABASE_NOT_LOADED);
    }

    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    for (size_t i = 0; i < entry_count_; ++i) {
        if (entries_[i].frequency == frequency) {
            return ErrorResult<FrequencyEntry>::success(entries_[i]);
        }
    }
    
    return ErrorResult<FrequencyEntry>::failure(ErrorCode::INVALID_PARAMETER);
}

size_t DatabaseManager::get_database_size() const noexcept {
    return entry_count_;
}

bool DatabaseManager::is_loaded() const noexcept {
    return loaded_.test();
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

void DatabaseManager::set_database_file(const char* filename) noexcept {
    if (filename == nullptr) {
        return;
    }
    
    // Copy filename (max 31 chars + null terminator)
    size_t i = 0;
    while (i < 31 && filename[i] != '\0') {
        database_file_[i] = filename[i];
        ++i;
    }
    database_file_[i] = '\0';
    
    // Reset state so database will be fully reloaded
    loaded_.clear();
    current_index_ = 0;
    entry_count_ = 0;
}

} 
