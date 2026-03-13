# Architectural Blueprint: eda_locking.hpp Fixes - Part 4

**Date:** 2026-03-13
**Target:** STM32F405 (ARM Cortex-M4, 128KB RAM, 4KB stack per thread)
**Context:** Subtask 2 - Architectural Blueprint for Locking System Fixes

---

## SECTION 8: IMPLEMENTATION PRIORITY

### 8.1 Phase 1: Critical Fixes (Must Do Immediately)

**Timeline:** Immediate (before any other work)

**Fixes:**

1. **CriticalSection nesting_count Fix (1.1)**
   - **Priority:** CRITICAL
   - **Risk:** High (current code is broken)
   - **Impact:** Nested critical sections don't work correctly
   - **Effort:** 1-2 hours
   - **Dependencies:** None

2. **MutexTryLock push_lock Timing Fix (1.2)**
   - **Priority:** CRITICAL
   - **Risk:** Medium (lock tracking is incorrect)
   - **Impact:** Lock order violations not detected correctly
   - **Effort:** 1-2 hours
   - **Dependencies:** None

3. **SDCardLock push_lock Timing Fix (1.3)**
   - **Priority:** CRITICAL
   - **Risk:** Medium (lock tracking is incorrect)
   - **Impact:** Lock order violations not detected correctly
   - **Effort:** 1-2 hours
   - **Dependencies:** None

4. **ChibiOS API Verification (1.4)**
   - **Priority:** CRITICAL
   - **Risk:** High (API mismatch causes undefined behavior)
   - **Impact:** Potential crashes or deadlocks
   - **Effort:** 2-4 hours
   - **Dependencies:** None

5. **SDCardLock Validation Logic Fix (1.5)**
   - **Priority:** CRITICAL
   - **Risk:** Low (validation is unreliable anyway)
   - **Impact:** Simplifies code, removes unreliable heuristic
   - **Effort:** 1 hour
   - **Dependencies:** None

**Total Effort:** 6-11 hours

**Testing Required:**
- Unit tests for `CriticalSection` nesting
- Unit tests for `MutexTryLock` lock tracking
- Unit tests for `SDCardLock` lock tracking
- Integration test with ChibiOS API

**Acceptance Criteria:**
- All critical fixes implemented and tested
- No regressions in existing functionality
- All unit tests pass
- Code compiles without warnings

---

### 8.2 Phase 2: High Priority Fixes (Before Production)

**Timeline:** Before production release

**Fixes:**

1. **Complete LockOrder Enum (2.1)**
   - **Priority:** HIGH
   - **Risk:** Medium (breaking change to API)
   - **Impact:** Enables proper lock order validation for all mutexes
   - **Effort:** 4-6 hours
   - **Dependencies:** Phase 1 complete

2. **Lock Order Documentation Update (2.1)**
   - **Priority:** HIGH
   - **Risk:** Low (documentation only)
   - **Impact:** Clear guidance for developers
   - **Effort:** 2-3 hours
   - **Dependencies:** Complete LockOrder enum

3. **Compile-Time Lock Order Validation (2.1)**
   - **Priority:** HIGH
   - **Risk:** Low (compile-time checks only)
   - **Impact:** Catches lock order errors at compile time
   - **Effort:** 2-3 hours
   - **Dependencies:** Complete LockOrder enum

4. **LockOrderTracker Duplicate Detection (3.1)**
   - **Priority:** HIGH
   - **Risk:** Low (adds safety check)
   - **Impact:** Prevents duplicate lock acquisition
   - **Effort:** 2-3 hours
   - **Dependencies:** Phase 1 complete

**Total Effort:** 10-15 hours

**Testing Required:**
- Compile all code with new LockOrder enum
- Verify all mutexes use correct LockOrder values
- Test duplicate lock detection
- Test compile-time validation

**Acceptance Criteria:**
- All mutexes in codebase use correct LockOrder values
- Compile-time validation catches invalid lock orders
- Duplicate lock detection works correctly
- All unit tests pass

---

### 8.3 Phase 3: Medium Priority Fixes (Next Iteration)

