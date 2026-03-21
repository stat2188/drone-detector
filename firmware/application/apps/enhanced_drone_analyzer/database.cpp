#include "database.hpp"
#include "file.hpp"
#include "file_path.hpp"
#include "sd_card.hpp"
#include <cstring>
#include <ctype.h>

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
 * @brief Parse a single FREQMAN format line and add to entries
 * @param line_buf Line buffer
 * @param line_len Line length
 * @return true if an entry was added
 */
static bool parse_freqman_line(
    uint8_t* line_buf,
    size_t line_len,
    FrequencyEntry* entries,
    size_t& entry_count,
    size_t max_entries
) noexcept {
    if (entry_count >= max_entries || line_len == 0) {
        return false;
    }

    // Skip comment lines
    if (line_buf[0] == '#') {
        return false;
    }

    size_t pos = 0;

    // Skip leading whitespace
    while (pos < line_len && (line_buf[pos] == ' ' || line_buf[pos] == '\t')) {
        pos++;
    }

    // Parse key=value pairs
    FreqHz freq = 0;
    bool has_freq = false;
    const char* desc_start = nullptr;
    size_t desc_len = 0;

    while (pos < line_len) {
        // Get key
        if (pos >= line_len) break;
        char key = static_cast<char>(::tolower(line_buf[pos]));
        pos++;

        // Skip '='
        if (pos < line_len && line_buf[pos] == '=') {
            pos++;
        }

        // Parse value based on key
        if (key == 'f' || key == 'a' || key == 'r') {
            // Frequency value
            int64_t freq_val = 0;
            while (pos < line_len && line_buf[pos] >= '0' && line_buf[pos] <= '9') {
                freq_val = freq_val * 10 + (line_buf[pos] - '0');
                pos++;
            }
            if (!has_freq) {
                freq = static_cast<FreqHz>(freq_val);
                has_freq = true;
            }
        } else if (key == 'd') {
            // Description value
            desc_start = reinterpret_cast<const char*>(&line_buf[pos]);
            while (pos < line_len && line_buf[pos] != ',') {
                desc_len++;
                pos++;
            }
        } else {
            // Skip unknown key's value
            while (pos < line_len && line_buf[pos] != ',') {
                pos++;
            }
        }

        // Skip comma separator
        if (pos < line_len && line_buf[pos] == ',') {
            pos++;
        }
    }

    if (!has_freq) {
        return false;
    }

    if (freq < MIN_FREQUENCY_HZ || freq > MAX_FREQUENCY_HZ) {
        return false;
    }

    if (desc_start == nullptr || desc_len == 0) {
        return false;
    }

    const DroneType type = parse_drone_type_from_description(desc_start, desc_len);
    if (type == DroneType::UNKNOWN) {
        return false;
    }

    entries[entry_count++] = FrequencyEntry(freq, type);
    return true;
}

/**
 * @brief Custom FREQMAN parser
 * @note Cannot use freqman_db.hpp (load_freqman_file) because it relies on
 *       std::string and heap-allocated freqman_entry objects. This implementation
 *       uses fixed-size line buffers for embedded safety.
 */
ErrorCode DatabaseManager::load_from_file_internal() noexcept {
    entry_count_ = 0;
    current_index_ = 0;

    // Check SD card is mounted before file access
    if (sd_card::status() != sd_card::Status::Mounted) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }

    // Build path: FREQMAN / <filename> .TXT
    // Matches pattern used by freqman_db.cpp: get_freqman_path()
    // Convert char database_file_ to char16_t for path constructor
    char16_t wide_name[32];
    size_t wi = 0;
    for (; wi < 31 && database_file_[wi] != '\0'; ++wi) {
        wide_name[wi] = static_cast<char16_t>(database_file_[wi]);
    }
    wide_name[wi] = u'\0';

    const auto filepath = freqman_dir /
        std::filesystem::path(wide_name) +
        std::filesystem::path(u".TXT");

    File file;
    const auto open_result = file.open(filepath, true, false);
    if (!open_result) {
        return ErrorCode::DATABASE_NOT_LOADED;
    }

    constexpr size_t READ_CHUNK_SIZE = 256;
    uint8_t chunk[READ_CHUNK_SIZE];
    uint8_t line_buf[128];
    size_t line_len = 0;

    while (entry_count_ < MAX_DATABASE_ENTRIES) {
        const auto read_result = file.read(chunk, READ_CHUNK_SIZE);
        if (!read_result.is_ok() || read_result.value() == 0) {
            break;
        }

        const size_t bytes_read = read_result.value();

        for (size_t i = 0; i < bytes_read && entry_count_ < MAX_DATABASE_ENTRIES; ++i) {
            const char c = static_cast<char>(chunk[i]);

            if (c == '\r' || c == '\n') {
                parse_freqman_line(line_buf, line_len, entries_.data(), entry_count_, MAX_DATABASE_ENTRIES);
                line_len = 0;
            } else if (line_len < sizeof(line_buf) - 1) {
                line_buf[line_len++] = static_cast<uint8_t>(c);
            }
        }
    }

    file.close();

    // Process final line if file doesn't end with newline
    parse_freqman_line(line_buf, line_len, entries_.data(), entry_count_, MAX_DATABASE_ENTRIES);

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
    
    // Reset loaded flag so database will be reloaded
    loaded_.clear();
}

} 
