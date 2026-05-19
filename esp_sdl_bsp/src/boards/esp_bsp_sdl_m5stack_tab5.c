/**
 * @file esp_bsp_sdl_m5stack_tab5.c
 * @brief M5Stack Tab5 ESP32-P4 specific implementation for ESP-BSP SDL abstraction layer
 * Uses M5Stack Tab5 BSP with NOGLIB mode for SDL compatibility
 *
 * IMPORTANT: M5Stack Tab5 requires 200MHz PSRAM speed for proper operation!
 */

#include "esp_bsp_sdl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Include M5Stack Tab5 BSP headers - only when this board is selected
#include "bsp/display.h"
#include "bsp/m5stack_tab5.h"

#if CONFIG_SDL_BSP_TOUCH_ENABLE
#    include "bsp/touch.h"
#    include "esp_lcd_touch.h"
#endif

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u
#define SDL_PIXELFORMAT_RGB888 0x16161804u

static const char *TAG = "esp_bsp_sdl_m5stack_tab5";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;

#if CONFIG_SDL_BSP_TOUCH_ENABLE
static esp_lcd_touch_handle_t s_touch_handle = NULL;
#endif

static esp_err_t m5stack_tab5_init(esp_bsp_sdl_display_config_t *config,
                                   esp_lcd_panel_handle_t *panel_handle,
                                   esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing M5Stack Tab5 display using BSP");
    ESP_LOGW(TAG, "CRITICAL: Ensure 200MHz PSRAM is configured for proper operation!");

    if(!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Step 1: Fill in display configuration for M5Stack Tab5
    // Native resolution is 720x1280 (portrait), but we use landscape 1280x720 for SDL
    config->width = 1280;  // Landscape width (rotated)
    config->height = 720;  // Landscape height (rotated)

    // M5Stack Tab5 uses RGB565 format for MIPI-DSI
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (config->width * config->height) * 2;  // 2 bytes per pixel for RGB565

#if CONFIG_SDL_BSP_TOUCH_ENABLE
    config->has_touch = BSP_CAPS_TOUCH == 1;
#else
    config->has_touch = false;
#endif

    // Step 2: Initialize BSP display using the official M5Stack Tab5 BSP
    ESP_LOGI(TAG, "Initializing MIPI-DSI display panel (%dx%d)...", config->width, config->height);

    // Use bsp_display_new_with_handles for complete LCD handles
    bsp_lcd_handles_t lcd_handles;
    const bsp_display_config_t bsp_disp_cfg = {
        .dsi_bus = {
            .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
            .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
        },
    };

    ret = bsp_display_new_with_handles(&bsp_disp_cfg, &lcd_handles);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }

    s_panel_handle = lcd_handles.panel;
    s_panel_io_handle = lcd_handles.io;

    // Step 3: MIPI-DSI panels are typically always on after initialization
    ESP_LOGI(TAG, "MIPI-DSI display is ready...");

    // Step 4: Turn on backlight if supported
    ESP_LOGI(TAG, "Turning on backlight...");
    ret = bsp_display_brightness_init();
    if(ret == ESP_OK) {
        ret = bsp_display_backlight_on();
        if(ret != ESP_OK) {
            ESP_LOGW(TAG, "Failed to turn on backlight: %s", esp_err_to_name(ret));
            // Don't fail initialization if backlight fails
        }
    } else {
        ESP_LOGW(TAG, "Backlight initialization failed: %s", esp_err_to_name(ret));
        // Don't fail initialization if backlight init fails
    }

    // Return handles to caller
    *panel_handle = s_panel_handle;
    *panel_io_handle = s_panel_io_handle;

    ESP_LOGI(TAG, "M5Stack Tab5 display initialized: %dx%d (landscape mode)", config->width, config->height);
    ESP_LOGI(TAG, "Display features: MIPI-DSI, RGB565, %s", config->has_touch ? "Touch enabled" : "Touch disabled");

    return ESP_OK;
}

static esp_err_t m5stack_tab5_backlight_on(void)
{
    ESP_LOGI(TAG, "M5Stack Tab5: Turning backlight on");
    esp_err_t ret = bsp_display_backlight_on();
    if(ret != ESP_OK) {
        ESP_LOGW(TAG, "Backlight control not supported: %s", esp_err_to_name(ret));
    }
    return ret;
}

