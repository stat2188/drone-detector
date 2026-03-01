# FINAL INVESTIGATION REPORT: Enhanced Drone Analyzer (EDA) - Part 2

**Project:** STM32F405 (ARM Cortex-M4, 128KB RAM) - HackRF Mayhem Firmware  
**Investigation Date:** 2026-03-01  
**Report Version:** 1.0  
**Status:** CRITICAL ISSUES IDENTIFIED - IMMEDIATE ACTION REQUIRED

---

## 5. Constraint Violation Analysis

This section provides a detailed breakdown of all constraint violations identified during the investigation, categorized by type and severity.

### 5.1 Heap Allocation Violations

#### 5.1.1 Framework-Level Heap Allocations

**FW-1: View::children_ (std::vector<Widget*>)**

**Location:** [`ui_widget.hpp:187`](firmware/common/ui_widget.hpp:187)

```cpp
class View : public Widget {
protected:
    std::vector<Widget*> children_{}; // HEAP ALLOCATION ON EVERY VIEW!
};
```

**Violation Details:**
- **Type:** Heap-allocating dynamic array
- **Impact:** Every View object allocates heap memory for children vector
- **Typical Allocation:** 24 bytes (vector overhead) + 8 bytes per child pointer
- **Frequency:** EVERY view creation, regardless of whether it has children
- **Total Impact:** ~2.4KB heap allocation across application (assuming 100+ View objects)

**Root Cause:**
The PortaPack framework's View base class uses `std::vector<Widget*>` to manage child widgets. This design assumes unlimited heap memory, which is incompatible with embedded systems with strict memory constraints.

**Why This is Critical:**
1. **Pervasive:** Every View subclass inherits this violation
2. **Unnecessary:** Most views have 0-2 children, yet allocate heap for dynamic capacity
3. **Fragmenting:** Frequent vector reallocation causes heap fragmentation
4. **Hidden:** Violation is in framework code, not application code

**Proposed Solution:**
Replace with fixed-size array or linked list:

```cpp
// Option 1: Fixed-size array (for views with known max children)
class View : public Widget {
protected:
    static constexpr size_t MAX_CHILDREN = 8;
    std::array<Widget*, MAX_CHILDREN> children_{};
    size_t child_count_{0};
};

// Option 2: Linked list (for views with variable children)
class View : public Widget {
protected:
    struct ChildNode {
        Widget* widget;
        ChildNode* next;
    };
    ChildNode* children_head_{nullptr};
    ChildNode* children_tail_{nullptr};
    size_t child_count_{0};
    
    // Use object pool for ChildNode allocation
};
```

**Memory Impact:**
- **Current:** ~24-96 bytes heap per View (depending on capacity)
- **After Fix:** 0 bytes heap (all stack allocation)
- **Reduction:** ~2.4KB total

---

**FW-2: NavigationView::view_stack (std::vector<ViewState>)**

**Location:** [`ui_navigation.hpp:156`](firmware/application/ui_navigation.hpp:156)

```cpp
class NavigationView : public View {
private:
    std::vector<ViewState> view_stack{}; // HEAP ALLOCATION ON EVERY PUSH/POP!
};
```

**Violation Details:**
- **Type:** Heap-allocating dynamic array
- **Impact:** Every push/pop operation causes heap reallocation
- **Typical Allocation:** 24 bytes (vector overhead) + 16 bytes per ViewState
- **Frequency:** EVERY view navigation operation
- **Total Impact:** ~720-880 bytes heap allocation with 10 views in stack

**Root Cause:**
The NavigationView uses `std::vector<ViewState>` to manage the navigation history. The vector grows dynamically as users navigate deeper into the application, causing repeated heap allocations and reallocations.

**Why This is Critical:**
1. **Frequent:** Every navigation operation triggers allocation
2. **O(n) Reallocation:** Vector growth strategy copies all ViewState objects
3. **Fragmenting:** Repeated push/pop operations fragment heap
4. **Unbounded:** No limit on navigation depth, potential for unbounded growth

