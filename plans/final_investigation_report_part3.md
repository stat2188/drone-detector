# FINAL INVESTIGATION REPORT: Enhanced Drone Analyzer (EDA) - Part 3

**Project:** STM32F405 (ARM Cortex-M4, 128KB RAM) - HackRF Mayhem Firmware  
**Investigation Date:** 2026-03-01  
**Report Version:** 1.0  
**Status:** CRITICAL ISSUES IDENTIFIED - IMMEDIATE ACTION REQUIRED

---

## 8. Proposed Solutions

This section presents a consolidated solution roadmap with prioritized fixes organized by urgency (P0-P3).

### 8.1 P0 - Critical (Must Fix Immediately)

#### P0-1: Replace View::children_ with Fixed-Size Array

**Priority:** CRITICAL  
**Effort:** 2-3 weeks  
**Risk:** HIGH  
**Impact:** ~2.4KB heap reduction

**Problem:**
[`View::children_`](firmware/common/ui_widget.hpp:187) uses `std::vector<Widget*>` which allocates heap on every View creation.

**Solution:**
Replace with fixed-size array for views with known max children, or linked list for variable children.

```cpp
// firmware/common/ui_widget.hpp
class View : public Widget {
protected:
    // Option 1: Fixed-size array (recommended for most views)
    static constexpr size_t MAX_CHILDREN = 8;
    std::array<Widget*, MAX_CHILDREN> children_{};
    size_t child_count_{0};
    
    // Option 2: Linked list (for views with variable children)
    /*
    struct ChildNode {
        Widget* widget;
        ChildNode* next;
    };
    ChildNode* children_head_{nullptr};
    ChildNode* children_tail_{nullptr};
    size_t child_count_{0};
    */
    
public:
    void add_child(Widget* child) {
        if (child_count_ < MAX_CHILDREN) {
            children_[child_count_++] = child;
        }
        // Handle error: max children exceeded
    }
    
    void remove_child(Widget* child) {
        for (size_t i = 0; i < child_count_; ++i) {
            if (children_[i] == child) {
                // Shift remaining children
                for (size_t j = i; j < child_count_ - 1; ++j) {
                    children_[j] = children_[j + 1];
                }
                children_[--child_count_] = nullptr;
                return;
            }
        }
    }
    
    Widget* get_child(size_t index) const {
        return (index < child_count_) ? children_[index] : nullptr;
    }
    
    size_t child_count() const { return child_count_; }
};
```

**Migration Path:**
1. Update View base class with new implementation
2. Test with existing view hierarchies
3. Increase MAX_CHILDREN if needed for specific views
4. Remove old std::vector implementation

**Testing:**
- Verify all view hierarchies work correctly
- Test with views that have 0, 1, and multiple children
- Verify no memory leaks
- Check stack usage (should be within 4KB limit)

---

#### P0-2: Replace NavigationView::view_stack with Fixed-Size Circular Buffer

**Priority:** CRITICAL  
**Effort:** 1-2 weeks  
**Risk:** MEDIUM  
**Impact:** ~720-880 bytes heap reduction

**Problem:**
[`NavigationView::view_stack`](firmware/application/ui_navigation.hpp:156) uses `std::vector<ViewState>` which allocates heap on every push/pop.

**Solution:**
Replace with fixed-size circular buffer.

```cpp
// firmware/application/ui_navigation.hpp
class NavigationView : public View {
private:
    static constexpr size_t MAX_NAVIGATION_DEPTH = 16;
    
    struct ViewState {
        View* view;  // Raw pointer to object pool (see P0-3)
        void (*on_pop)();  // Function pointer instead of std::function
        
        void cleanup() {
            if (view) {
                view->~View();  // Call destructor
                ViewObjectPool::instance().deallocate(view);
                view = nullptr;
            }
        }
        
        void execute_on_pop() {
            if (on_pop) {
                on_pop();
            }
        }
    };
    
    std::array<ViewState, MAX_NAVIGATION_DEPTH> view_stack_{};
    size_t stack_top_{0};
    size_t stack_bottom_{0};
    
public:
    void push_view(View* view, void (*on_pop)() = nullptr) {
        if ((stack_top_ + 1) % MAX_NAVIGATION_DEPTH == stack_bottom_) {
            // Stack full - cannot push
            // Log error or handle gracefully
            return;
        }
        
        view_stack_[stack_top_].view = view;
        view_stack_[stack_top_].on_pop = on_pop;
        stack_top_ = (stack_top_ + 1) % MAX_NAVIGATION_DEPTH;
        
        // Update top view
        if (stack_top_ != stack_bottom_) {
            set_focus(view);
        }
    }
    
    void pop_view() {
        if (stack_top_ == stack_bottom_) {
            // Stack empty - cannot pop
            return;
        }
        
        stack_top_ = (stack_top_ - 1 + MAX_NAVIGATION_DEPTH) % MAX_NAVIGATION_DEPTH;
        view_stack_[stack_top_].execute_on_pop();
        view_stack_[stack_top_].cleanup();
        
        // Update top view
        if (stack_top_ != stack_bottom_) {
            size_t prev_top = (stack_top_ - 1 + MAX_NAVIGATION_DEPTH) % MAX_NAVIGATION_DEPTH;
            set_focus(view_stack_[prev_top].view);
        }
    }
    
    View* top_view() const {
        if (stack_top_ == stack_bottom_) {
            return nullptr;
        }
        size_t prev_top = (stack_top_ - 1 + MAX_NAVIGATION_DEPTH) % MAX_NAVIGATION_DEPTH;
        return view_stack_[prev_top].view;
    }
    
    size_t depth() const {
        if (stack_top_ >= stack_bottom_) {
            return stack_top_ - stack_bottom_;
        } else {
            return MAX_NAVIGATION_DEPTH - stack_bottom_ + stack_top_;
        }
    }
};
```

