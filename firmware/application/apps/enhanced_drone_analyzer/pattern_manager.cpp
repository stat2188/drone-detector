#include "pattern_manager.hpp"
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cctype>
#include <string>
#include "file_path.hpp"
#include "string_format.hpp"
#include "file.hpp"

namespace drone_analyzer {

PatternManager::PatternManager() noexcept
    : patterns_{}
    , pattern_count_(0)
    , loaded_()
    , mutex_()
    , dir_open_(false) {
    chMtxInit(&mutex_);
}

PatternManager::~PatternManager() noexcept {
    // No cleanup needed - directory_iterator RAII handles resources
}

ErrorCode PatternManager::load_patterns() noexcept {
    MutexLock<LockOrder::PATTERN_MUTEX> lock(mutex_);

    pattern_count_ = 0;

    const auto pattern_dir_path = std::filesystem::path{PATTERN_DIR};

    for (const auto& entry : std::filesystem::directory_iterator(pattern_dir_path, (const TCHAR*)u"*.TXT")) {
        if (!std::filesystem::is_regular_file(entry.status())) {
            continue;
        }

        if (pattern_count_ >= MAX_PATTERNS) {
            break;
        }

        const auto& entry_path = entry.path();
        auto stem = entry_path.stem();

        char filename_stem[PATTERN_NAME_MAX_LEN];
        const auto& stem_native = stem.native();
        const size_t copy_len = (stem_native.length() >= PATTERN_NAME_MAX_LEN)
            ? (PATTERN_NAME_MAX_LEN - 1)
            : stem_native.length();

        for (size_t i = 0; i < copy_len; ++i) {
            filename_stem[i] = static_cast<char>(stem_native[i]);
        }
        filename_stem[copy_len] = '\0';

        const ErrorCode err = load_from_file(entry_path);
        if (err == ErrorCode::SUCCESS) {
            ++pattern_count_;
        }
    }

    if (pattern_count_ > 0) {
        loaded_.set();
    }

    return ErrorCode::SUCCESS;
}

ErrorCode PatternManager::load_from_file(const std::filesystem::path& file_path) noexcept {
    File file;
    const auto open_err = file.open(file_path, true, false);
    if (open_err.is_valid()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    constexpr size_t READ_CHUNK_SIZE = 128;
    uint8_t chunk[READ_CHUNK_SIZE];
    uint8_t line_buf[128];
    size_t line_len = 0;
    ErrorCode result = ErrorCode::SUCCESS;

    while (true) {
        const File::Result<File::Size> read_result = file.read(chunk, READ_CHUNK_SIZE);
        if (!read_result.is_ok()) {
            result = ErrorCode::DATABASE_CORRUPTED;
            break;
        }

        const File::Size bytes_read = read_result.value();
        if (bytes_read == 0) {
            break;
        }

        for (size_t i = 0; i < bytes_read; ++i) {
            const char c = static_cast<char>(chunk[i]);

            if (c == '\r' || c == '\n') {
                if (line_len > 0) {
                    const ErrorCode err = load_pattern_from_line(
                        reinterpret_cast<const char*>(line_buf),
                        line_len
                    );
                    if (err != ErrorCode::SUCCESS) {
                        result = err;
                        break;
                    }
                }
                line_len = 0;
            } else if (line_len < sizeof(line_buf) - 1) {
                line_buf[line_len++] = chunk[i];
            }
        }

        if (result != ErrorCode::SUCCESS) {
            break;
        }
    }

    file.close();
    return result;
}

ErrorCode PatternManager::load_pattern_from_line(
    const char* line,
    size_t line_length
) noexcept {
    if (pattern_count_ >= MAX_PATTERNS) {
        return ErrorCode::BUFFER_FULL;
    }

    SignalPattern& pattern = patterns_[pattern_count_];
    pattern = SignalPattern{};

    size_t field = 0;
    char temp[16];
    size_t temp_len = 0;

    for (size_t i = 0; i < line_length && field < 27; ++i) {
        const char c = line[i];

        if (c == ',' || c == '\0') {
            temp[temp_len] = '\0';

            switch (field) {
                case 0:
                    (void)safe_str_copy(pattern.name, PATTERN_NAME_MAX_LEN, temp);
                    break;
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 11:
                case 12:
                case 13:
                case 14:
                case 15:
                case 16: {
                    const size_t waveform_idx = field - 1;
                    if (waveform_idx < PATTERN_WAVEFORM_SIZE) {
                        pattern.waveform[waveform_idx] = static_cast<uint8_t>(atoi(temp));
                    }
                    break;
                }
                case 17:
                    pattern.features.peak_position = static_cast<uint8_t>(atoi(temp));
                    break;
                case 18:
                    pattern.features.peak_value = static_cast<uint8_t>(atoi(temp));
                    break;
                case 19:
                    pattern.features.noise_floor = static_cast<uint8_t>(atoi(temp));
                    break;
                case 20:
                    pattern.features.margin = static_cast<uint8_t>(atoi(temp));
                    break;
                case 21:
                    pattern.features.width = static_cast<uint8_t>(atoi(temp));
                    break;
                case 22:
                    pattern.features.sharpness = static_cast<uint8_t>(atoi(temp));
                    break;
                case 23:
                    pattern.features.flatness = static_cast<uint8_t>(atoi(temp));
                    break;
                case 24:
                    pattern.features.symmetry = static_cast<uint8_t>(atoi(temp));
                    break;
                case 25:
                    pattern.match_threshold = static_cast<uint8_t>(atoi(temp));
                    break;
                case 26:
                    pattern.flags = static_cast<uint8_t>(atoi(temp));
                    break;
            }

            ++field;
            temp_len = 0;
        } else if (temp_len < sizeof(temp) - 1) {
            temp[temp_len++] = c;
        }
    }

    if (pattern.is_valid()) {
        ++pattern_count_;
        return ErrorCode::SUCCESS;
    }

    return ErrorCode::DATABASE_CORRUPTED;
}

ErrorCode PatternManager::save_pattern(const SignalPattern& pattern) noexcept {
    MutexLock<LockOrder::PATTERN_MUTEX> lock(mutex_);

    if (pattern_count_ >= MAX_PATTERNS) {
        return ErrorCode::BUFFER_FULL;
    }

    patterns_[pattern_count_] = pattern;
    ++pattern_count_;

    return save_to_file(pattern);
}

ErrorCode PatternManager::save_to_file(const SignalPattern& pattern) noexcept {
    const auto pattern_dir_path = std::filesystem::path{PATTERN_DIR};
    
    std::u16string name_u16;
    for (size_t i = 0; i < PATTERN_NAME_MAX_LEN && pattern.name[i] != '\0'; ++i) {
        name_u16 += static_cast<char16_t>(pattern.name[i]);
    }
    const auto file_path = pattern_dir_path / (std::filesystem::path{name_u16} + u".TXT");
    
    File file;
    const auto open_err = file.create(file_path);
    if (open_err.is_valid()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    uint8_t write_buf[256];
    size_t write_pos = 0;

    auto write_char = [&](char c) noexcept -> void {
        if (write_pos < sizeof(write_buf)) {
            write_buf[write_pos++] = static_cast<uint8_t>(c);
        }
    };

    auto write_int = [&](int32_t val) noexcept -> void {
        char tmp[12];
        const int len = snprintf(tmp, sizeof(tmp), "%ld", static_cast<int32_t>(val));
        for (int i = 0; i < len; ++i) {
            write_char(tmp[i]);
        }
    };

    for (size_t i = 0; i < PATTERN_NAME_MAX_LEN && pattern.name[i] != '\0'; ++i) {
        write_char(pattern.name[i]);
    }

    for (size_t i = 0; i < PATTERN_WAVEFORM_SIZE; ++i) {
        write_char(',');
        write_int(static_cast<int32_t>(pattern.waveform[i]));
    }

    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.peak_position));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.peak_value));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.noise_floor));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.margin));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.width));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.sharpness));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.flatness));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.features.symmetry));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.match_threshold));
    write_char(',');
    write_int(static_cast<int32_t>(pattern.flags));
    write_char('\n');

    const File::Result<File::Size> write_result = file.write(write_buf, static_cast<File::Size>(write_pos));
    file.close();
    
    if (!write_result.is_ok()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }
    
    return ErrorCode::SUCCESS;
}

