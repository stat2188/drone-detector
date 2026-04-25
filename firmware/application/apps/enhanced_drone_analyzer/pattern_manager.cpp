#include "pattern_manager.hpp"
#include <cstring>

namespace drone_analyzer {

namespace {
static constexpr size_t FATFS_MAX_FILENAME = 128;

static size_t tchar_to_char(const TCHAR* src, char* dest, size_t dest_size) noexcept {
    if (src == nullptr || dest == nullptr || dest_size == 0) {
        return 0;
    }

    size_t len = 0;
    while (src[len] != 0 && len < dest_size - 1) {
        dest[len] = static_cast<char>(src[len] & 0xFF);
        ++len;
    }
    dest[len] = '\0';
    return len;
}
}

uint8_t PatternManager::parse_uint8(const char* str, size_t len) noexcept {
    if (str == nullptr || len == 0) {
        return 0;
    }

    uint8_t result = 0;
    for (size_t i = 0; i < len && str[i] >= '0' && str[i] <= '9'; ++i) {
        result = result * 10 + (str[i] - '0');
    }
    return result;
}

bool PatternManager::str_equals_ignore_case(
    const char* a,
    const char* b
) noexcept {
    if (a == nullptr || b == nullptr) return false;

    for (size_t i = 0; a[i] != '\0' && b[i] != '\0'; ++i) {
        const char ca = (a[i] >= 'A' && a[i] <= 'Z') ? a[i] + 32 : a[i];
        const char cb = (b[i] >= 'A' && b[i] <= 'Z') ? b[i] + 32 : b[i];
        if (ca != cb) return false;
    }
    return a[0] == '\0' && b[0] == '\0';
}

PatternManager::PatternManager() noexcept
    : patterns_{}
    , pattern_count_(0)
    , loaded_()
    , mutex_()
    , dir_open_(false) {
    chMtxInit(&mutex_);
}

PatternManager::~PatternManager() noexcept {
}

ErrorCode PatternManager::load_patterns() noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    if (loaded_.test()) {
        return ErrorCode::SUCCESS;
    }

    pattern_count_ = 0;

    DIR dir;
    FILINFO fno;
    const FRESULT res = f_opendir(&dir, reinterpret_cast<const TCHAR*>(PATTERN_DIR));

    if (res != FR_OK) {
        loaded_.set();
        return ErrorCode::SUCCESS;
    }

    struct DirGuard {
        DIR* dir;
        ~DirGuard() { if (dir) f_closedir(dir); }
    } dir_guard = {&dir};

    while (true) {
        const FRESULT readdir_res = f_readdir(&dir, &fno);
        if (readdir_res != FR_OK || fno.fname[0] == (TCHAR)'\0') {
            break;
        }

        if (fno.fattrib & AM_DIR) {
            continue;
        }

        char fname_buf[FATFS_MAX_FILENAME];
        const size_t fname_len = tchar_to_char(fno.fname, fname_buf, sizeof(fname_buf));
        if (fname_len < 4 || str_equals_ignore_case(&fname_buf[fname_len - 4], ".TXT") == false) {
            continue;
        }

        if (pattern_count_ >= MAX_PATTERNS) {
            break;
        }

        // CRITICAL FIX: Calculate maximum path size to prevent buffer overflow
        // PATTERN_DIR = "/PATTERNS/" (11 bytes) + "/" + filename (FATFS_MAX_FILENAME = 128) + ".TXT" (4) + null = 145 bytes
        constexpr size_t MAX_FULL_PATH_LEN = sizeof(PATTERN_DIR) + FATFS_MAX_FILENAME + 8;
        char full_path[MAX_FULL_PATH_LEN];
        const int written = snprintf(full_path, sizeof(full_path), "%s/%s", PATTERN_DIR, fname_buf);
        
        // Validate path was not truncated
        if (written < 0 || static_cast<size_t>(written) >= sizeof(full_path)) {
            continue;  // Skip files with too-long paths
        }

        const ErrorCode err = load_pattern_from_line(full_path, strlen(full_path));
        // NOTE: parse_pattern_csv() inside load_pattern_from_line() already increments
        // pattern_count_ on success. Do NOT increment here — that would double-count.
        (void)err;
    }

    if (pattern_count_ > 0) {
        loaded_.set();
    }