**Migration Path:**
1. Update NavigationView with new implementation
2. Replace std::unique_ptr<View> with raw pointers (see P0-3)
3. Replace std::function<void()> with function pointers
4. Test navigation flow (push, pop, back button)
5. Verify no memory leaks

**Testing:**
- Test navigation to maximum depth (16 views)
- Test push/pop cycles
- Verify back button functionality
- Test stack full/empty conditions
- Check memory usage (should be within limits)

---

#### P0-3: Implement View Object Pool

**Priority:** CRITICAL  
**Effort:** 1-2 weeks  
**Risk:** MEDIUM  
**Impact:** ~8-512 bytes heap reduction per View

**Problem:**
`ViewState::unique_ptr<View>` allocates heap for every View object in navigation stack.

**Solution:**
Implement object pool for View objects.

```cpp
// firmware/common/ui_view_pool.hpp (NEW FILE)
#ifndef __UI_VIEW_POOL_HPP__
#define __UI_VIEW_POOL_HPP__

#include <array>
#include <new>
#include <type_traits>

namespace ui {

class ViewObjectPool {
public:
    static constexpr size_t MAX_VIEWS = 32;
    static constexpr size_t MAX_VIEW_SIZE = 512;  // bytes
    
    static ViewObjectPool& instance() {
        static ViewObjectPool pool;
        return pool;
    }
    
    template<typename T, typename... Args>
    T* allocate(Args&&... args) {
        static_assert(sizeof(T) <= MAX_VIEW_SIZE, "View size exceeds pool capacity");
        static_assert(std::is_base_of<View, T>::value, "T must derive from View");
        
        for (size_t i = 0; i < MAX_VIEWS; ++i) {
            if (!used_[i]) {
                used_[i] = true;
                return new (&pool_[i]) T(std::forward<Args>(args)...);
            }
        }
        return nullptr;  // Pool exhausted
    }
    
    void deallocate(View* view) {
        if (!view) return;
        
        for (size_t i = 0; i < MAX_VIEWS; ++i) {
            if (reinterpret_cast<View*>(&pool_[i]) == view) {
                view->~View();
                used_[i] = false;
                return;
            }
        }
        // View not from pool - ignore or log error
    }
    
    size_t allocated_count() const {
        size_t count = 0;
        for (bool used : used_) {
            if (used) ++count;
        }
        return count;
    }
    
    size_t available_count() const {
        return MAX_VIEWS - allocated_count();
    }
    
private:
    ViewObjectPool() = default;
    ~ViewObjectPool() = default;
    ViewObjectPool(const ViewObjectPool&) = delete;
    ViewObjectPool& operator=(const ViewObjectPool&) = delete;
    
    std::array<std::aligned_storage<MAX_VIEW_SIZE>::type, MAX_VIEWS> pool_{};
    std::array<bool, MAX_VIEWS> used_{};
};

} // namespace ui

#endif // __UI_VIEW_POOL_HPP__
```

**Usage Example:**
```cpp
// Allocate view from pool
AudioSettingsView* view = ViewObjectPool::instance().allocate<AudioSettingsView>();

// Use view
navigation_view.push_view(view);

// Deallocate view (called by ViewState::cleanup())
ViewObjectPool::instance().deallocate(view);
```

**Migration Path:**
1. Create ViewObjectPool class
2. Update ViewState to use raw pointers
3. Replace all `new View()` with `ViewObjectPool::instance().allocate<View>()`
4. Replace all `delete view` with `ViewObjectPool::instance().deallocate(view)`
5. Test all view creation/destruction paths

**Testing:**
- Test view allocation/deallocation cycles
- Verify no memory leaks
- Test pool exhaustion (allocate >32 views)
- Verify proper destructor calls
- Check stack usage

---

#### P0-4: Replace ViewState::std::function with Function Pointer

