#ifndef SDL_espidfshared_h_
#define SDL_espidfshared_h_

#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_bsp_sdl.h"

// Shared handles
extern esp_lcd_panel_handle_t panel_handle;
extern esp_lcd_panel_io_handle_t panel_io_handle;

extern esp_bsp_sdl_display_config_t display_config;

#ifdef ESP_BSP_SDL_TOUCH_SUPPORT
extern esp_lcd_touch_handle_t touch_handle;
#endif

#endif /* SDL_espidfshared_h_ */