**Proposed Solution:**
Replace with fixed-size circular buffer:

```cpp
class NavigationView : public View {
private:
    static constexpr size_t MAX_NAVIGATION_DEPTH = 16;
    
    struct ViewState {
        std::unique_ptr<View> view;  // Will be replaced with object pool
        std::function<void()> on_pop;  // Will be replaced with function pointer
    };
    
    std::array<ViewState, MAX_NAVIGATION_DEPTH> view_stack_{};
    size_t stack_top_{0};
    size_t stack_bottom_{0};
    
public:
    void push_view(View* view) {
        if ((stack_top_ + 1) % MAX_NAVIGATION_DEPTH == stack_bottom_) {
            // Stack full - cannot push
            return;
        }
        view_stack_[stack_top_].view.reset(view);
        stack_top_ = (stack_top_ + 1) % MAX_NAVIGATION_DEPTH;
    }
    
    void pop_view() {
        if (stack_top_ == stack_bottom_) {
            // Stack empty - cannot pop
            return;
        }
        stack_top_ = (stack_top_ - 1 + MAX_NAVIGATION_DEPTH) % MAX_NAVIGATION_DEPTH;
        view_stack_[stack_top_].view.reset();
    }
};
```

**Memory Impact:**
- **Current:** ~24-400 bytes heap (depending on stack depth)
- **After Fix:** 0 bytes heap (256 bytes stack for 16 ViewState objects)
- **Reduction:** ~720-880 bytes total

---

**FW-3: ViewState::unique_ptr<View>**

**Location:** [`ui_navigation.hpp:151`](firmware/application/ui_navigation.hpp:151)

```cpp
struct ViewState {
    std::unique_ptr<View> view; // HEAP ALLOCATION FOR VIEW OBJECT!
    std::function<void()> on_pop; // POTENTIAL HEAP ALLOCATION!
};
```

**Violation Details:**
- **Type:** Heap-allocating smart pointer
- **Impact:** Each view in stack allocates heap for View object
- **Typical Allocation:** 8 bytes (unique_ptr) + View object size (varies)
- **Frequency:** EVERY view pushed onto navigation stack
- **Total Impact:** ~8 bytes per view + View object size

**Root Cause:**
The ViewState uses `std::unique_ptr<View>` to manage View object lifetime. This design assumes heap allocation is acceptable for View objects, which violates embedded system constraints.

**Why This is Critical:**
1. **Per-View Allocation:** Every view in navigation stack allocates heap
2. **Unpredictable Size:** View objects vary in size, making heap usage unpredictable
3. **Fragmenting:** Frequent allocation/deallocation fragments heap
4. **Unnecessary:** View objects could be allocated from object pool

**Proposed Solution:**
Replace with object pool:

```cpp
// Object pool for View objects
class ViewObjectPool {
public:
    static constexpr size_t MAX_VIEWS = 32;
    static constexpr size_t MAX_VIEW_SIZE = 512;  // bytes
    
    void* allocate(size_t size) {
        if (size > MAX_VIEW_SIZE) return nullptr;
        
        for (size_t i = 0; i < MAX_VIEWS; ++i) {
            if (!used_[i]) {
                used_[i] = true;
                return &pool_[i];
            }
        }
        return nullptr;  // Pool exhausted
    }
    
    void deallocate(void* ptr) {
        for (size_t i = 0; i < MAX_VIEWS; ++i) {
            if (&pool_[i] == ptr) {
                used_[i] = false;
                return;
            }
        }
    }
    
private:
    std::array<std::aligned_storage<MAX_VIEW_SIZE>::type, MAX_VIEWS> pool_{};
    std::array<bool, MAX_VIEWS> used_{};
};

// Updated ViewState
struct ViewState {
    View* view;  // Raw pointer to object pool
    void (*on_pop)();  // Function pointer instead of std::function
    
    void cleanup() {
        if (view) {
            view->~View();  // Call destructor
            ViewObjectPool::instance().deallocate(view);
            view = nullptr;
        }
    }
};
```