**Priority:** CRITICAL  
**Effort:** 1 week  
**Risk:** LOW  
**Impact:** ~16-32 bytes heap reduction per View

**Problem:**
`ViewState::std::function<void()>` may allocate heap for callback objects.

**Solution:**
Replace with function pointer.

```cpp
// firmware/application/ui_navigation.hpp
struct ViewState {
    View* view;
    void (*on_pop)();  // Simple function pointer - no heap allocation
    
    void execute_on_pop() {
        if (on_pop) {
            on_pop();  // Direct function call
        }
    }
};
```

**For callbacks with captures:**
```cpp
// Use context object instead of captures
struct PopCallbackContext {
    DroneAnalyzerSettingsView* settings_view;
    AudioSettingsView* audio_view;
    bool save_settings;
    
    static void execute(void* ctx) {
        auto* context = static_cast<PopCallbackContext*>(ctx);
        if (context->save_settings) {
            context->settings_view->save();
        }
        context->audio_view->reset();
        delete context;  // Clean up context
    }
};

// Usage
auto* context = new PopCallbackContext{settings_view, audio_view, true};
ViewState state;
state.view = view;
state.on_pop = []() { PopCallbackContext::execute(context); };
```

**Migration Path:**
1. Update ViewState struct
2. Replace all std::function<void()> with function pointers
3. For callbacks with captures, use context objects
4. Test all navigation pop callbacks

**Testing:**
- Test all pop callbacks
- Verify callbacks execute correctly
- Test callbacks with context objects
- Check for memory leaks in context objects

---

#### P0-5: Implement title_string_view() Method

**Priority:** CRITICAL  
**Effort:** 2-3 weeks  
**Risk:** MEDIUM  
**Impact:** ~6-12 KB heap reduction

**Problem:**
[`View::title()`](firmware/common/ui_widget.hpp:184) returns `std::string` which allocates heap on every title access.

**Solution:**
Add `title_string_view()` method to View base class.

```cpp
// firmware/common/ui_widget.hpp
class View : public Widget {
    // Legacy method - kept for backward compatibility
    virtual std::string title() const;
    
    // NEW: Zero-allocation method - returns string_view to static storage
    virtual std::string_view title_string_view() const noexcept {
        // Default implementation returns empty string
        return "";
    }
};

// firmware/common/ui_string_view_helper.hpp (NEW FILE)
#ifndef __UI_STRING_VIEW_HELPER_HPP__
#define __UI_STRING_VIEW_HELPER_HPP__

#include <string_view>
#include <cstring>

namespace ui {

// Helper class to convert constexpr char arrays to string_view
template<size_t N>
struct StaticString {
    constexpr StaticString(const char (&str)[N]) noexcept {
        std::memcpy(data_, str, N);
    }
    
    constexpr operator std::string_view() const noexcept {
        return std::string_view{data_, N - 1};  // Exclude null terminator
    }
    
    constexpr const char* c_str() const noexcept { return data_; }
    constexpr size_t size() const noexcept { return N - 1; }
    
private:
    char data_[N];
};

// Macro to create static string from literal
#define UI_STATIC_STRING(str) []() constexpr { \
    return StaticString<sizeof(str)>(str); \
}()

} // namespace ui

#endif // __UI_STRING_VIEW_HELPER_HPP__
```

**Update Derived View Classes:**
```cpp
// firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp

class AudioSettingsView : public View {
    // Legacy method - kept for framework compatibility
    std::string title() const noexcept override { 
        return "Audio Settings";  // Still allocates, but rarely called
    }
    
    // NEW: Zero-allocation method
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Audio Settings";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

class HardwareSettingsView : public View {
    std::string title() const noexcept override { 
        return "Hardware Settings";
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Hardware Settings";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

class ScanningSettingsView : public View {
    std::string title() const noexcept override { 
        return "Scanning Settings";
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Scanning Settings";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

class DroneAnalyzerSettingsView : public View {
    std::string title() const noexcept override { 
        return "Drone Analyzer";
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Drone Analyzer";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

class LoadingView : public View {
    std::string title() const noexcept override { 
        return "Loading";
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Loading";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

class DroneEntryEditorView : public View {
    std::string title() const noexcept override { 
        return "Edit Entry";
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Edit Entry";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};
```

**Update NavigationView:**
```cpp
// firmware/application/ui_navigation.cpp

// BEFORE:
void NavigationView::paint_title(Painter& painter) {
    std::string title = top_view->title();  // Heap allocation
    // ... render title
}

// AFTER:
void NavigationView::paint_title(Painter& painter) {
    std::string_view title_sv = top_view->title_string_view();  // Zero allocation
    // ... render title from string_view
}
```

**Migration Path:**
1. Add title_string_view() to View base class
2. Create ui_string_view_helper.hpp utility
3. Update NavigationView to use title_string_view()
4. Migrate all 60+ View classes to override title_string_view()
5. Mark legacy title() as [[deprecated]]
6. Test all view title rendering

