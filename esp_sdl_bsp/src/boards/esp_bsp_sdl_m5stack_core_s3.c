/**
 * @file esp_bsp_sdl_m5stack_core_s3.c
 * @brief M5Stack CoreS3 implementation for ESP-BSP SDL abstraction layer
 * Uses official espressif/m5stack_core_s3_noglib BSP
 */

#include "esp_bsp_sdl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Include M5Stack Core S3 BSP headers - only when this board is selected
#include "bsp/m5stack_core_s3.h"
#include "bsp/touch.h"
#include "esp_lcd_touch.h"

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u

static const char *TAG = "esp_bsp_sdl_m5stack_core_s3";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;
static esp_lcd_touch_handle_t s_touch_handle = NULL;

static esp_err_t m5stack_core_s3_init(esp_bsp_sdl_display_config_t *config,
                                      esp_lcd_panel_handle_t *panel_handle,
                                      esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing M5Stack Core S3 display using BSP");

    if(!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Step 1: Fill in display configuration for M5Stack Core S3 (320x240)
    config->width = 320;   // M5Stack Core S3 specific resolution
    config->height = 240;  // M5Stack Core S3 specific resolution
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (320 * 240) * sizeof(uint16_t);
    config->has_touch = BSP_CAPS_TOUCH == 1;

    // Step 2: Initialize backlight PWM control FIRST (M5Stack Core S3 requires this)
    ESP_LOGI(TAG, "Initializing backlight control...");
    ret = bsp_display_brightness_init();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize backlight PWM: %s", esp_err_to_name(ret));
        return ret;
    }

    // Step 3: Initialize BSP display using the official BSP
    ESP_LOGI(TAG, "Initializing display panel...");
    const bsp_display_config_t bsp_disp_cfg = {
        .max_transfer_sz = config->max_transfer_sz,
    };

    ret = bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &s_panel_io_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }

    // Step 4: Turn on the display
    ESP_LOGI(TAG, "Enabling display...");
    ret = esp_lcd_panel_disp_on_off(s_panel_handle, true);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on display: %s", esp_err_to_name(ret));
        return ret;
    }

    // Step 5: Turn on backlight (M5Stack Core S3 specific)
    ESP_LOGI(TAG, "Turning on backlight...");
    ret = bsp_display_backlight_on();
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to turn on backlight: %s", esp_err_to_name(ret));
        return ret;
    }

    // Return handles to caller
    *panel_handle = s_panel_handle;
    *panel_io_handle = s_panel_io_handle;

    ESP_LOGI(TAG, "M5Stack Core S3 display initialized: %dx%d", config->width, config->height);

    return ESP_OK;
}

static esp_err_t m5stack_core_s3_backlight_on(void)
{
    ESP_LOGI(TAG, "M5Stack Core S3: Turning backlight on");
    return bsp_display_backlight_on();
}

static esp_err_t m5stack_core_s3_backlight_off(void)
{
    ESP_LOGI(TAG, "M5Stack Core S3: Turning backlight off");
    return bsp_display_backlight_off();
}

static esp_err_t m5stack_core_s3_display_on_off(bool enable)
{
    ESP_LOGD(TAG, "%s display", enable ? "Enabling" : "Disabling");
    if(s_panel_handle) {
        return esp_lcd_panel_disp_on_off(s_panel_handle, enable);
    }
    return ESP_ERR_INVALID_STATE;
}

static esp_err_t m5stack_core_s3_touch_init(void)
{
#if BSP_CAPS_TOUCH == 1
    ESP_LOGI(TAG, "Initializing touch interface");

    // M5Stack Core S3 has capacitive touch support
    const bsp_touch_config_t touch_cfg = {.dummy = NULL};
    esp_err_t ret = bsp_touch_new(&touch_cfg, &s_touch_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Touch interface initialized successfully");
    return ESP_OK;
#else
    ESP_LOGW(TAG, "Touch not supported on this board configuration");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t m5stack_core_s3_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
#if BSP_CAPS_TOUCH == 1
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
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static const char *m5stack_core_s3_get_name(void)
{
    return "M5Stack Core S3";
}

static esp_err_t m5stack_core_s3_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing M5Stack Core S3");

    // Clean up resources if needed
    if(s_panel_handle) {
        s_panel_handle = NULL;
    }

    if(s_panel_io_handle) {
        s_panel_io_handle = NULL;
    }

    return ESP_OK;
}

// M5Stack CoreS3 board interface
const esp_bsp_sdl_board_interface_t esp_bsp_sdl_m5stack_core_s3_interface = {
    .init = m5stack_core_s3_init,
    .backlight_on = m5stack_core_s3_backlight_on,
    .backlight_off = m5stack_core_s3_backlight_off,
    .display_on_off = m5stack_core_s3_display_on_off,
    .touch_init = m5stack_core_s3_touch_init,
    .touch_read = m5stack_core_s3_touch_read,
    .get_name = m5stack_core_s3_get_name,
    .deinit = m5stack_core_s3_deinit,
    .board_name = "M5Stack CoreS3"};
