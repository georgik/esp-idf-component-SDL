/**
 * @file board_init.c
 * @brief Board-specific initialization for SDL example
 */

#include "esp_log.h"
#include "esp_err.h"
#include "sdkconfig.h"

static const char *TAG = "board_init";

#if CONFIG_BSP_SELECT_M5STACK_TAB5 || CONFIG_BSP_SELECT_ESP_BOX_3 || \
    CONFIG_BSP_SELECT_M5STACK_CORE_S3 || CONFIG_BSP_SELECT_M5_ATOM_S3 || \
    CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD || CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD

esp_err_t board_init_display(void)
{
    // SDL video driver handles all display initialization internally via esp_sdl_bsp
    // Just call SDL_Init() later to trigger display init
    ESP_LOGI(TAG, "Display will be initialized by SDL video driver");
    return ESP_OK;
}

#else

// Fallback for unsupported boards
esp_err_t board_init_display(void)
{
    ESP_LOGE(TAG, "No board selected or board not supported!");
    ESP_LOGE(TAG, "Please select a board via idf.py menuconfig:");
    ESP_LOGE(TAG, "  Component config -> ESP-BSP -> BSP board selection");
    return ESP_FAIL;
}

#endif