static esp_err_t m5stack_tab5_backlight_off(void)
{
    ESP_LOGI(TAG, "M5Stack Tab5: Turning backlight off");
    esp_err_t ret = bsp_display_backlight_off();
    if(ret != ESP_OK) {
        ESP_LOGW(TAG, "Backlight control not supported: %s", esp_err_to_name(ret));
    }
    return ret;
}

static esp_err_t m5stack_tab5_display_on_off(bool enable)
{
    ESP_LOGD(TAG, "%s display", enable ? "Enabling" : "Disabling");
    // MIPI-DSI displays are typically always on, but try the BSP function if available
    if(s_panel_handle) {
        return esp_lcd_panel_disp_on_off(s_panel_handle, enable);
    }
    return ESP_OK;
}

static esp_err_t m5stack_tab5_touch_init(void)
{
#if CONFIG_SDL_BSP_TOUCH_ENABLE
    ESP_LOGI(TAG, "Initializing GT911 touch interface");

    const bsp_touch_config_t touch_cfg = {
        // M5Stack Tab5 BSP handles GT911 configuration internally
    };
    esp_err_t ret = bsp_touch_new(&touch_cfg, &s_touch_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "GT911 touch interface initialized successfully");
    return ESP_OK;
#else
    ESP_LOGW(TAG, "Touch support disabled via configuration");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t m5stack_tab5_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
#if CONFIG_SDL_BSP_TOUCH_ENABLE
    if(!touch_info) {
        return ESP_ERR_INVALID_ARG;
    }

    if(!s_touch_handle) {
        ESP_LOGW(TAG, "Touch not initialized");
        touch_info->pressed = false;
        touch_info->x = 0;
        touch_info->y = 0;
        return ESP_ERR_INVALID_STATE;
    }

    uint16_t touch_x[1] = {0};
    uint16_t touch_y[1] = {0};
    uint16_t touch_strength[1] = {0};
    uint8_t touch_cnt = 0;

    esp_err_t ret = esp_lcd_touch_read_data(s_touch_handle);
    if(ret == ESP_OK) {
        bool pressed = esp_lcd_touch_get_coordinates(s_touch_handle, touch_x, touch_y, touch_strength, &touch_cnt, 1);
        if(pressed && touch_cnt > 0) {
            touch_info->pressed = true;
            // Convert from native portrait 720x1280 to landscape 1280x720
            // Rotate coordinates 90 degrees clockwise
            touch_info->x = touch_y[0] * 1280 / 720;          // Scale Y to X
            touch_info->y = 720 - (touch_x[0] * 720 / 1280);  // Scale and flip X to Y
        } else {
            touch_info->pressed = false;
            touch_info->x = 0;
            touch_info->y = 0;
        }
    } else {
        touch_info->pressed = false;
        touch_info->x = 0;
        touch_info->y = 0;
    }

    return ESP_OK;
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static const char *m5stack_tab5_get_name(void)
{
    return "M5Stack Tab5";
}

static esp_err_t m5stack_tab5_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing M5Stack Tab5");

    // Clean up touch resources
#if CONFIG_SDL_BSP_TOUCH_ENABLE
    if(s_touch_handle) {
        s_touch_handle = NULL;
    }
#endif

    // Clean up display resources
    if(s_panel_handle) {
        s_panel_handle = NULL;
    }

    if(s_panel_io_handle) {
        s_panel_io_handle = NULL;
    }

    return ESP_OK;
}

// M5Stack Tab5 board interface
const esp_bsp_sdl_board_interface_t esp_bsp_sdl_m5stack_tab5_interface = {.init = m5stack_tab5_init,
                                                                          .backlight_on = m5stack_tab5_backlight_on,
                                                                          .backlight_off = m5stack_tab5_backlight_off,
                                                                          .display_on_off = m5stack_tab5_display_on_off,
                                                                          .touch_init = m5stack_tab5_touch_init,
                                                                          .touch_read = m5stack_tab5_touch_read,
                                                                          .get_name = m5stack_tab5_get_name,
                                                                          .deinit = m5stack_tab5_deinit,
                                                                          .board_name = "M5Stack Tab5"};