ErrorCode PatternManager::delete_pattern(size_t index) noexcept {
    MutexLock<LockOrder::PATTERN_MUTEX> lock(mutex_);
    
    if (index >= pattern_count_) {
        return ErrorCode::INVALID_PARAMETER;
    }
    
    const auto pattern_dir_path = std::filesystem::path{PATTERN_DIR};
    
    std::u16string name_u16;
    for (size_t i = 0; i < PATTERN_NAME_MAX_LEN && patterns_[index].name[i] != '\0'; ++i) {
        name_u16 += static_cast<char16_t>(patterns_[index].name[i]);
    }
    const auto file_path = pattern_dir_path / (std::filesystem::path{name_u16} + u".TXT");
    const auto del_err = delete_file(file_path);
    if (!del_err.ok()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    for (size_t i = index; i < pattern_count_ - 1; ++i) {
        patterns_[i] = patterns_[i + 1];
    }

    // Clear last slot to prevent stale data
    if (pattern_count_ > 0) {
        patterns_[pattern_count_ - 1] = SignalPattern{};
    }

    --pattern_count_;

    return ErrorCode::SUCCESS;
}

const SignalPattern* PatternManager::get_pattern(size_t index) const noexcept {
    MutexLock<LockOrder::PATTERN_MUTEX> lock(mutex_);

    if (index >= pattern_count_) {
        return nullptr;
    }

    return &patterns_[index];
}

const SignalPattern* PatternManager::get_patterns_array() const noexcept {
    return patterns_.data();
}

size_t PatternManager::get_pattern_count() const noexcept {
    return pattern_count_;
}

size_t PatternManager::find_pattern_by_name(const char* name) const noexcept {
    MutexLock<LockOrder::PATTERN_MUTEX> lock(mutex_);

    for (size_t i = 0; i < pattern_count_; ++i) {
        if (strcmp(patterns_[i].name, name) == 0) {
            return i;
        }
    }

    return pattern_count_;
}

void PatternManager::clear_all_patterns() noexcept {
    MutexLock<LockOrder::PATTERN_MUTEX> lock(mutex_);

    pattern_count_ = 0;
    for (auto& pattern : patterns_) {
        pattern = SignalPattern{};
    }
}

bool PatternManager::is_loaded() const noexcept {
    return loaded_.test();
}

bool PatternManager::safe_str_copy(
    char* dest,
    size_t dest_size,
    const char* src
) const noexcept {
    if (dest == nullptr || src == nullptr || dest_size == 0) {
        return false;
    }

    size_t i = 0;
    while (i < dest_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }

    dest[i] = '\0';
    return true;
}

} // namespace drone_analyzer
