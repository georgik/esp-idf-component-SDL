/**
 * The file contains missing implementation from ESP-IDF.
 * It's intended as a workaround until ESP-IDF implemets those pieces of code.
 **/

#include <stdint.h>
#include <stddef.h>

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