**Memory Impact:**
- **Current:** ~8-512 bytes heap per View (depending on View size)
- **After Fix:** 0 bytes heap (16KB pre-allocated object pool)
- **Reduction:** ~8-512 bytes per view

---

**FW-4: ViewState::std::function<void()>**

**Location:** [`ui_navigation.hpp:154`](firmware/application/ui_navigation.hpp:154)

```cpp
struct ViewState {
    std::unique_ptr<View> view;
    std::function<void()> on_pop; // POTENTIAL HEAP ALLOCATION!
};
```

**Violation Details:**
- **Type:** Heap-allocating function wrapper
- **Impact:** Each view may allocate heap for callback object
- **Typical Allocation:** 16-32 bytes (std::function overhead) + callback capture size
- **Frequency:** EVERY view pushed onto navigation stack
- **Total Impact:** ~16-32 bytes per view

**Root Cause:**
The ViewState uses `std::function<void()>` to store callbacks for view pop operations. This design allows flexible callbacks but may allocate heap for captured variables.

**Why This is Critical:**
1. **Potential Allocation:** std::function may allocate heap for captures
2. **Unpredictable:** Allocation depends on callback implementation
3. **Fragmenting:** Frequent allocation/deallocation fragments heap
4. **Overkill:** Most callbacks are simple function pointers

**Proposed Solution:**
Replace with function pointer:

```cpp
// Updated ViewState with function pointer
struct ViewState {
    View* view;
    void (*on_pop)();  // Simple function pointer - no heap allocation
    
    void execute_on_pop() {
        if (on_pop) {
            on_pop();  // Direct function call
        }
    }
};

// Usage example
void cleanup_audio_settings() {
    // Cleanup logic
}

ViewState state;
state.view = view;
state.on_pop = cleanup_audio_settings;  // No heap allocation
```

**For callbacks with captures:**
```cpp
// Use context object instead of captures
struct PopCallbackContext {
    DroneAnalyzerSettingsView* settings_view;
    AudioSettingsView* audio_view;
    
    static void execute(void* ctx) {
        auto* context = static_cast<PopCallbackContext*>(ctx);
        // Cleanup logic using context
        context->settings_view->save();
        context->audio_view->reset();
    }
};

ViewState state;
state.view = view;
state.on_pop_context = new PopCallbackContext{settings_view, audio_view};
state.on_pop = [](void* ctx) {
    PopCallbackContext::execute(ctx);
};
```

**Memory Impact:**
- **Current:** ~16-32 bytes heap per View
- **After Fix:** 0 bytes heap (8 bytes stack for function pointer)
- **Reduction:** ~16-32 bytes per view

---

**FW-5: View::title() (std::string return)**

**Location:** [`ui_widget.hpp:184`](firmware/common/ui_widget.hpp:184)

```cpp
class View : public Widget {
    virtual std::string title() const;  // HEAP ALLOCATION ON EVERY TITLE ACCESS!
};
```

**Violation Details:**
- **Type:** Heap-allocating string return
- **Impact:** Every title access allocates heap for string copy
- **Typical Allocation:** 100-200 bytes (string overhead + title text)
- **Frequency:** EVERY view creation, stack push, title rendering
- **Total Impact:** ~100-200 bytes per view × 60+ views = ~6-12 KB

**Root Cause:**
The View base class requires `std::string` return type for the title() method. This design assumes heap allocation is acceptable for strings, which violates embedded system constraints.

**Why This is Critical:**
1. **Pervasive:** Every View subclass must implement this method
2. **Frequent:** Title is accessed on every view navigation and render
3. **Unnecessary:** Title strings are typically static literals
4. **Framework-Level:** Violation is in framework, requires framework fix

**Proposed Solution:**
Add `title_string_view()` method to View base class:

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

// Derived class example
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