**Testing:**
- Verify all view titles render correctly
- Test with views that don't override title_string_view()
- Check memory usage (should be significantly reduced)
- Verify no heap allocation during title rendering

---

### 8.2 P1 - High (Should Fix Soon)

#### P1-1: Refactor PNGWriter to Use C Strings

**Priority:** HIGH  
**Effort:** 1 week  
**Risk:** LOW  
**Impact:** ~1KB heap reduction per screenshot

**Problem:**
[`PNGWriter::create()`](firmware/common/png_writer.hpp:38) uses `std::filesystem::path` and [`write_scanline()`](firmware/common/png_writer.hpp:40) uses `std::vector<ui::ColorRGB888>`.

**Solution:**
Replace with C strings and std::array.

```cpp
// firmware/common/png_writer.hpp

class PNGWriter {
public:
    ~PNGWriter();
    
    // NEW: Zero-allocation filename handling
    Optional<File::Error> create(const char* filename) noexcept;
    
    // REMOVED: std::vector overload
    // void write_scanline(const std::vector<ui::ColorRGB888>& scanline);
    
    // KEPT: std::array overload (zero allocation)
    void write_scanline(const std::array<ui::ColorRGB888, 240>& scanline) noexcept;
    
private:
    File file{};
    int scanline_count{0};
    CRC<32, true, true> crc{0x04c11db7, 0xffffffff, 0xffffffff};
    Adler32 adler_32{};
    
    // NEW: Fixed-size filename buffer
    static constexpr size_t MAX_FILENAME_LENGTH = 255;
    char filename_buffer_[MAX_FILENAME_LENGTH] = {0};
    
    void write_chunk_header(const size_t length, const std::array<uint8_t, 4>& type);
    void write_chunk_content(const void* const p, const size_t count);
    
    template <size_t N>
    void write_chunk_content(const std::array<uint8_t, N>& data) {
        write_chunk_content(data.data(), sizeof(data));
    }
    
    void write_chunk_crc();
    void write_uint32_be(const uint32_t v);
};
```

```cpp
// firmware/common/png_writer.cpp

Optional<File::Error> PNGWriter::create(const char* filename) noexcept {
    // Validate input
    if (!filename || filename[0] == '\0') {
        return File::Error::INVALID_PARAMETER;
    }
    
    // Copy filename to internal buffer (stack allocation)
    size_t len = std::strlen(filename);
    if (len >= MAX_FILENAME_LENGTH) {
        return File::Error::NAME_TOO_LONG;
    }
    std::memcpy(filename_buffer_, filename, len + 1);  // Include null terminator
    
    // Create file using internal buffer
    auto result = file.create(filename_buffer_);
    if (result.is_error()) {
        return result.error();
    }
    
    // Write PNG header
    write_png_signature();
    
    return { };
}

void PNGWriter::write_scanline(const std::array<ui::ColorRGB888, 240>& scanline) noexcept {
    // Process scanline - zero heap allocation
    // ... (existing scanline processing code)
}
```

**Update Callers:**
```cpp
// Example caller - screenshot capture

// BEFORE:
void capture_screenshot() {
    PNGWriter writer;
    writer.create(std::filesystem::path("/SCREENSHOTS/shot.png"));  // Heap allocation
    
    std::vector<ui::ColorRGB888> scanline(240);  // Heap allocation
    for (int y = 0; y < screen_height; y++) {
        // ... fill scanline ...
        writer.write_scanline(scanline);  // Uses vector overload
    }
}

// AFTER:
void capture_screenshot() {
    PNGWriter writer;
    writer.create("/SCREENSHOTS/shot.png");  // Zero allocation (string literal)
    
    std::array<ui::ColorRGB888, 240> scanline;  // Stack allocation
    for (int y = 0; y < screen_height; y++) {
        // ... fill scanline ...
        writer.write_scanline(scanline);  // Uses array overload
    }
}
```

**Migration Path:**
1. Update PNGWriter::create() signature to accept const char*
2. Add internal filename buffer
3. Remove std::vector overload
4. Find all callers of PNGWriter
5. Replace std::filesystem::path with C strings
6. Replace std::vector<ui::ColorRGB888> with std::array<ui::ColorRGB888, 240>
7. Test screenshot functionality

**Testing:**
- Test screenshot capture with various filenames
- Test with long filenames (>255 chars)
- Test with invalid filenames
- Verify screenshot quality unchanged
- Check memory usage (should be reduced)

---

#### P1-2: Replace FixedStringBuffer with Placement New

**Priority:** HIGH  
**Effort:** 1 week  
**Risk:** LOW  
**Impact:** ~24-48 bytes heap reduction per instance

**Problem:**
[`FixedStringBuffer`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:458) uses `std::string` member which allocates heap.

**Solution:**
Use placement new with aligned storage.