**Timeline:** Next development iteration

**Fixes:**

1. **SDCardLock Timeout Parameter Usage (Medium Priority #1)**
   - **Priority:** MEDIUM
   - **Risk:** Low (parameter is unused)
   - **Impact:** API consistency, potential future timeout support
   - **Effort:** 2-3 hours
   - **Dependencies:** Phase 1 complete

2. **StackMonitor Conservative Behavior Fix (Medium Priority #2)**
   - **Priority:** MEDIUM
   - **Risk:** Low (optimization only)
   - **Impact:** More accurate stack monitoring
   - **Effort:** 2-3 hours
   - **Dependencies:** None

3. **AtomicFlag Volatile with Atomic Built-ins (Medium Priority #3)**
   - **Priority:** MEDIUM
   - **Risk:** Low (code works, just documentation)
   - **Impact:** Clearer intent, better documentation
   - **Effort:** 1-2 hours
   - **Dependencies:** None

4. **LockOrderTracker Overflow Handling (3.2)**
   - **Priority:** MEDIUM
   - **Risk:** Low (adds safety check)
   - **Impact:** Better debugging, no silent failures
   - **Effort:** 2-3 hours
   - **Dependencies:** Phase 2 complete

**Total Effort:** 7-11 hours

**Testing Required:**
- Test timeout parameter (if implemented)
- Test stack monitoring accuracy
- Verify AtomicFlag documentation
- Test overflow handling

**Acceptance Criteria:**
- Timeout parameter used or documented as unused
- Stack monitoring is accurate
- AtomicFlag documentation is clear
- Overflow handling works correctly

---

### 8.4 Phase 4: Low Priority Fixes (Nice to Have)

**Timeline:** Future iterations

**Fixes:**

1. **Explicit Bounds Checking on lock_stack_ Array Access (Low Priority #1)**
   - **Priority:** LOW
   - **Risk:** Very Low (array access is already bounds-checked)
   - **Impact:** Defensive programming, safety margin
   - **Effort:** 1-2 hours
   - **Dependencies:** Phase 2 complete

2. **Const Correctness Improvements (Low Priority #2)**
   - **Priority:** LOW
   - **Risk:** Very Low (code works, just style)
   - **Impact:** Better code quality, clearer intent
   - **Effort:** 3-4 hours
   - **Dependencies:** Phase 2 complete

**Total Effort:** 4-6 hours

**Testing Required:**
- Verify bounds checking doesn't break anything
- Verify const correctness doesn't break anything

**Acceptance Criteria:**
- All array accesses have explicit bounds checking
- All methods that can be const are const

---

### 8.5 Phase 5: Code Organization (Optional)

**Timeline:** After all fixes are complete and tested

**Fixes:**

1. **Split eda_locking.hpp into Multiple Files (5.2-5.11)**
   - **Priority:** OPTIONAL
   - **Risk:** Medium (breaking change to include structure)
   - **Impact:** Better code organization, each file < 800 lines
   - **Effort:** 8-12 hours
   - **Dependencies:** All phases complete

**Total Effort:** 8-12 hours

**Testing Required:**
- Compile all code with new file structure
- Verify all includes work correctly
- Verify no circular dependencies
- Run all tests

**Acceptance Criteria:**
- All files < 800 lines
- No circular dependencies
- All includes work correctly
- All tests pass

---

### 8.6 Implementation Summary

**Total Effort Estimate:**

| Phase | Effort (hours) | Priority | Timeline |
|--------|----------------|-----------|----------|
| Phase 1: Critical Fixes | 6-11 | CRITICAL | Immediate |
| Phase 2: High Priority Fixes | 10-15 | HIGH | Before production |
| Phase 3: Medium Priority Fixes | 7-11 | MEDIUM | Next iteration |
| Phase 4: Low Priority Fixes | 4-6 | LOW | Future iterations |
| Phase 5: Code Organization | 8-12 | OPTIONAL | After all fixes |
| **Total** | **35-55** | - | - |

**Recommended Implementation Order:**

1. **Week 1:** Phase 1 - Critical Fixes (6-11 hours)
2. **Week 2:** Phase 2 - High Priority Fixes (10-15 hours)
3. **Week 3:** Phase 3 - Medium Priority Fixes (7-11 hours)
4. **Week 4:** Phase 4 - Low Priority Fixes (4-6 hours)
5. **Week 5:** Phase 5 - Code Organization (8-12 hours)

**Total Duration:** 5 weeks (at 8 hours per week)

---

### 8.7 Risk Mitigation

**Phase 1 Risks:**

| Risk | Probability | Impact | Mitigation |
|-------|-------------|--------|------------|
| ChibiOS API mismatch | Medium | High | Verify API before implementing |
| Breaking existing code | Low | High | Comprehensive testing |
| Stack overflow | Very Low | High | Stack monitoring |

**Phase 2 Risks:**

| Risk | Probability | Impact | Mitigation |
|-------|-------------|--------|------------|
| Wrong LockOrder values | Medium | Medium | Compile-time validation |
| Breaking changes | Low | High | Update all usages |
| Missing mutexes | Low | Medium | Code review |

**Phase 3 Risks:**

| Risk | Probability | Impact | Mitigation |
|-------|-------------|--------|------------|
| Timeout not supported | Low | Low | Document as unused |
| Stack monitoring inaccuracy | Low | Low | Testing |
| Overflow handling not needed | Low | Low | Debug-only code |

**Phase 4 Risks:**

| Risk | Probability | Impact | Mitigation |
|-------|-------------|--------|------------|
| Bounds checking overhead | Very Low | Very Low | Compile-time optimization |
| Const correctness breaks | Very Low | Low | Testing |

**Phase 5 Risks:**

| Risk | Probability | Impact | Mitigation |
|-------|-------------|--------|------------|
| Circular dependencies | Low | High | Dependency graph |
| Include errors | Medium | Medium | Testing |
| Breaking changes | Low | High | Update all includes |

---

### 8.8 Testing Strategy

**Unit Testing:**

```cpp
// Test CriticalSection nesting
TEST(CriticalSection, Nesting) {
    EXPECT_EQ(0, get_nesting_count());
    {
        CriticalSection lock1;
        EXPECT_EQ(1, get_nesting_count());
        {
            CriticalSection lock2;
            EXPECT_EQ(2, get_nesting_count());
        }
        EXPECT_EQ(1, get_nesting_count());
    }
    EXPECT_EQ(0, get_nesting_count());
}

// Test MutexTryLock lock tracking
TEST(MutexTryLock, LockTracking) {
    Mutex mtx;
    chMtxInit(&mtx);
    
    {
        MutexTryLock lock1(mtx, LockOrder::DATA_MUTEX);
        EXPECT_TRUE(lock1.is_locked());
        EXPECT_TRUE(is_lock_held(LockOrder::DATA_MUTEX));
        
        {
            MutexTryLock lock2(mtx, LockOrder::STATE_MUTEX);
            EXPECT_FALSE(lock2.is_locked());
            EXPECT_FALSE(is_lock_held(LockOrder::STATE_MUTEX));
        }
        
        EXPECT_TRUE(is_lock_held(LockOrder::DATA_MUTEX));
    }
    
    EXPECT_FALSE(is_lock_held(LockOrder::DATA_MUTEX));
}

// Test duplicate lock detection
TEST(LockOrderTracker, DuplicateDetection) {
    Mutex mtx;
    chMtxInit(&mtx);
    
    {
        MutexLock lock1(mtx, LockOrder::DATA_MUTEX);
        EXPECT_TRUE(is_lock_held(LockOrder::DATA_MUTEX));
        
        // Try to acquire same lock again (should fail)
        bool result = LockOrderTracker::instance().push_lock(LockOrder::DATA_MUTEX);
        EXPECT_FALSE(result);  // Duplicate detected
    }
}
```

**Integration Testing:**

1. **Lock Order Validation:**
   - Create test that acquires locks in wrong order
   - Verify lock order violation is detected
   - Verify deadlock is prevented

2. **Stack Monitoring:**
   - Create test with known stack usage
   - Verify StackMonitor reports correct free space
   - Verify safety margin is applied

3. **ChibiOS API Compatibility:**
   - Test with actual ChibiOS version
   - Verify all mutex operations work correctly
   - Verify no crashes or deadlocks

**Regression Testing:**

1. Run all existing EDA tests
2. Verify no performance degradation
3. Verify no memory leaks
4. Verify no stack overflows

---

### 8.9 Rollback Plan

**If Critical Fixes Cause Issues:**

1. **Immediate Rollback:** Revert to previous version of `eda_locking.hpp`
2. **Investigate:** Analyze what went wrong
3. **Fix:** Implement corrected fix
4. **Test:** Comprehensive testing before re-deploying

**If High Priority Fixes Cause Issues:**

1. **Partial Rollback:** Revert specific fix that caused issues
2. **Investigate:** Analyze root cause
3. **Fix:** Implement corrected fix
4. **Test:** Comprehensive testing

**If Code Organization Causes Issues:**

1. **Revert:** Keep single `eda_locking.hpp` file
2. **Alternative:** Reduce file size without splitting
3. **Document:** Explain why split wasn't feasible

---

### 8.10 Success Metrics

**Code Quality Metrics:**

| Metric | Target | Current | Status |
|---------|---------|---------|--------|
| Critical defects | 0 | 5 | Pending |
| High priority defects | 0 | 4 | Pending |
| Medium priority defects | 0 | 4 | Pending |
| Low priority defects | 0 | 2 | Pending |
| Lines per file | < 800 | 993 | Pending |
| Stack usage per thread | < 4KB | 176 bytes | ✓ |
| RAM usage | < 1% | 0.13% | ✓ |

**Performance Metrics:**

| Metric | Target | Current | Status |
|---------|---------|---------|--------|
| Lock acquisition overhead | < 100 cycles | ~40-60 cycles | ✓ |
| Lock tracking overhead | < 10 cycles | ~4 cycles | ✓ |
| Stack monitoring overhead | < 100 cycles | ~50 cycles | ✓ |
| Total overhead per lock | < 200 cycles | ~100-150 cycles | ✓ |

**Testing Metrics:**

| Metric | Target | Status |
|---------|---------|--------|
| Unit test coverage | > 80% | Pending |
| Integration tests pass | 100% | Pending |
| Regression tests pass | 100% | Pending |
| No memory leaks | 0 leaks | Pending |
| No stack overflows | 0 overflows | Pending |

---

## APPENDIX A: QUICK REFERENCE

### A.1 Critical Fixes Checklist

- [ ] Fix CriticalSection nesting_count (1.1)
- [ ] Fix MutexTryLock push_lock timing (1.2)
- [ ] Fix SDCardLock push_lock timing (1.3)
- [ ] Verify ChibiOS API signature (1.4)
- [ ] Fix SDCardLock validation logic (1.5)

### A.2 High Priority Fixes Checklist

- [ ] Complete LockOrder enum (2.1)
- [ ] Update lock order documentation (2.1)
- [ ] Add compile-time validation (2.1)
- [ ] Add duplicate lock detection (3.1)

### A.3 Medium Priority Fixes Checklist

- [ ] Use SDCardLock timeout parameter (Medium #1)
- [ ] Fix StackMonitor conservative behavior (Medium #2)
- [ ] Document AtomicFlag volatile usage (Medium #3)
- [ ] Add LockOrderTracker overflow handling (3.2)

### A.4 Low Priority Fixes Checklist

- [ ] Add explicit bounds checking (Low #1)
- [ ] Improve const correctness (Low #2)

### A.5 Code Organization Checklist

- [ ] Create eda_locking_core.hpp
- [ ] Create eda_locking_tracker.hpp
- [ ] Create eda_locking_wrappers.hpp
- [ ] Create eda_locking_stack.hpp
- [ ] Create eda_locking_init.hpp
- [ ] Update all includes
- [ ] Test new file structure
- [ ] Delete old eda_locking.hpp

---

## APPENDIX B: FILE STRUCTURE

### B.1 Proposed File Structure

```
firmware/application/apps/enhanced_drone_analyzer/
├── eda_locking_core.hpp       (~250 lines)
├── eda_locking_tracker.hpp     (~150 lines)
├── eda_locking_wrappers.hpp   (~350 lines)
├── eda_locking_stack.hpp      (~120 lines)
├── eda_locking_init.hpp       (~50 lines)
└── eda_locking.hpp            (master include, ~20 lines)
```

### B.2 Dependency Graph

```
eda_locking_core.hpp (foundation)
    ├── eda_locking_tracker.hpp
    ├── eda_locking_wrappers.hpp
    │   └── eda_locking_tracker.hpp
    ├── eda_locking_stack.hpp
    └── eda_locking_init.hpp

eda_locking.hpp (master include)
    ├── eda_locking_core.hpp
    ├── eda_locking_tracker.hpp
    ├── eda_locking_wrappers.hpp
    ├── eda_locking_stack.hpp
    └── eda_locking_init.hpp
```

---

## APPENDIX C: GLOSSARY

**Term Definitions:**

- **RAII (Resource Acquisition Is Initialization):** C++ idiom where resource acquisition is done in constructor and release in destructor.
- **thread_local:** C++11 keyword that declares a variable with thread-local storage (each thread has its own copy).
- **constexpr:** C++11 keyword that declares a compile-time constant (evaluated at compile time).
- **noexcept:** C++11 keyword that declares a function never throws exceptions.
- **LIFO (Last In, First Out):** Data structure where last element added is first element removed.
- **Mutex (Mutual Exclusion):** Synchronization primitive that ensures only one thread can access a resource at a time.
- **Critical Section:** Region of code where interrupts are disabled to ensure atomic operations.
- **Lock Order:** Hierarchical ordering of mutexes to prevent deadlock.
- **Deadlock:** Situation where two or more threads are waiting for each other, causing all to block indefinitely.
- **Priority Inversion:** Situation where a high-priority thread is blocked by a low-priority thread holding a mutex.
- **Thread-Local Storage (TLS):** Memory that is local to each thread (not shared between threads).
- **Flash:** Non-volatile memory that retains contents when power is off (read-only in normal operation).
- **RAM:** Volatile memory that requires power to retain contents (read-write in normal operation).
- **BSS Segment:** Memory segment for uninitialized static variables (zero-initialized at startup).

---

## CONCLUSION

This architectural blueprint provides a comprehensive plan for fixing all 15 defects identified in `eda_locking.hpp`:

**Critical Fixes (5):** Must be implemented immediately to ensure correct behavior.
**High Priority Fixes (4):** Should be implemented before production to enable proper lock order validation.
**Medium Priority Fixes (4):** Should be implemented in the next iteration to improve code quality.
**Low Priority Fixes (2):** Can be implemented in future iterations for code quality improvements.
**Code Organization (Optional):** Can be implemented after all fixes are complete to improve maintainability.

**Total Effort:** 35-55 hours (5 weeks at 8 hours per week)

**Key Principles:**
- Zero-overhead release builds
- RAII pattern consistency
- ChibiOS RTOS compatibility
- Stack usage under 4KB
- Diamond Code standards
- Code organization under 800 lines per file

**Next Steps:**
1. Review and approve this blueprint
2. Switch to Code mode for implementation
3. Implement Phase 1 (Critical Fixes)
4. Test and validate all fixes
5. Proceed to Phase 2 (High Priority Fixes)
6. Continue through remaining phases

---

**End of Part 4**

This completes Part 4 of architectural blueprint, covering:
- Section 8: Implementation Priority (8.1-8.10)
- Appendix A: Quick Reference
- Appendix B: File Structure
- Appendix C: Glossary
- Conclusion

**All Parts Complete:**
- Part 1: Critical Fixes Architecture & LockOrder Enhancement
- Part 2: LockOrderTracker Improvements & Stack Optimization & Code Organization
- Part 3: Memory Placement Strategy & Red Team Attack Plan
- Part 4: Implementation Priority & Appendices
