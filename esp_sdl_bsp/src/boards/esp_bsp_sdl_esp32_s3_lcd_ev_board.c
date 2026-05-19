/**
 * @file esp_bsp_sdl_esp32_s3_lcd_ev_board.c
 * @brief ESP32-S3-LCD-EV-Board implementation for ESP-BSP SDL abstraction layer
 * Uses official espressif/esp32_s3_lcd_ev_board_noglib BSP
 * Supports multiple LCD sub-boards: 480x480 (RGB) and 800x480 (RGB)
 */

#include "esp_bsp_sdl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Include ESP32-S3-LCD-EV-Board BSP headers - only when this board is selected
#include "bsp/esp32_s3_lcd_ev_board.h"
#include "bsp/display.h"
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
#    include "bsp/touch.h"
#    include "esp_lcd_touch.h"
#endif

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u

static const char *TAG = "esp_bsp_sdl_esp32_s3_lcd_ev_board";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
static esp_lcd_touch_handle_t s_touch_handle = NULL;
#endif

static esp_err_t esp32_s3_lcd_ev_board_init(esp_bsp_sdl_display_config_t *config,
                                            esp_lcd_panel_handle_t *panel_handle,
                                            esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing ESP32-S3-LCD-EV-Board display using BSP");

    if(!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Step 1: Fill in display configuration
    // Note: ESP32-S3-LCD-EV-Board supports multiple sub-boards with different resolutions
    // For now, we'll default to sub-board 3 (800x480) as it's commonly used
    uint16_t h_res = bsp_display_get_h_res();
    uint16_t v_res = bsp_display_get_v_res();

    if(h_res == 0 || v_res == 0) {
        // Default to sub-board 3 resolution if BSP hasn't determined it yet
        h_res = BSP_LCD_SUB_BOARD_3_H_RES;
        v_res = BSP_LCD_SUB_BOARD_3_V_RES;
    }

    config->width = h_res;
    config->height = v_res;
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (h_res * v_res) * sizeof(uint16_t);
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
    config->has_touch = BSP_CAPS_TOUCH == 1;
#else
    config->has_touch = false;
#endif

    ESP_LOGI(TAG, "Configuring display: %dx%d", config->width, config->height);

    // Step 2: Initialize BSP display using the official BSP
    ESP_LOGI(TAG, "Initializing display panel...");
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = config->max_transfer_sz,
    };

    ret = bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &s_panel_io_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }

    // Step 3: Turn on the display
    ESP_LOGI(TAG, "Enabling display...");
    ret = esp_lcd_panel_disp_on_off(s_panel_handle, true);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on display: %s", esp_err_to_name(ret));
        return ret;
    }

    // Step 4: Backlight is not controllable on ESP32-S3-LCD-EV-Board (always on)
    ESP_LOGI(TAG, "Display backlight is always on (hardware controlled)");

    // Return handles to caller
    *panel_handle = s_panel_handle;
    *panel_io_handle = s_panel_io_handle;

    ESP_LOGI(TAG, "ESP32-S3-LCD-EV-Board display initialized: %dx%d", config->width, config->height);

    return ESP_OK;
}

static esp_err_t esp32_s3_lcd_ev_board_backlight_on(void)
{
    ESP_LOGW(TAG, "ESP32-S3-LCD-EV-Board: Backlight control not supported (always on)");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t esp32_s3_lcd_ev_board_backlight_off(void)
{
    ESP_LOGW(TAG, "ESP32-S3-LCD-EV-Board: Backlight control not supported (always on)");
    return ESP_ERR_NOT_SUPPORTED;
}

static esp_err_t esp32_s3_lcd_ev_board_display_on_off(bool enable)
{
    ESP_LOGD(TAG, "%s display", enable ? "Enabling" : "Disabling");
    if(s_panel_handle) {
        return esp_lcd_panel_disp_on_off(s_panel_handle, enable);
    }
    return ESP_ERR_INVALID_STATE;
}

static esp_err_t esp32_s3_lcd_ev_board_touch_init(void)
{
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
#    if BSP_CAPS_TOUCH == 1
    ESP_LOGI(TAG, "Initializing touch interface");

    // ESP32-S3-LCD-EV-Board supports touch on sub-boards 2 and 3
    const bsp_touch_config_t touch_cfg = {.dummy = NULL};
    esp_err_t ret = bsp_touch_new(&touch_cfg, &s_touch_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Touch interface initialized successfully");
    return ESP_OK;
#    else
    ESP_LOGW(TAG, "Touch not supported on this board configuration");
    return ESP_ERR_NOT_SUPPORTED;
#    endif
#else
    ESP_LOGW(TAG, "Touch support disabled in configuration");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t esp32_s3_lcd_ev_board_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
#    if BSP_CAPS_TOUCH == 1
    if(!touch_info || !s_touch_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    // Read touch data using ESP-LCD touch API
    uint16_t touch_x[1] = {0};
    uint16_t touch_y[1] = {0};
    uint8_t touch_cnt = 0;

    // First read the touch data
    esp_lcd_touch_read_data(s_touch_handle);

    // Then get the coordinates
    bool touched = esp_lcd_touch_get_coordinates(s_touch_handle, touch_x, touch_y, NULL, &touch_cnt, 1);

    touch_info->pressed = touched && (touch_cnt > 0);
    touch_info->x = touch_info->pressed ? (int) touch_x[0] : 0;
    touch_info->y = touch_info->pressed ? (int) touch_y[0] : 0;

    return ESP_OK;
#    else
    return ESP_ERR_NOT_SUPPORTED;
#    endif
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static const char *esp32_s3_lcd_ev_board_get_name(void)
{
    return "ESP32-S3-LCD-EV-Board";
}

static esp_err_t esp32_s3_lcd_ev_board_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing ESP32-S3-LCD-EV-Board");

    // Clean up resources if needed
    if(s_panel_handle) {
        s_panel_handle = NULL;
    }

    if(s_panel_io_handle) {
        s_panel_io_handle = NULL;
    }

#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
    if(s_touch_handle) {
        s_touch_handle = NULL;
    }
#endif

    return ESP_OK;
}

// ESP32-S3-LCD-EV-Board board interface
const esp_bsp_sdl_board_interface_t esp_bsp_sdl_esp32_s3_lcd_ev_board_interface = {
    .init = esp32_s3_lcd_ev_board_init,
    .backlight_on = esp32_s3_lcd_ev_board_backlight_on,
    .backlight_off = esp32_s3_lcd_ev_board_backlight_off,
    .display_on_off = esp32_s3_lcd_ev_board_display_on_off,
    .touch_init = esp32_s3_lcd_ev_board_touch_init,
    .touch_read = esp32_s3_lcd_ev_board_touch_read,
    .get_name = esp32_s3_lcd_ev_board_get_name,
    .deinit = esp32_s3_lcd_ev_board_deinit,
    .board_name = "ESP32-S3-LCD-EV-Board"};