```cpp
// firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp

class FixedStringBuffer {
public:
    explicit FixedStringBuffer(char* buffer, size_t capacity) noexcept
        : buffer_(buffer), capacity_(capacity), size_(0) {
        buffer_[0] = '\0';
        
        // Use placement new to construct std::string in pre-allocated storage
        new (&string_storage_) std::string();
        
        // Reserve capacity to stay within SSO threshold (typically 15 bytes)
        // If capacity > 15, string will allocate heap - but this is acceptable
        // since it's only temporary during TextEdit operations
        get_string().reserve(std::min(capacity, size_t(15)));
    }
    
    ~FixedStringBuffer() {
        // Manually destroy std::string
        get_string().~basic_string();
    }
    
    // Prevent copying
    FixedStringBuffer(const FixedStringBuffer&) = delete;
    FixedStringBuffer& operator=(const FixedStringBuffer&) = delete;
    
    // Allow moving
    FixedStringBuffer(FixedStringBuffer&& other) noexcept
        : buffer_(other.buffer_), capacity_(other.capacity_), size_(other.size_) {
        // Move std::string to new storage
        new (&string_storage_) std::string(std::move(other.get_string()));
        other.buffer_ = nullptr;
        other.capacity_ = 0;
        other.size_ = 0;
    }
    
    FixedStringBuffer& operator=(FixedStringBuffer&& other) noexcept {
        if (this != &other) {
            // Destroy existing string
            get_string().~basic_string();
            
            // Move from other
            buffer_ = other.buffer_;
            capacity_ = other.capacity_;
            size_ = other.size_;
            new (&string_storage_) std::string(std::move(other.get_string()));
            
            // Reset other
            other.buffer_ = nullptr;
            other.capacity_ = 0;
            other.size_ = 0;
        }
        return *this;
    }
    
    operator std::string&() noexcept {
        // Sync fixed buffer to std::string
        get_string().assign(buffer_, size_);
        return get_string();
    }
    
    void sync_from_string() {
        // Sync std::string back to fixed buffer
        size_t len = std::min(get_string().size(), capacity_ - 1);
        std::memcpy(buffer_, get_string().data(), len);
        buffer_[len] = '\0';
        size_ = len;
    }
    
private:
    char* buffer_;
    size_t capacity_;
    size_t size_;
    
    // Aligned storage for std::string (no heap allocation for small strings)
    alignas(alignof(std::string)) std::byte string_storage_[sizeof(std::string)];
    
    std::string& get_string() noexcept {
        return *reinterpret_cast<std::string*>(string_storage_);
    }
    
    const std::string& get_string() const noexcept {
        return *reinterpret_cast<const std::string*>(string_storage_);
    }
};
```

**Migration Path:**
1. Update FixedStringBuffer implementation
2. Test with DroneEntryEditorView
3. Verify TextEdit widget compatibility
4. Check for memory leaks

**Testing:**
- Test text editing with various string lengths
- Test with strings at SSO threshold (15 bytes)
- Test with strings exceeding SSO threshold
- Verify no memory leaks
- Check stack usage

---

#### P1-3: Extract DSP Processing from UI paint() Methods

**Priority:** HIGH  
**Effort:** 2-3 weeks  
**Risk:** MEDIUM  
**Impact:** Improved testability and maintainability

**Problem:**
[`DroneDisplayController::paint()`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:2341) contains embedded DSP processing.

**Solution:**
Extract spectrum processing to separate DSP layer.

```cpp
// firmware/application/apps/enhanced_drone_analyzer/spectrum_processor.hpp (NEW FILE)
#ifndef __SPECTRUM_PROCESSOR_HPP__
#define __SPECTRUM_PROCESSOR_HPP__

#include <array>
#include "ui_enhanced_drone_analyzer.hpp"

namespace eda {

class SpectrumProcessor {
public:
    struct ProcessedSpectrumData {
        std::array<uint8_t, 256> power_levels;
        std::array<uint16_t, 64> histogram_bins;
        uint8_t noise_floor;
        uint8_t max_power;
        uint32_t signal_width_hz;
        bool is_valid;
    };
    
    ProcessedSpectrumData process(const ChannelSpectrum& spectrum) noexcept;
    
private:
    void extract_power_levels(
        const ChannelSpectrum& spectrum,
        ProcessedSpectrumData& result) noexcept;
    
    void compute_histogram(
        const ProcessedSpectrumData& data,
        Frequency center_frequency,
        uint32_t bandwidth_hz) noexcept;
    
    uint8_t calculate_noise_floor(
        const std::array<uint8_t, 256>& power_levels) noexcept;
    
    uint8_t calculate_max_power(
        const std::array<uint8_t, 256>& power_levels) noexcept;
};

} // namespace eda

#endif // __SPECTRUM_PROCESSOR_HPP__
```

