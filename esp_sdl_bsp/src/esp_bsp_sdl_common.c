/**
 * @file esp_bsp_sdl_common.c
 * @brief Runtime board selection for ESP-BSP SDL abstraction layer
 */

#include "esp_bsp_sdl.h"
#include "esp_log.h"
#include "sdkconfig.h"

static const char *TAG = "esp_bsp_sdl";

// Board interface structure is defined in esp_bsp_sdl.h

// Conditional forward declarations for board implementations based on what's being compiled
// Uses BSP_SELECT variables from bsp_selector
#ifdef CONFIG_BSP_SELECT_M5_ATOM_S3
extern const esp_bsp_sdl_board_interface_t esp_bsp_sdl_m5_atom_s3_interface;
#endif
#ifdef CONFIG_BSP_SELECT_ESP_BOX_3
extern const esp_bsp_sdl_board_interface_t esp_bsp_sdl_esp_box_3_interface;
#endif
#ifdef CONFIG_BSP_SELECT_M5STACK_CORE_S3
extern const esp_bsp_sdl_board_interface_t esp_bsp_sdl_m5stack_core_s3_interface;
#endif
#ifdef CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD
extern const esp_bsp_sdl_board_interface_t esp_bsp_sdl_esp32_p4_function_ev_interface;
#endif
#ifdef CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD
extern const esp_bsp_sdl_board_interface_t esp_bsp_sdl_esp32_s3_lcd_ev_board_interface;
#endif
#ifdef CONFIG_BSP_SELECT_M5STACK_TAB5
extern const esp_bsp_sdl_board_interface_t esp_bsp_sdl_m5stack_tab5_interface;
#endif

static const esp_bsp_sdl_board_interface_t *s_current_board = NULL;

// Runtime board detection based on bsp_selector Kconfig variables
static const esp_bsp_sdl_board_interface_t *detect_board(void)
{
#ifdef CONFIG_BSP_SELECT_M5_ATOM_S3
    ESP_LOGI(TAG, "Detected board: M5 Atom S3");
    return &esp_bsp_sdl_m5_atom_s3_interface;
#elif CONFIG_BSP_SELECT_ESP_BOX_3
    ESP_LOGI(TAG, "Detected board: ESP32-S3-BOX-3");
    return &esp_bsp_sdl_esp_box_3_interface;
#elif CONFIG_BSP_SELECT_M5STACK_CORE_S3
    ESP_LOGI(TAG, "Detected board: M5Stack CoreS3");
    return &esp_bsp_sdl_m5stack_core_s3_interface;
#elif CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD
    ESP_LOGI(TAG, "Detected board: ESP32-P4 Function EV Board");
    return &esp_bsp_sdl_esp32_p4_function_ev_interface;
#elif CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD
    ESP_LOGI(TAG, "Detected board: ESP32-S3-LCD-EV-Board");
    return &esp_bsp_sdl_esp32_s3_lcd_ev_board_interface;
#elif CONFIG_BSP_SELECT_M5STACK_TAB5
    ESP_LOGI(TAG, "Detected board: M5Stack Tab5");
    return &esp_bsp_sdl_m5stack_tab5_interface;
#else
    ESP_LOGE(TAG, "No board configuration detected!");
    return NULL;
#endif
}

esp_err_t esp_bsp_sdl_init(esp_bsp_sdl_display_config_t *config,
                           esp_lcd_panel_handle_t *panel_handle,
                           esp_lcd_panel_io_handle_t *panel_io_handle)
{
    ESP_LOGI(TAG, "Initializing ESP-BSP SDL abstraction layer");

    // Detect and select the board at runtime
    s_current_board = detect_board();
    if(!s_current_board) {
        ESP_LOGE(TAG, "Failed to detect board configuration");
        return ESP_ERR_NOT_SUPPORTED;
    }

    ESP_LOGI(TAG, "Selected board: %s", s_current_board->board_name);

    return s_current_board->init(config, panel_handle, panel_io_handle);
}

esp_err_t esp_bsp_sdl_backlight_on(void)
{
    if(!s_current_board) {
        ESP_LOGE(TAG, "Board not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return s_current_board->backlight_on();
}

esp_err_t esp_bsp_sdl_backlight_off(void)
{
    if(!s_current_board) {
        ESP_LOGE(TAG, "Board not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return s_current_board->backlight_off();
}

esp_err_t esp_bsp_sdl_display_on_off(bool enable)
{
    if(!s_current_board) {
        ESP_LOGE(TAG, "Board not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return s_current_board->display_on_off(enable);
}

esp_err_t esp_bsp_sdl_touch_init(void)
{
    if(!s_current_board) {
        ESP_LOGE(TAG, "Board not initialized");
        return ESP_ERR_INVALID_STATE;
    }
    return s_current_board->touch_init();
}

esp_err_t esp_bsp_sdl_touch_read(esp_bsp_sdl_touch_info_t *touch_info)
{
    if(!touch_info) {
        return ESP_ERR_INVALID_ARG;
    }

    if(!s_current_board) {
        ESP_LOGE(TAG, "Board not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    return s_current_board->touch_read(touch_info);
}

const char *esp_bsp_sdl_get_board_name(void)
{
    if(!s_current_board) {
        return "Unknown";
    }
    return s_current_board->board_name;
}

esp_err_t esp_bsp_sdl_deinit(void)
{
    ESP_LOGI(TAG, "Deinitializing ESP-BSP SDL abstraction layer");

    if(!s_current_board) {
        return ESP_OK;
    }

    esp_err_t ret = s_current_board->deinit();
    s_current_board = NULL;
    return ret;
}