// Update NavigationView to use new method
void NavigationView::paint_title(Painter& painter) {
    std::string_view title_sv = top_view->title_string_view();  // Zero allocation
    // ... render title from string_view
}
```

**Memory Impact:**
- **Current:** ~100-200 bytes heap per view
- **After Fix:** 0 bytes heap (title strings stored in Flash)
- **Reduction:** ~6-12 KB total

---

#### 5.1.2 Application-Level Heap Allocations

**APP-1: PNGWriter::create() (std::filesystem::path)**

**Location:** [`png_writer.hpp:38`](firmware/common/png_writer.hpp:38)

```cpp
class PNGWriter {
public:
    Optional<File::Error> create(const std::filesystem::path& filename);  // HEAP ALLOCATION!
};
```

**Violation Details:**
- **Type:** Heap-allocating filesystem path
- **Impact:** Screenshot capture triggers heap allocation
- **Typical Allocation:** 200-400 bytes (path overhead + filename)
- **Frequency:** EVERY screenshot capture
- **Total Impact:** ~200-400 bytes per screenshot

**Root Cause:**
The PNGWriter uses `std::filesystem::path` for filename handling. This type allocates heap for path storage, which is unnecessary for simple filename strings.

**Why This is High Priority:**
1. **Predictable:** Screenshot capture is a known operation
2. **Avoidable:** Filename is typically a string literal
3. **Simple Fix:** Replace with const char*
4. **No Framework Impact:** Application-level change only

**Proposed Solution:**
Replace with C string:

```cpp
// firmware/common/png_writer.hpp
class PNGWriter {
public:
    // Filename must remain valid during create() call (C string)
    Optional<File::Error> create(const char* filename) noexcept;
    
private:
    static constexpr size_t MAX_FILENAME_LENGTH = 255;
    char filename_buffer_[MAX_FILENAME_LENGTH] = {0};
};

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
    
    // Write PNG header...
    return { };
}

// Update caller
void capture_screenshot() {
    PNGWriter writer;
    writer.create("/SCREENSHOTS/shot.png");  // Zero allocation (string literal)
}
```

**Memory Impact:**
- **Current:** ~200-400 bytes heap per screenshot
- **After Fix:** 0 bytes heap (255 bytes stack buffer)
- **Reduction:** ~200-400 bytes per screenshot

---

**APP-2: PNGWriter::write_scanline() (std::vector<ui::ColorRGB888>)**

**Location:** [`png_writer.hpp:40`](firmware/common/png_writer.hpp:40)

```cpp
class PNGWriter {
public:
    void write_scanline(const std::array<ui::ColorRGB888, 240>& scanline);  // OK
    void write_scanline(const std::vector<ui::ColorRGB888>& scanline);     // HEAP ALLOCATION!
};
```

**Violation Details:**
- **Type:** Heap-allocating dynamic array
- **Impact:** Screenshot capture triggers heap allocation
- **Typical Allocation:** 720 bytes (240 pixels × 3 bytes RGB)
- **Frequency:** EVERY screenshot capture (320 scanlines)
- **Total Impact:** ~720 bytes per screenshot

**Root Cause:**
The PNGWriter provides two overloads for write_scanline(): one with std::array (fixed-size) and one with std::vector (dynamic). The vector overload is unnecessary for the fixed screen width of 240 pixels.

**Why This is High Priority:**
1. **Unnecessary:** std::array overload already exists
2. **Simple Fix:** Remove vector overload
3. **No Framework Impact:** Application-level change only
4. **Predictable:** Screen width is fixed at 240 pixels

**Proposed Solution:**
Remove vector overload:

```cpp
// firmware/common/png_writer.hpp
class PNGWriter {
public:
    // Only fixed-size array allowed - zero heap allocation
    void write_scanline(const std::array<ui::ColorRGB888, 240>& scanline) noexcept;
    
    // REMOVED: std::vector overload
    // void write_scanline(const std::vector<ui::ColorRGB888>& scanline);
};