```cpp
// firmware/application/apps/enhanced_drone_analyzer/spectrum_processor.cpp

namespace eda {

SpectrumProcessor::ProcessedSpectrumData SpectrumProcessor::process(
    const ChannelSpectrum& spectrum) noexcept {
    
    ProcessedSpectrumData result{};
    
    // Extract power levels from spectrum data
    extract_power_levels(spectrum, result);
    
    // Calculate noise floor
    result.noise_floor = calculate_noise_floor(result.power_levels);
    
    // Calculate max power
    result.max_power = calculate_max_power(result.power_levels);
    
    // Compute histogram
    compute_histogram(result, spectrum.center_frequency, spectrum.bandwidth);
    
    result.is_valid = true;
    return result;
}

void SpectrumProcessor::extract_power_levels(
    const ChannelSpectrum& spectrum,
    ProcessedSpectrumData& result) noexcept {
    
    // Extract power levels from spectrum FIFO
    for (size_t i = 0; i < 256; ++i) {
        result.power_levels[i] = spectrum.db[i];
    }
}

uint8_t SpectrumProcessor::calculate_noise_floor(
    const std::array<uint8_t, 256>& power_levels) noexcept {
    
    // Calculate median of lower 10% as noise floor
    std::array<uint8_t, 26> noise_samples;
    for (size_t i = 0; i < 26; ++i) {
        noise_samples[i] = power_levels[i];
    }
    
    // Simple median calculation
    std::sort(noise_samples.begin(), noise_samples.end());
    return noise_samples[13];  // Median
}

uint8_t SpectrumProcessor::calculate_max_power(
    const std::array<uint8_t, 256>& power_levels) noexcept {
    
    uint8_t max_power = 0;
    for (uint8_t power : power_levels) {
        if (power > max_power) {
            max_power = power;
        }
    }
    return max_power;
}

void SpectrumProcessor::compute_histogram(
    ProcessedSpectrumData& data,
    Frequency center_frequency,
    uint32_t bandwidth_hz) noexcept {
    
    // Compute histogram bins
    // ... (existing histogram computation logic)
}

} // namespace eda
```

```cpp
// firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp

class DroneDisplayController {
private:
    SpectrumProcessor spectrum_processor_;  // DSP processing
    SpectrumProcessor::ProcessedSpectrumData cached_spectrum_data_;  // Thread-safe cache
    bool spectrum_data_valid_{false};
    
public:
    void paint(Painter& painter) override {
        View::paint(painter);
        
        // Render only using cached_spectrum_data_
        // No DSP calls in paint()
        
        if (spectrum_data_valid_) {
            render_bar_spectrum(painter, cached_spectrum_data_);
            render_histogram(painter, cached_spectrum_data_);
        }
    }
    
    void update_spectrum(const ChannelSpectrum& spectrum) {
        // Called from scanner thread
        cached_spectrum_data_ = spectrum_processor_.process(spectrum);
        spectrum_data_valid_ = true;
        set_dirty();  // Trigger repaint
    }
    
private:
    void render_bar_spectrum(
        Painter& painter,
        const SpectrumProcessor::ProcessedSpectrumData& data);
    
    void render_histogram(
        Painter& painter,
        const SpectrumProcessor::ProcessedSpectrumData& data);
};
```

**Migration Path:**
1. Create SpectrumProcessor class
2. Move DSP logic from DroneDisplayController to SpectrumProcessor
3. Add cached_spectrum_data_ to DroneDisplayController
4. Update paint() to use cached data
5. Add update_spectrum() method to process data from scanner thread
6. Test spectrum rendering

**Testing:**
- Verify spectrum rendering unchanged
- Test with various spectrum data
- Verify no DSP calls in paint()
- Check UI thread performance
- Test thread safety of cached data

---

### 8.3 P2 - Medium (Nice to Have)

#### P2-1: Extract Initialization Logic to Separate Controller

**Priority:** MEDIUM  
**Effort:** 2-3 weeks  
**Risk:** MEDIUM  
**Impact:** Improved testability and code clarity

**Problem:**
[`EnhancedDroneSpectrumAnalyzerView::paint()`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:3507) contains initialization state machine.

**Solution:**
Extract initialization to separate controller.

