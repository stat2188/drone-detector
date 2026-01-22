#ifndef CHIBIOS_MOCK_HPP_
#define CHIBIOS_MOCK_HPP_

#include <cstdint>
#include <cstddef>

typedef uint32_t systime_t;

struct Thread {
    void* dummy;
};

struct Mutex {
    uint32_t dummy;
};

struct Semaphore {
    uint32_t dummy;
};

typedef int32_t msg_t;

inline void chMtxLock(Mutex* mtx) { (void)mtx; }
inline void chMtxUnlock() { }
inline msg_t chThdSleepMilliseconds(uint32_t ms) {
    (void)ms;
    return 0;
}
inline Thread* chThdCreateStatic(void* mem, size_t size, int prio, void* func, void* arg) {
    (void)mem; (void)size; (void)prio; (void)func; (void)arg;
    return nullptr;
}
inline void chThdTerminate(Thread* tp) { (void)tp; }
inline void chThdWait(Thread* tp) { (void)tp; }

#endif // CHIBIOS_MOCK_HPP_
