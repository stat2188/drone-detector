#ifndef EDA_SAFE_STRING_HPP_
#define EDA_SAFE_STRING_HPP_

#include <cstddef>
#include <cstdint>
#include <string_view>
#include <cassert>

namespace ui::apps::enhanced_drone_analyzer {

// Safe String Copy
inline size_t safe_strcpy(char* dest, const char* src, size_t buffer_size) noexcept {
    // Guard clause: null or zero-size buffer returns early
    if (!dest || buffer_size == 0) {
        return 0;
    }
    
    // Guard clause: null source becomes empty string
    if (!src) {
        dest[0] = '\0';
        return 0;
    }
    
    // Guard clause: single-byte buffer can only hold terminator
    if (buffer_size == 1) {
        dest[0] = '\0';
        return 0;
    }
    
    size_t i = 0;
    while (i < buffer_size - 1 && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    
    dest[i] = '\0';
    
    return i;
}

// Safe String Concatenate
inline size_t safe_strcat(char* dest, const char* src, size_t buffer_size) noexcept {
    // Guard clause: null or zero-size buffer returns early
    if (!dest || buffer_size == 0) {
        return 0;
    }
    
    // Guard clause: null source is a no-op
    if (!src) {
        return 0;
    }
    
    size_t dest_len = 0;
    while (dest_len < buffer_size - 1 && dest[dest_len] != '\0') {
        ++dest_len;
    }
    
    // Guard clause: already at capacity
    if (dest_len >= buffer_size - 1) {
        return 0;
    }
    
    size_t i = 0;
    while (dest_len + i < buffer_size - 1 && src[i] != '\0') {
        dest[dest_len + i] = src[i];
        ++i;
    }
    
    dest[dest_len + i] = '\0';
    
    return i;
}

// Safe String Length
inline size_t safe_strlen(const char* str, size_t max_len) noexcept {
    // Guard clause: null pointer returns 0
    if (!str) {
        return 0;
    }
    
    size_t len = 0;
    while (len < max_len && str[len] != '\0') {
        ++len;
    }
    
    return len;
}

// Safe String Compare
inline int safe_strcmp(const char* str1, const char* str2, size_t max_len) noexcept {
    // Guard clause: both null treated as equal
    if (!str1 && !str2) {
        return 0;
    }
    
    // Guard clause: null sorts before non-null
    if (!str1) {
        return -1;
    }
    
    // Guard clause: non-null sorts after null
    if (!str2) {
        return 1;
    }
    
    for (size_t i = 0; i < max_len; ++i) {
        if (str1[i] == '\0' && str2[i] == '\0') {
            return 0;
        }
        if (str1[i] == '\0') {
            return -1;
        }
        if (str2[i] == '\0') {
            return 1;
        }
        if (str1[i] != str2[i]) {
            return static_cast<int>(static_cast<unsigned char>(str1[i])) -
                   static_cast<int>(static_cast<unsigned char>(str2[i]));
        }
    }
    
    return 0;
}

// Safe String N-Copy
inline size_t safe_strncpy(char* dest, const char* src, size_t n, size_t buffer_size) noexcept {
    // Guard clause: null or zero-size buffer returns early
    if (!dest || buffer_size == 0) {
        return 0;
    }
    
    // Guard clause: null source becomes empty string
    if (!src) {
        dest[0] = '\0';
        return 0;
    }
    
    // Guard clause: single-byte buffer can only hold terminator
    if (buffer_size == 1) {
        dest[0] = '\0';
        return 0;
    }
    
    const size_t max_copy = (n < buffer_size - 1) ? n : buffer_size - 1;
    
    size_t i = 0;
    while (i < max_copy && src[i] != '\0') {
        dest[i] = src[i];
        ++i;
    }
    
    dest[i] = '\0';
    
    return i;
}

// Fixed String Buffer
template<size_t N>
class FixedString {
public:
    FixedString() noexcept {
        buffer_[0] = '\0';
        length_ = 0;
        verify_invariants();
    }

    explicit FixedString(const char* str) noexcept {
        set(str);
        verify_invariants();
    }

    explicit FixedString(std::string_view sv) noexcept {
        set(sv);
        verify_invariants();
    }

    size_t set(const char* str) noexcept {
        length_ = safe_strcpy(buffer_, str, N);
        verify_invariants();
        return length_;
    }

    size_t set(std::string_view sv) noexcept {
        // Guard clause: empty view clears the buffer
        if (sv.empty()) {
            buffer_[0] = '\0';
            length_ = 0;
            verify_invariants();
            return 0;
        }

        const size_t copy_len = (sv.size() < N - 1) ? sv.size() : N - 1;
        
        for (size_t i = 0; i < copy_len; ++i) {
            buffer_[i] = sv.data()[i];
        }
        buffer_[copy_len] = '\0';
        length_ = copy_len;
        
        verify_invariants();
        return copy_len;
    }

    size_t append(const char* str) noexcept {
        size_t appended = safe_strcat(buffer_, str, N);
        length_ = safe_strlen(buffer_, N);
        verify_invariants();
        return appended;
    }

    size_t append(std::string_view sv) noexcept {
        size_t current_len = safe_strlen(buffer_, N);

        // DIAMOND FIX: Ensure null-termination when buffer is full
        // Guard clause: already at capacity
        if (current_len >= N - 1) {
            buffer_[current_len] = '\0';
            length_ = current_len;
            verify_invariants();
            return 0;
        }

        // Guard clause: empty view is a no-op
        if (sv.empty()) {
            verify_invariants();
            return 0;
        }

        const size_t space = N - 1 - current_len;
        const size_t append_len = (sv.size() < space) ? sv.size() : space;

        for (size_t i = 0; i < append_len; ++i) {
            buffer_[current_len + i] = sv.data()[i];
        }
        buffer_[current_len + append_len] = '\0';
        length_ = current_len + append_len;

        verify_invariants();
        return append_len;
    }

    bool would_fit_append(std::string_view sv) const noexcept {
        size_t current_len = safe_strlen(buffer_, N);
        return (current_len + sv.size()) < N;
    }

    bool would_fit_append(const char* str) const noexcept {
        if (!str) return true;
        size_t current_len = safe_strlen(buffer_, N);
        size_t str_len = 0;
        while (str[str_len] != '\0' && (current_len + str_len) < N) {
            ++str_len;
        }
        return (current_len + str_len) < N;
    }

    void clear() noexcept {
        buffer_[0] = '\0';
        length_ = 0;
        verify_invariants();
    }

    const char* c_str() const noexcept {
        return buffer_;
    }

    size_t length() const noexcept {
        return length_;
    }

    bool empty() const noexcept {
        return length_ == 0;
    }

    std::string_view view() const noexcept {
        return std::string_view{buffer_, length_};
    }

    static constexpr size_t capacity() noexcept {
        return N - 1;
    }

    static constexpr size_t stack_usage() noexcept {
        return N;
    }

    static constexpr bool fits(const char* str) noexcept {
        if (!str) return true;
        size_t len = 0;
        while (str[len] != '\0') {
            if (++len >= N - 1) return false;
        }
        return true;
    }

    char operator[](size_t index) const noexcept {
        return (index < length_) ? buffer_[index] : '\0';
    }

private:
    char buffer_[N];
    size_t length_;

    inline void verify_invariants() const noexcept {
#ifdef NDEBUG
        (void)0;
#else
        assert(length_ < N && "FixedString: length_ exceeds buffer capacity");
        assert(buffer_[length_] == '\0' && "FixedString: buffer not null-terminated at length_");
        assert(safe_strlen(buffer_, N) == length_ && "FixedString: length_ mismatch with actual string length");
#endif
    }
};

// Common Buffer Type Aliases
using TinyString = FixedString<8>;
using SmallString = FixedString<16>;
using TitleBuffer = FixedString<32>;
using DescriptionBuffer = FixedString<64>;
using ErrorMessageBuffer = FixedString<128>;
using PathBuffer = FixedString<256>;

} // namespace ui::apps::enhanced_drone_analyzer

#endif // EDA_SAFE_STRING_HPP_