```cpp
// firmware/application/apps/enhanced_drone_analyzer/init_controller.hpp (NEW FILE)
#ifndef __INIT_CONTROLLER_HPP__
#define __INIT_CONTROLLER_HPP__

namespace eda {

class SpectrumAnalyzerInitializationController {
public:
    enum class InitPhase {
        PHASE_0_WAIT_FOR_DB,
        PHASE_1_ALLOCATE_BUFFERS,
        PHASE_2_INITIALIZE_SCANNER,
        PHASE_3_WAIT_FOR_READY,
        PHASE_4_START_SCANNING,
        PHASE_5_COMPLETE,
        PHASE_6_ERROR
    };
    
    enum class InitError {
        NONE,
        DATABASE_LOAD_FAILED,
        BUFFER_ALLOCATION_FAILED,
        SCANNER_INIT_FAILED,
        TIMEOUT
    };
    
    void start_initialization();
    void step();  // Called from UI event loop
    
    InitPhase get_current_phase() const { return current_phase_; }
    bool is_complete() const { return current_phase_ == InitPhase::PHASE_5_COMPLETE; }
    bool has_error() const { return error_ != InitError::NONE; }
    
    const char* get_error_message() const;
    uint8_t get_progress_percent() const;
    
private:
    InitPhase current_phase_{InitPhase::PHASE_0_WAIT_FOR_DB};
    InitError error_{InitError::NONE};
    uint32_t start_time_{0};
    
    void phase_0_wait_for_db();
    void phase_1_allocate_buffers();
    void phase_2_initialize_scanner();
    void phase_3_wait_for_ready();
    void phase_4_start_scanning();
    
    void set_error(InitError error);
    uint32_t elapsed_time() const;
};

} // namespace eda

#endif // __INIT_CONTROLLER_HPP__
```

```cpp
// firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.hpp

class EnhancedDroneSpectrumAnalyzerView : public View {
private:
    SpectrumAnalyzerInitializationController init_controller_;
    
public:
    void paint(Painter& painter) override {
        View::paint(painter);
        
        if (init_controller_.is_complete()) {
            // Render spectrum display
            display_controller_.render_bar_spectrum(painter);
            display_controller_.render_histogram(painter);
        } else {
            // Render initialization UI
            render_initialization_ui(painter);
        }
    }
    
    void on_tick() override {
        if (!init_controller_.is_complete()) {
            init_controller_.step();
            set_dirty();
        }
    }
    
private:
    void render_initialization_ui(Painter& painter);
};
```

**Migration Path:**
1. Create SpectrumAnalyzerInitializationController class
2. Move initialization logic from paint() to controller
3. Simplify paint() to render only
4. Add on_tick() to drive initialization
5. Test initialization flow

**Testing:**
- Test all initialization phases
- Test error conditions
- Verify UI rendering during initialization
- Check for deadlocks or hangs

---

#### P2-2: Add Error Handling for SPI/File Operations

**Priority:** MEDIUM  
**Effort:** 1 week  
**Risk:** LOW  
**Impact:** Improved robustness

**Problem:**
PNGWriter lacks error handling for SPI/File failures.

**Solution:**
Add comprehensive error handling.

```cpp
// firmware/common/png_writer.hpp

class PNGWriter {
public:
    enum class Error {
        NONE,
        FILE_CREATE_FAILED,
        FILE_WRITE_FAILED,
        SPI_ERROR,
        INVALID_PARAMETER,
        NAME_TOO_LONG,
        OUT_OF_MEMORY
    };
    
    Optional<Error> create(const char* filename) noexcept;
    Optional<Error> write_scanline(const std::array<ui::ColorRGB888, 240>& scanline) noexcept;
    Optional<Error> finalize() noexcept;
    
    const char* get_error_message(Error error) const;
    
private:
    Error last_error_{Error::NONE};
};
```

**Migration Path:**
1. Add error enum to PNGWriter
2. Update all methods to return Optional<Error>
3. Add error message lookup
4. Update callers to handle errors
5. Test error conditions

**Testing:**
- Test with invalid filenames
- Test with full SD card
- Test with SD card removed
- Verify error messages are helpful

---

### 8.4 P3 - Low (Cosmetic Improvements)

#### P3-1: Remove Namespace Pollution

**Priority:** LOW  
**Effort:** 1-2 weeks  
**Risk:** LOW  
**Impact:** Improved code clarity

**Problem:**
EDA code uses `using namespace` directives that pollute the global namespace.

**Solution:**
Remove `using namespace` and use explicit namespace qualifiers.

```cpp
// BEFORE:
using namespace ui;
using namespace eda;

void some_function() {
    auto view = new AudioSettingsView();  // Which namespace?
}

// AFTER:
void some_function() {
    auto view = new eda::AudioSettingsView();  // Explicit
}
```

**Migration Path:**
1. Find all `using namespace` directives
2. Replace with explicit namespace qualifiers
3. Test compilation
4. Verify no functionality changes

---

#### P3-2: Add Global Qualifiers

**Priority:** LOW  
**Effort:** 1 week  
**Risk:** LOW  
**Impact:** Improved code clarity

**Problem:**
Some function calls are ambiguous without global qualifiers.

**Solution:**
Add explicit `::` prefix for global functions.

```cpp
// BEFORE:
void some_function() {
    auto result = strlen(buffer);  // Which strlen?
}

// AFTER:
void some_function() {
    auto result = ::strlen(buffer);  // Explicitly global
}
```

**Migration Path:**
1. Find ambiguous function calls
2. Add `::` prefix for global functions
3. Test compilation
4. Verify no functionality changes

