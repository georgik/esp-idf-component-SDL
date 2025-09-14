#ifndef SDL_espidfshared_h_
#define SDL_espidfshared_h_

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
// ABSTRACTION LAYER: Use sdl_bsp instead of direct BSP includes
#include "sdl_bsp.h"
// Include touch handle type
#include "esp_lcd_touch.h"

// Shared handles
extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t panel_io_handle;

// Touch handle (if supported by current board configuration)
extern esp_lcd_touch_handle_t touch_handle;

#endif /* SDL_espidfshared_h_ */
