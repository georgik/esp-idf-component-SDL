/**
 * The file contains missing implementation from ESP-IDF.
 * It's intended as a workaround until ESP-IDF implemets those pieces of code.
 **/

#include <stdint.h>
#include <stddef.h>
#include <pthread.h>
#include <sched.h>
#include "SDL_thread.h"

typedef uint32_t sigset_t;

int sigemptyset(sigset_t *set) {
    if (set == NULL) {
        return -1;
    }
    *set = 0;
    return 0;
}

int sigaddset(sigset_t *set, int signo) {
    if (set == NULL || signo < 1 || signo > 31) {
        return -1;
    }
    *set |= (1U << (signo - 1));
    return 0;
}

/**
 * Wrapper function for SDL_SYS_SetThreadPriority
 */
int __wrap_SDL_SYS_SetThreadPriority(SDL_ThreadPriority priority) { return 0; }

/**
 * Stub for pthread_setschedparam which is missing in ESP-IDF
 */
int pthread_setschedparam(pthread_t thread, int policy, const struct sched_param *param) {
    // Dummy implementation: do nothing and return success
    return 0;
}

/**
 * Stub for pthread_setschedparam which is missing in ESP-IDF
 */
int pthread_getschedparam(pthread_t thread, int *policy, struct sched_param *param) {
    if (policy) {
        *policy = SCHED_OTHER; // Default scheduling policy
    }
    if (param) {
        param->sched_priority = 0; // Default priority
    }
    return 0;
}