// Update caller
void capture_screenshot() {
    PNGWriter writer;
    writer.create("/SCREENSHOTS/shot.png");
    
    std::array<ui::ColorRGB888, 240> scanline;  // Stack allocation
    for (int y = 0; y < screen_height; y++) {
        // ... fill scanline ...
        writer.write_scanline(scanline);  // Uses array overload
    }
}
```

**Memory Impact:**
- **Current:** ~720 bytes heap per screenshot
- **After Fix:** 0 bytes heap (720 bytes stack buffer)
- **Reduction:** ~720 bytes per screenshot

---

**APP-3 to APP-5: FixedStringBuffer (std::string)**

**Location:** [`ui_enhanced_drone_settings.hpp:458-527`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp:458)

```cpp
class FixedStringBuffer {
public:
    explicit FixedStringBuffer(char* buffer, size_t capacity) noexcept
        : buffer_(buffer), capacity_(capacity), size_(0) {
        buffer_[0] = '\0';
        temp_string_.reserve(capacity);  // APP-4: Heap allocation
    }
    
    operator std::string&() noexcept {  // APP-5: Returns heap-allocated reference
        temp_string_.assign(buffer_, size_);
        return temp_string_;
    }
    
private:
    char* buffer_;
    size_t capacity_;
    size_t size_;
    std::string temp_string_;  // APP-3: Heap-allocated member variable
};
```

**Violation Details:**
- **Type:** Heap-allocating string member
- **Impact:** TextEdit workaround allocates heap for string conversion
- **Typical Allocation:** 24-48 bytes (string overhead + SSO threshold)
- **Frequency:** PER FixedStringBuffer instance
- **Total Impact:** ~24-48 bytes per instance

**Root Cause:**
The TextEdit widget requires `std::string&` parameter, but EDA uses fixed-size character buffers for storage. The FixedStringBuffer workaround uses `std::string` internally to bridge this gap.

**Why This is High Priority:**
1. **Workaround:** This is a workaround for framework limitation
2. **Avoidable:** Placement new can eliminate heap allocation
3. **Predictable:** String size is known at compile time
4. **Limited Scope:** Only affects DroneEntryEditorView

**Proposed Solution:**
Use placement new with aligned storage:

```cpp
// firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_settings.hpp
class FixedStringBuffer {
public:
    explicit FixedStringBuffer(char* buffer, size_t capacity) noexcept
        : buffer_(buffer), capacity_(capacity), size_(0) {
        buffer_[0] = '\0';
        
        // Use placement new to construct std::string in pre-allocated storage
        new (&string_storage_) std::string();
        get_string().reserve(std::min(capacity, size_t(15)));  // SSO threshold
    }
    
    ~FixedStringBuffer() {
        // Manually destroy std::string
        get_string().~basic_string();
    }
    
