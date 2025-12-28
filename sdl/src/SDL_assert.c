/*
 * Wrapper for SDL/src/SDL_assert.c
 *
 * This wrapper adds #include <stdlib.h> to fix implicit declaration errors
 * for abort() with newer ESP-IDF compiler versions.
 *
 * The rest of the file is included from the upstream SDL implementation.
 */

#include <stdlib.h>
#include "../SDL/src/SDL_assert.c"
