/**
 * @file esp_bsp_sdl.h
 * @brief ESP-BSP SDL Abstraction Layer
 *
 * This component provides a board-agnostic abstraction layer between SDL and ESP-BSP.
 * It allows SDL to work with different ESP boards without needing to know the specific
 * BSP implementation details.
 */

#pragma once

#include "esp_err.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_vendor.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Display configuration structure for SDL abstraction
 */
typedef struct {
    int width;              /*!< Display width in pixels */
    int height;             /*!< Display height in pixels */
    int pixel_format;       /*!< SDL pixel format */
    size_t max_transfer_sz; /*!< Maximum transfer size for display operations */
    bool has_touch;         /*!< Whether the display has touch capability */
} esp_bsp_sdl_display_config_t;

/**
 * @brief Touch information structure
 */
typedef struct {
    bool pressed; /*!< Touch state */
    int x;        /*!< Touch X coordinate */
    int y;        /*!< Touch Y coordinate */
} esp_bsp_sdl_touch_info_t;

/**
 * @brief Board interface function pointer structure (for internal use)
 */
typedef struct {
    esp_err_t (*init)(esp_bsp_sdl_display_config_t *config,
                      esp_lcd_panel_handle_t *panel_handle,
                      esp_lcd_panel_io_handle_t *panel_io_handle);
    esp_err_t (*backlight_on)(void);
    esp_err_t (*backlight_off)(void);
    esp_err_t (*display_on_off)(bool enable);
    esp_err_t (*touch_init)(void);
    esp_err_t (*touch_read)(esp_bsp_sdl_touch_info_t *touch_info);
    const char *(*get_name)(void);
    esp_err_t (*deinit)(void);
    const char *board_name;
} esp_bsp_sdl_board_interface_t;

/**
 * @brief Initialize the ESP-BSP SDL abstraction layer
 *
 * This function initializes the board-specific display and touch interfaces
 * based on the selected board configuration.
 *
 * @param[out] config Display configuration structure to be filled
 * @param[out] panel_handle LCD panel handle
 * @param[out] panel_io_handle LCD panel IO handle
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp_bsp_sdl_init(esp_bsp_sdl_display_config_t *config,
                           esp_lcd_panel_handle_t *panel_handle,
                           esp_lcd_panel_io_handle_t *panel_io_handle);

/**
 * @brief Turn on display backlight
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp_bsp_sdl_backlight_on(void);

/**
 * @brief Turn off display backlight
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp_bsp_sdl_backlight_off(void);

/**
 * @brief Enable/disable display
 *
 * @param enable true to enable display, false to disable
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp_bsp_sdl_display_on_off(bool enable);

/**
 * @brief Initialize touch interface (if available)
 *
 * @return ESP_OK on success, ESP_ERR_NOT_SUPPORTED if no touch, error code otherwise
 */
esp_err_t esp_bsp_sdl_touch_init(void);

/**
 * @brief Read touch information
 *
 * @param[out] touch_info Touch information structure to be filled
 * @return ESP_OK on success, ESP_ERR_NOT_SUPPORTED if no touch, error code otherwise
 */
esp_err_t esp_bsp_sdl_touch_read(esp_bsp_sdl_touch_info_t *touch_info);

/**
 * @brief Get the selected board name (for debugging/logging)
 *
 * @return Board name string
 */
const char *esp_bsp_sdl_get_board_name(void);

/**
 * @brief Deinitialize the ESP-BSP SDL abstraction layer
 *
 * @return ESP_OK on success, error code otherwise
 */
esp_err_t esp_bsp_sdl_deinit(void);

#ifdef __cplusplus
}
#endif