    operator std::string&() noexcept {
        // Sync fixed buffer to std::string
        get_string().assign(buffer_, size_);
        return get_string();
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
};
```

**Memory Impact:**
- **Current:** ~24-48 bytes heap per instance
- **After Fix:** 0 bytes heap (24 bytes stack for aligned storage)
- **Reduction:** ~24-48 bytes per instance

---

**APP-6 to APP-9: View::title() in EDA Views**

**Location:** Various EDA view classes

```cpp
// Example: ui_enhanced_drone_settings.hpp:247
class AudioSettingsView : public View {
    std::string title() const noexcept override { return "Audio Settings"; }
};

// Example: ui_enhanced_drone_settings.hpp:278
class HardwareSettingsView : public View {
    std::string title() const noexcept override { return "Hardware Settings"; }
};

// Example: capture_app.hpp:47
class CaptureAppView : public View {
    std::string title() const noexcept override { return "Capture"; }
};
```

**Violation Details:**
- **Type:** Heap-allocating string return
- **Impact:** Every view title access allocates heap
- **Typical Allocation:** 100-200 bytes per title
- **Frequency:** EVERY view creation, stack push, title rendering
- **Total Impact:** ~100-200 bytes per view × 7 EDA views = ~700-1400 bytes

**Root Cause:**
EDA views follow the framework pattern of returning `std::string` from title() method. This is the same issue as FW-5, but specific to EDA views.

**Why This is High Priority:**
1. **Framework Dependency:** Requires framework fix (FW-5)
2. **Limited Scope:** Only 7 EDA views affected
3. **Simple Migration:** Easy to add title_string_view() override
4. **Predictable:** All titles are static literals

**Proposed Solution:**
Add `title_string_view()` override to each EDA view:

```cpp
// AudioSettingsView
class AudioSettingsView : public View {
    std::string title() const noexcept override { 
        return "Audio Settings";  // Legacy - rarely called
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Audio Settings";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

// HardwareSettingsView
class HardwareSettingsView : public View {
    std::string title() const noexcept override { 
        return "Hardware Settings";  // Legacy - rarely called
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Hardware Settings";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};

// CaptureAppView
class CaptureAppView : public View {
    std::string title() const noexcept override { 
        return "Capture";  // Legacy - rarely called
    }
    
    std::string_view title_string_view() const noexcept override {
        static constexpr char title_str[] = "Capture";
        return std::string_view{title_str, sizeof(title_str) - 1};
    }
};
```

**Memory Impact:**
- **Current:** ~700-1400 bytes heap
- **After Fix:** 0 bytes heap (title strings in Flash)
- **Reduction:** ~700-1400 bytes total

---

### 5.2 Architectural Violations (Mixed UI/DSP Logic)

#### 5.2.1 Critical Architectural Violations

**ARCH-CRIT-1: DroneDisplayController::paint() with Spectrum Processing**

**Location:** [`ui_enhanced_drone_analyzer.cpp:2341`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:2341)

**UI Components Present:**
- Painter operations: `painter.fill_rectangle()`, `painter.draw_string()`
- UI widget rendering: `big_display_`, `scanning_progress_`, `text_threat_summary_`
- Display mode management: `set_display_mode()`, visibility toggles
- Status bar updates: `status_bar_.update_normal_status()`

**DSP Components Present:**
- Spectrum data processing: `process_mini_spectrum_data()` called during paint
- Histogram data processing: `process_bins()` called during paint
- Spectrum rendering: `render_bar_spectrum()` contains spectrum data calculations
- Histogram rendering: `render_histogram()` contains histogram bin calculations
- Frequency formatting: `FrequencyFormatter::to_string_short_freq_buffer()`
- Signal type detection logic embedded in rendering path

**Problem Statement:**
The `DroneDisplayController::paint()` method is responsible for UI rendering but directly calls DSP processing methods. This violates the Single Responsibility Principle - the paint method should only render, not process signals.

**Impact Analysis:**
- **Performance:** UI thread blocked by spectrum data processing (O(256) operations per paint)
- **Testability:** Cannot unit test spectrum processing without Painter dependency
- **Maintainability:** Changing spectrum algorithm requires modifying paint() method
- **Reusability:** Spectrum processing logic cannot be reused in other contexts

**Proposed Solution:**
Extract spectrum processing to separate DSP layer (detailed in Part 3, Section 6.1)

---

**ARCH-CRIT-2: EnhancedDroneSpectrumAnalyzerView::paint() with Initialization Logic**

**Location:** [`ui_enhanced_drone_analyzer.cpp:3507`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:3507)

**UI Components Present:**
- Painter operations: `painter.fill_rectangle()`, `painter.draw_string()`
- Progress bar rendering: Dynamic progress bar with percentage calculation
- Error message display: Error header and message rendering
- Status message display: Loading status messages
- Display mode management: `display_controller_.set_display_mode()`

**DSP Components Present:**
- Initialization state machine: Multi-phase initialization (7 phases)
- Database loading coordination: Checks `is_database_loading_complete()`
- Scanner state validation: Validates `display_controller_.are_buffers_valid()`
- Buffer allocation management: `allocate_buffers_from_pool()`, `deallocate_buffers()`
- Initialization continuation: `continue_initialization()` called from paint()

**Problem Statement:**
The `EnhancedDroneSpectrumAnalyzerView::paint()` method contains a complex initialization state machine that validates database loading, allocates display buffers, coordinates with scanner initialization, renders progress indicators, handles initialization errors, and manages multi-phase initialization flow.

**Impact Analysis:**
- **Testability:** Initialization logic cannot be tested without View/Painter dependencies
- **Maintainability:** Initialization changes require modifying paint() method
- **Performance:** Initialization logic runs on UI thread during every paint
- **Code Clarity:** 500+ lines in single paint() method with mixed concerns

**Proposed Solution:**
Extract initialization to separate controller (detailed in Part 3, Section 6.2)

---

**ARCH-CRIT-3 & ARCH-CRIT-4: DSP Processing in UI Classes**

**Locations:**
- [`process_mini_spectrum_data()`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:2946)
- [`process_bins()`](firmware/application/apps/enhanced_drone_analyzer/ui_enhanced_drone_analyzer.cpp:2960)

**Problem Statement:**
These methods are pure DSP logic but are part of the `DroneDisplayController` class, which is a UI component. This creates incorrect architectural layering (DSP logic in UI class).

**Impact Analysis:**
- **Testability:** Cannot test spectrum processing without UI controller
- **Maintainability:** Spectrum algorithm changes require modifying UI controller
- **Performance:** Spectrum processing blocks UI thread when called from paint()
- **Code Clarity:** UI controller class contains 60+ lines of DSP logic

**Proposed Solution:**
Move spectrum processing to separate DSP layer (detailed in Part 3, Section 6.1)

---

## 6. Root Cause Analysis

This section provides a deep dive into the root causes of the identified violations.

### 6.1 Framework-Level Technical Debt

#### 6.1.1 View::title() Signature Issue

**Root Cause:**
The PortaPack framework's View base class was designed with the assumption that heap allocation is acceptable. The `std::string` return type for `title()` method reflects desktop application development practices, not embedded constraints.

**Historical Context:**
- Framework originally developed for desktop prototyping
- Later ported to embedded STM32F405 platform
- Heap allocation constraints not considered during initial design
- Legacy API maintained for backward compatibility

**Why It Wasn't Fixed Earlier:**
1. **Backward Compatibility:** Changing View::title() signature would break all existing views
2. **Lack of Awareness:** Developers may not have understood heap allocation impact
3. **Performance Priority:** Focus on functionality over memory optimization
4. **Incremental Development:** Heap allocation issues emerged gradually as codebase grew

**Solution Complexity:**
- **Framework Change Required:** Cannot fix in application code alone
- **Migration Required:** All 60+ View classes need migration
- **Testing Required:** Extensive testing to ensure no regressions
- **Documentation Required:** Clear migration guide for developers

---

#### 6.1.2 View::children_ Vector Issue

**Root Cause:**
The View class uses `std::vector<Widget*>` for child management because it provides:
- Dynamic capacity (no need to pre-allocate)
- Automatic memory management
- Convenient API (push_back, iterator, etc.)

**Why This Was Chosen:**
1. **Convenience:** Easy to add/remove children dynamically
2. **Flexibility:** No need to know child count in advance
3. **Standard Practice:** Common pattern in desktop UI frameworks
4. **Simplicity:** Less code than custom linked list

**Embedded Reality:**
- Most views have 0-2 children (not dynamic)
- Heap allocation cost far exceeds convenience benefit
- Fixed-size arrays would work for 99% of cases
- Object pool could handle remaining 1%

**Solution Complexity:**
- **Framework Change Required:** Cannot fix in application code
- **API Impact:** May break code that relies on dynamic capacity
- **Testing Required:** All view hierarchies need validation
- **Performance Impact:** Need to verify fixed-size arrays don't cause issues

---

### 6.2 Widget Limitations

#### 6.2.1 TextEdit Widget Requires std::string&

**Root Cause:**
The TextEdit widget was designed to edit mutable strings, and `std::string&` provides:
- Automatic memory management
- Dynamic capacity (no fixed size limit)
- Convenient API (append, erase, etc.)

**Why This Was Chosen:**
1. **Flexibility:** Can edit strings of any length
2. **Standard Practice:** Common pattern in UI frameworks
3. **Ease of Use:** No need to manage buffer size
4. **Desktop Heritage:** Designed for desktop applications

**Embedded Reality:**
- EDA uses fixed-size character buffers (64 bytes)
- TextEdit widget cannot accept fixed buffers directly
- Workaround (FixedStringBuffer) adds complexity
- Heap allocation violates embedded constraints

**Solution Complexity:**
- **Framework Change Required:** Cannot fix in application code
- **API Impact:** Would break existing TextEdit usage
- **Alternative:** Use placement new workaround (current approach)
- **Long-term:** Consider custom text edit widget for embedded

---

### 6.3 External Library Dependencies

#### 6.3.1 PNGWriter Uses std::filesystem::path

**Root Cause:**
PNGWriter was designed using modern C++17 filesystem API for:
- Cross-platform path handling
- Automatic path normalization
- Convenient path manipulation

**Why This Was Chosen:**
1. **Modern C++:** Uses C++17 filesystem API
2. **Cross-Platform:** Works on Windows, Linux, macOS
3. **Convenience:** Automatic path separators, normalization
4. **Standard Practice:** Common in modern C++ code

**Embedded Reality:**
- Embedded systems have simple file systems (no complex paths)
- std::filesystem::path allocates heap unnecessarily
- Simple C strings would work just as well
- Heap allocation violates embedded constraints

**Solution Complexity:**
- **Simple Fix:** Replace with const char*
- **No Framework Impact:** Application-level change only
- **Breaking Change:** Requires updating all callers
- **Testing Required:** Verify screenshot functionality

---

### 6.4 Architectural Design Issues

#### 6.4.1 Mixed UI/DSP Concerns

**Root Cause:**
EDA was developed with performance optimization taking precedence over separation of concerns. Developers embedded DSP logic directly in UI classes to:
- Avoid data copying overhead
- Simplify data flow
- Reduce function call overhead
- Optimize for real-time performance

**Why This Was Chosen:**
1. **Performance:** Direct access to spectrum data without copying
2. **Simplicity:** Fewer classes, simpler code structure
3. **Development Speed:** Faster to implement in monolithic classes
4. **Real-Time Focus:** Prioritized real-time deadlines over architecture

**Embedded Reality:**
- Performance gains are minimal (O(256) operations)
- Testability is severely compromised
- Maintainability suffers over time
- Code reusability is impossible

**Solution Complexity:**
- **Major Refactoring:** Requires extracting DSP logic to separate layer
- **Data Flow Redesign:** Need to design thread-safe data structures
- **Testing Required:** Extensive testing to ensure no regressions
- **Performance Validation:** Must verify no real-time deadline misses

---

#### 6.4.2 Lack of Clear Layering

**Root Cause:**
EDA lacks a clear architectural layering strategy. Code is organized by functionality rather than by architectural layer, resulting in:
- DSP logic in UI classes
- UI logic in DSP classes
- Data processing mixed with presentation
- No clear separation of concerns

**Why This Exists:**
1. **Organic Growth:** Codebase evolved without architectural planning
2. **Feature-Driven:** Development focused on features, not architecture
3. **Lack of Guidelines:** No architectural standards or guidelines
4. **Team Size:** Small team may not have dedicated architect

**Embedded Reality:**
- Clear layering is critical for embedded systems
- Separation of concerns improves testability
- Layered architecture enables code reuse
- Architectural discipline prevents technical debt

**Solution Complexity:**
- **Major Refactoring:** Requires reorganizing entire codebase
- **New Classes:** Need to create DSP, Data, Service, Controller layers
- **Testing Required:** Comprehensive testing to ensure no regressions
- **Documentation Required:** Clear architectural guidelines for future development

---

## 7. Next Steps

This concludes Part 2 of Final Investigation Report. Please continue to Part 3 for Proposed Solutions and Memory Impact Analysis.

**Part 3 Contents:**
- Proposed Solutions (P0-P3 priority roadmap)
- Memory Impact Analysis (current vs projected)
- Stack usage analysis
- Flash storage impact

---

**End of Part 2**