    return ErrorCode::SUCCESS;
}

ErrorCode PatternManager::load_pattern_from_line(
    const char* filepath,
    size_t filepath_length
) noexcept {
    if (filepath == nullptr || filepath_length == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    if (pattern_count_ >= MAX_PATTERNS) {
        return ErrorCode::BUFFER_FULL;
    }

    File file;
    const auto open_err = file.open(reinterpret_cast<const TCHAR*>(filepath));
    if (open_err.is_valid()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    // CRITICAL FIX: RAII file guard ensures file is always closed
    struct FileGuard {
        File* const file;
        explicit FileGuard(File* f) noexcept : file(f) {}
        ~FileGuard() { if (file) file->close(); }
        
        FileGuard(const FileGuard&) = delete;
        FileGuard& operator=(const FileGuard&) = delete;
    } file_guard(&file);

    constexpr size_t READ_BUF_SIZE = 256;
    uint8_t read_buf[READ_BUF_SIZE];
    char line_buf[READ_BUF_SIZE];
    size_t line_pos = 0;
    bool eof = false;

    while (!eof) {
        const auto read_result = file.read(read_buf, READ_BUF_SIZE);
        if (!read_result.is_ok()) {
            return ErrorCode::DATABASE_LOAD_TIMEOUT;
        }

        const size_t bytes_read = read_result.value();
        if (bytes_read == 0) {
            eof = true;
        }

        for (size_t i = 0; i < bytes_read; ++i) {
            const char c = static_cast<char>(read_buf[i]);

            if (c == '\n' || c == '\r' || line_pos >= READ_BUF_SIZE - 1) {
                if (line_pos > 0) {
                    line_buf[line_pos] = '\0';

                    const ErrorCode parse_err = parse_pattern_csv(line_buf, line_pos);
                    if (parse_err == ErrorCode::SUCCESS) {
                        ++pattern_count_;
                    }

                    line_pos = 0;

                    if (pattern_count_ >= MAX_PATTERNS) {
                        return ErrorCode::SUCCESS;
                    }
                }
            } else if (c != '\n' && c != '\r') {
                line_buf[line_pos++] = c;
            }
        }
    }

    return ErrorCode::SUCCESS;
}

ErrorCode PatternManager::parse_pattern_csv(
    const char* csv_line,
    size_t csv_length
) noexcept {
    if (csv_line == nullptr || csv_length == 0) {
        return ErrorCode::INVALID_PARAMETER;
    }

    if (pattern_count_ >= MAX_PATTERNS) {
        return ErrorCode::BUFFER_FULL;
    }

    SignalPattern& pattern = patterns_[pattern_count_];
    pattern = SignalPattern{};

    size_t pos = 0;
    uint8_t field_index = 0;

    while (pos < csv_length && field_index < 27) {
        while (pos < csv_length && (csv_line[pos] == ',' || csv_line[pos] == ' ' || csv_line[pos] == '\t')) {
            ++pos;
        }

        if (pos >= csv_length) break;

        size_t field_start = pos;
        while (pos < csv_length && csv_line[pos] != ',' && csv_line[pos] != '\n' && csv_line[pos] != '\r') {
            ++pos;
        }
        size_t field_end = pos;

        if (field_start >= field_end) break;

        const size_t field_len = field_end - field_start;

        if (field_index == 0) {
            const size_t copy_len = (field_len < PATTERN_NAME_MAX_LEN) ? field_len : PATTERN_NAME_MAX_LEN - 1;
            for (size_t i = 0; i < copy_len; ++i) {
                pattern.name[i] = csv_line[field_start + i];
            }
            pattern.name[copy_len] = '\0';
        } else if (field_index >= 1 && field_index <= 16) {
            const uint8_t bin_idx = field_index - 1;
            if (bin_idx < PATTERN_WAVEFORM_SIZE) {
                pattern.waveform[bin_idx] = parse_uint8(&csv_line[field_start], field_len);
            }
        } else if (field_index == 17) {
            pattern.features.peak_position = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 18) {
            pattern.features.peak_value = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 19) {
            pattern.features.noise_floor = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 20) {
            pattern.features.margin = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 21) {
            pattern.features.width = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 22) {
            pattern.features.sharpness = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 23) {
            pattern.features.flatness = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 24) {
            pattern.features.symmetry = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 25) {
            pattern.match_threshold = parse_uint8(&csv_line[field_start], field_len);
        } else if (field_index == 26) {
            pattern.flags = parse_uint8(&csv_line[field_start], field_len);
        }

        ++field_index;
    }

    if (field_index < 25) {
        return ErrorCode::DATABASE_FORMAT_INVALID;
    }

    pattern.created_time = chTimeNow();
    pattern.match_count = 0;

    return ErrorCode::SUCCESS;
}

ErrorCode PatternManager::save_pattern(const SignalPattern& pattern) noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    if (pattern_count_ >= MAX_PATTERNS) {
        return ErrorCode::BUFFER_FULL;
    }

    f_mkdir(reinterpret_cast<const TCHAR*>(PATTERN_DIR));

    patterns_[pattern_count_] = pattern;
    ++pattern_count_;

    // CRITICAL FIX: Calculate maximum filename size to prevent buffer overflow
    // PATTERN_DIR = "/PATTERNS/" (11 bytes) + "/" + pattern.name (28) + ".TXT" (4) + null = 45 bytes
    constexpr size_t MAX_FILENAME_LEN = sizeof(PATTERN_DIR) + PATTERN_NAME_MAX_LEN + 8;
    char filename[MAX_FILENAME_LEN];
    const int written = snprintf(filename, sizeof(filename), "%s/%s.TXT", PATTERN_DIR, pattern.name);
    
    // Validate filename was not truncated
    if (written < 0 || static_cast<size_t>(written) >= sizeof(filename)) {
        return ErrorCode::INVALID_PARAMETER;
    }

    File file;
    const auto open_err = file.create(reinterpret_cast<const TCHAR*>(filename));
    if (open_err.is_valid()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    // CRITICAL FIX: RAII file guard ensures file is always closed
    struct FileGuard {
        File* const file;
        explicit FileGuard(File* f) noexcept : file(f) {}
        ~FileGuard() { if (file) file->close(); }
        
        FileGuard(const FileGuard&) = delete;
        FileGuard& operator=(const FileGuard&) = delete;
    } file_guard(&file);

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

    if (!write_result.is_ok()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    return ErrorCode::SUCCESS;
}

ErrorCode PatternManager::delete_pattern(size_t index) noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    if (index >= pattern_count_) {
        return ErrorCode::INVALID_PARAMETER;
    }

    // CRITICAL FIX: Calculate maximum filename size to prevent buffer overflow
    constexpr size_t MAX_FILENAME_LEN = sizeof(PATTERN_DIR) + PATTERN_NAME_MAX_LEN + 8;
    char filename[MAX_FILENAME_LEN];
    const int written = snprintf(filename, sizeof(filename), "%s/%s.TXT", PATTERN_DIR, patterns_[index].name);
    
    // Validate filename was not truncated
    if (written < 0 || static_cast<size_t>(written) >= sizeof(filename)) {
        return ErrorCode::INVALID_PARAMETER;
    }

    const auto del_err = delete_file(reinterpret_cast<const TCHAR*>(filename));
    if (!del_err.ok()) {
        return ErrorCode::DATABASE_LOAD_TIMEOUT;
    }

    for (size_t i = index; i < pattern_count_ - 1; ++i) {
        patterns_[i] = patterns_[i + 1];
    }

    if (pattern_count_ > 0) {
        patterns_[pattern_count_ - 1] = SignalPattern{};
    }

    --pattern_count_;

    return ErrorCode::SUCCESS;
}

const SignalPattern* PatternManager::get_pattern(size_t index) const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    if (index >= pattern_count_) {
        return nullptr;
    }

    return &patterns_[index];
}

const SignalPattern* PatternManager::get_patterns_array() const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    return patterns_.data();
}

size_t PatternManager::get_pattern_count() const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);
    return pattern_count_;
}

size_t PatternManager::find_pattern_by_name(const char* name) const noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

    for (size_t i = 0; i < pattern_count_; ++i) {
        if (strcmp(patterns_[i].name, name) == 0) {
            return i;
        }
    }

    return pattern_count_;
}

void PatternManager::clear_all_patterns() noexcept {
    MutexLock<LockOrder::DATABASE_MUTEX> lock(mutex_);

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
