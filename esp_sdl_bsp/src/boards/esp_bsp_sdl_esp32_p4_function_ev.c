/**
 * @file esp_bsp_sdl_esp32_p4_function_ev.c
 * @brief ESP32-P4 Function EV Board specific implementation for ESP-BSP SDL abstraction layer
 * Uses official espressif/esp32_p4_function_ev_board_noglib BSP
 */

#include "esp_bsp_sdl.h"
#include "esp_err.h"
#include "esp_log.h"
#include "sdkconfig.h"

// Include ESP32-P4 Function EV Board BSP headers - only when this board is selected
#include "bsp/display.h"
#include "bsp/esp32_p4_function_ev_board.h"

// Forward declarations for touch support to avoid including problematic headers
// The managed BSP component doesn't have esp_lcd_touch dependency, so we include it directly
#include "esp_lcd_touch.h"

// Define BSP touch types to avoid including bsp/touch.h which causes compilation errors
typedef struct {
    void *dummy;  /*!< Reserved for future use. */
} bsp_touch_config_t;

// Function prototype for bsp_touch_new (from the BSP) 
esp_err_t bsp_touch_new(const bsp_touch_config_t *config, esp_lcd_touch_handle_t *ret_touch);

// SDL pixel format constants - using direct values to avoid SDL dependency
#define SDL_PIXELFORMAT_RGB565 0x15151002u
#define SDL_PIXELFORMAT_RGB888 0x16161804u

static const char *TAG = "esp_bsp_sdl_esp32_p4_function_ev";
static esp_lcd_panel_handle_t s_panel_handle = NULL;
static esp_lcd_panel_io_handle_t s_panel_io_handle = NULL;
static esp_lcd_touch_handle_t s_touch_handle = NULL;

static esp_err_t esp32_p4_function_ev_init(esp_bsp_sdl_display_config_t *config,
                                           esp_lcd_panel_handle_t *panel_handle,
                                           esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing ESP32-P4 Function EV Board display using BSP");

    if(!config || !panel_handle || !panel_io_handle) {
        return ESP_ERR_INVALID_ARG;
    }

    esp_err_t ret = ESP_OK;

    // Step 1: Fill in display configuration for ESP32-P4 Function EV Board
    // Default LCD is 1280x800 ili9881c, but can be configured via menuconfig
#ifdef CONFIG_BSP_LCD_TYPE_1024_600
    config->width = 1024;  // EK79007 LCD resolution 1024x600
    config->height = 600;
#elif defined(CONFIG_BSP_LCD_TYPE_1280_800)
    config->width = 1280;  // ILI9881C LCD resolution 1280x800
    config->height = 800;
#else
    config->width = 1280;  // Default resolution
    config->height = 800;
#endif

    // Set pixel format based on configuration
#ifdef CONFIG_BSP_LCD_RGB888
    config->pixel_format = SDL_PIXELFORMAT_RGB888;
    config->max_transfer_sz = (config->width * config->height) * 3;  // 3 bytes per pixel for RGB888
#else
    config->pixel_format = SDL_PIXELFORMAT_RGB565;
    config->max_transfer_sz = (config->width * config->height) * 2;  // 2 bytes per pixel for RGB565
#endif

#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
    config->has_touch = BSP_CAPS_TOUCH == 1;
#else
    config->has_touch = false;
#endif

    // Step 2: Initialize BSP display using the official BSP
    ESP_LOGI(TAG, "Initializing display panel (%dx%d)...", config->width, config->height);
    const bsp_display_config_t bsp_disp_cfg = {
        .hdmi_resolution = BSP_HDMI_RES_NONE,  // Use LCD, not HDMI
        .dsi_bus =
            {
                .phy_clk_src = MIPI_DSI_PHY_CLK_SRC_DEFAULT,
                .lane_bit_rate_mbps = BSP_LCD_MIPI_DSI_LANE_BITRATE_MBPS,
            },
    };

    ret = bsp_display_new(&bsp_disp_cfg, &s_panel_handle, &s_panel_io_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize BSP display: %s", esp_err_to_name(ret));
        return ret;
    }

    // Step 3: DPI panels don't support disp_on_off, they're always on
    ESP_LOGI(TAG, "Display is ready (DPI panels are always on)...");

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

    ESP_LOGI(TAG, "ESP32-P4 Function EV Board display initialized: %dx%d", config->width, config->height);

    return ESP_OK;
}