---

## 9. Memory Impact Analysis

This section provides detailed memory usage analysis for current and projected states.

### 9.1 Current Heap Allocation Breakdown

| Category | Component | Heap Size | Frequency | Total Impact |
|----------|-----------|------------|-----------|--------------|
| **Framework** | View::children_ | ~24-96 bytes per View | EVERY View creation | ~2.4 KB |
| **Framework** | NavigationView::view_stack | ~24-400 bytes | EVERY push/pop | ~720-880 bytes |
| **Framework** | ViewState::unique_ptr<View> | ~8-512 bytes per View | EVERY view push | ~8-512 bytes |
| **Framework** | ViewState::std::function<void()> | ~16-32 bytes per View | EVERY view push | ~16-32 bytes |
| **Framework** | View::title() | ~100-200 bytes per View | EVERY title access | ~6-12 KB |
| **Application** | PNGWriter::create() | ~200-400 bytes | EVERY screenshot | ~200-400 bytes |
| **Application** | PNGWriter::write_scanline() | ~720 bytes | EVERY screenshot | ~720 bytes |
| **Application** | FixedStringBuffer | ~24-48 bytes | PER instance | ~24-48 bytes |
| **Application** | EDA View titles | ~100-200 bytes per View | EVERY title access | ~700-1400 bytes |
| **TOTAL** | | | | **~4.3-5.3 KB** |

### 9.2 Projected Heap Allocation After Fixes

| Category | Component | Current Heap | Projected Heap | Reduction |
|----------|-----------|--------------|-----------------|-----------|
| **Framework** | View::children_ | ~2.4 KB | 0 bytes | ~2.4 KB |
| **Framework** | NavigationView::view_stack | ~720-880 bytes | 0 bytes | ~720-880 bytes |
| **Framework** | ViewState::unique_ptr<View> | ~8-512 bytes | 0 bytes | ~8-512 bytes |
| **Framework** | ViewState::std::function<void()> | ~16-32 bytes | 0 bytes | ~16-32 bytes |
| **Framework** | View::title() | ~6-12 KB | 0 bytes | ~6-12 KB |
| **Application** | PNGWriter::create() | ~200-400 bytes | 0 bytes | ~200-400 bytes |
| **Application** | PNGWriter::write_scanline() | ~720 bytes | 0 bytes | ~720 bytes |
| **Application** | FixedStringBuffer | ~24-48 bytes | 0 bytes | ~24-48 bytes |
| **Application** | EDA View titles | ~700-1400 bytes | 0 bytes | ~700-1400 bytes |
| **TOTAL** | | **~4.3-5.3 KB** | **~0.7-1.5 KB** | **~3.5-3.8 KB** |

**Remaining Heap (~0.7-1.5 KB):**
- Temporary allocations during transitions
- Legacy title() calls (rare)
- Edge cases and error handling
- Future extensibility buffer

### 9.3 Stack Usage Analysis

| Component | Stack Size | Notes |
|-----------|------------|-------|
| **View::children_** (fixed array) | 64 bytes | 8 pointers × 8 bytes |
| **NavigationView::view_stack** (circular buffer) | 256 bytes | 16 ViewState × 16 bytes |
| **ViewState** | 16 bytes | 1 pointer + 1 function pointer |
| **PNGWriter** (filename buffer) | 255 bytes | Fixed-size buffer |
| **PNGWriter** (scanline) | 720 bytes | 240 pixels × 3 bytes RGB |
| **FixedStringBuffer** | 88 bytes | 64 byte buffer + 24 byte storage |
| **SpectrumProcessor::ProcessedSpectrumData** | 640 bytes | 256 + 128 + overhead |
| **Total Peak Stack** | ~2.0 KB | Within 4 KB limit |

**Stack Safety Margin:** ~2.0 KB (50% of limit)

### 9.4 Flash Storage Impact

| Component | Flash Size | Notes |
|-----------|------------|-------|
| **View title strings** | ~200 bytes | All view titles in Flash |
| **SpectrumProcessor** | ~2 KB | DSP processing code |
| **InitController** | ~1 KB | Initialization logic |
| **Object pool** | ~16 KB | Pre-allocated for 32 View objects |
| **Total Additional Flash** | ~19.2 KB | Acceptable within 1 MB Flash |

**Flash Usage:** ~19.2 KB additional
**Available Flash:** ~980 KB (after code and constants)
**Utilization:** ~2% increase

---

## 10. Next Steps

This concludes Part 3 of Final Investigation Report. Please continue to Part 4 for Architectural Recommendations, Implementation Roadmap, and Conclusion.

**Part 4 Contents:**
- Architectural Recommendations (layering, file structure, interfaces)
- Implementation Roadmap (phased approach with timelines)
- Conclusion (final verdict and next steps)

---

**End of Part 3**
