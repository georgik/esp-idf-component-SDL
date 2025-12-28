/*
 * Wrapper for SDL/src/stdlib/SDL_malloc.c
 *
 * This wrapper adds #include <stdlib.h> to fix implicit declaration errors
 * for malloc, calloc, and realloc with newer ESP-IDF compiler versions.
 *
 * The rest of the file is included from the upstream SDL implementation.
 */

#include <stdlib.h>
#include "../../SDL/src/stdlib/SDL_malloc.c"