static esp_err_t esp32_p4_function_ev_backlight_on(void)
{
    ESP_LOGI(TAG, "ESP32-P4 Function EV Board: Turning backlight on");
    esp_err_t ret = bsp_display_backlight_on();
    if(ret != ESP_OK) {
        ESP_LOGW(TAG, "Backlight control not supported: %s", esp_err_to_name(ret));
    }
    return ret;
}

static esp_err_t esp32_p4_function_ev_backlight_off(void)
{
    ESP_LOGI(TAG, "ESP32-P4 Function EV Board: Turning backlight off");
    esp_err_t ret = bsp_display_backlight_off();
    if(ret != ESP_OK) {
        ESP_LOGW(TAG, "Backlight control not supported: %s", esp_err_to_name(ret));
    }
    return ret;
}

static esp_err_t esp32_p4_function_ev_display_on_off(bool enable)
{
    ESP_LOGD(TAG, "%s display (DPI panels are always on)", enable ? "Enabling" : "Disabling");
    // DPI panels don't support disp_on_off, they're always on
    return ESP_OK;
}

static esp_err_t esp32_p4_function_ev_touch_init(void)
{
#if BSP_CAPS_TOUCH == 1
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
    ESP_LOGI(TAG, "Initializing touch interface");

    const bsp_touch_config_t touch_cfg = {
        .dummy = NULL,  // Reserved for future use
    };
    esp_err_t ret = bsp_touch_new(&touch_cfg, &s_touch_handle);
    if(ret != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize touch: %s", esp_err_to_name(ret));
        return ret;
    }

    ESP_LOGI(TAG, "Touch interface initialized successfully");
    return ESP_OK;
#else
    ESP_LOGW(TAG, "Touch disabled via CONFIG_SDL_BSP_TOUCH_ENABLE");
    return ESP_ERR_NOT_SUPPORTED;
#endif
#else
    ESP_LOGW(TAG, "Touch not supported on this board configuration");
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static esp_err_t esp32_p4_function_ev_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
#if BSP_CAPS_TOUCH == 1
#ifdef CONFIG_SDL_BSP_TOUCH_ENABLE
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
            touch_info->x = touch_x[0];
            touch_info->y = touch_y[0];
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
    // Touch is disabled via configuration
    if(touch_info) {
        touch_info->pressed = false;
        touch_info->x = 0;
        touch_info->y = 0;
    }
    return ESP_ERR_NOT_SUPPORTED;
#endif
#else
    return ESP_ERR_NOT_SUPPORTED;
#endif
}

static const char *esp32_p4_function_ev_get_name(void)
{
    return "ESP32-P4 Function EV Board";
}

static esp_err_t esp32_p4_function_ev_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing ESP32-P4 Function EV Board");

    // Clean up touch resources
    if(s_touch_handle) {
        s_touch_handle = NULL;
    }

    // Clean up display resources
    if(s_panel_handle) {
        s_panel_handle = NULL;
    }

    if(s_panel_io_handle) {
        s_panel_io_handle = NULL;
    }

    return ESP_OK;
}

// ESP32-P4 Function EV Board interface
const esp_bsp_sdl_board_interface_t esp_bsp_sdl_esp32_p4_function_ev_interface = {
    .init = esp32_p4_function_ev_init,
    .backlight_on = esp32_p4_function_ev_backlight_on,
    .backlight_off = esp32_p4_function_ev_backlight_off,
    .display_on_off = esp32_p4_function_ev_display_on_off,
    .touch_init = esp32_p4_function_ev_touch_init,
    .touch_read = esp32_p4_function_ev_touch_read,
    .get_name = esp32_p4_function_ev_get_name,
    .deinit = esp32_p4_function_ev_deinit,
    .board_name = "ESP32-P4 Function EV Board"};