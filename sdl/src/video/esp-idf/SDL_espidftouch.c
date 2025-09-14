#include "events/SDL_touch_c.h"
#include "video/SDL_sysvideo.h"
#include "SDL_espidftouch.h"
#include <stdbool.h>

#include "SDL_espidfshared.h"
#include "esp_log.h"

#define ESPIDF_TOUCH_ID         1
#define ESPIDF_TOUCH_FINGER     1

void ESPIDF_InitTouch(void)
{
    esp_err_t ret = esp_bsp_sdl_touch_init();
    if (ret == ESP_OK) {
        SDL_AddTouch(ESPIDF_TOUCH_ID, SDL_TOUCH_DEVICE_DIRECT, "Touchscreen");
        ESP_LOGI("SDL", "ESPIDF_InitTouch: Touch initialized successfully");
    } else if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGI("SDL", "ESPIDF_InitTouch: Touch not supported on this board");
    } else {
        ESP_LOGE("SDL", "ESPIDF_InitTouch: Failed to initialize touch: %s", esp_err_to_name(ret));
    }
}

void ESPIDF_PumpTouchEvent(void)
{
    esp_bsp_sdl_touch_info_t touch_info;
    static bool was_pressed = false;
    
    esp_err_t ret = esp_bsp_sdl_touch_read(&touch_info);
    if (ret == ESP_ERR_NOT_SUPPORTED) {
        // Touch not supported, nothing to do
        return;
    } else if (ret != ESP_OK) {
        // Error reading touch, skip this cycle
        ESP_LOGD("SDL", "Touch read error: %s", esp_err_to_name(ret));
        return;
    }

    SDL_Window *window = NULL;
    SDL_VideoDisplay *display = NULL;
    window = display ? display->fullscreen_window : NULL;

    if (touch_info.pressed != was_pressed) {
        was_pressed = touch_info.pressed;
        ESP_LOGD("SDL", "Touch %s: [%d, %d]", touch_info.pressed ? "pressed" : "released", touch_info.x, touch_info.y);
        SDL_SendTouch(0, ESPIDF_TOUCH_ID, ESPIDF_TOUCH_FINGER,
                      window,
                      touch_info.pressed,
                      touch_info.x,
                      touch_info.y,
                      touch_info.pressed ? 1.0f : 0.0f);
    } else if (touch_info.pressed) {
        SDL_SendTouchMotion(0, ESPIDF_TOUCH_ID, ESPIDF_TOUCH_FINGER,
                            window,
                            touch_info.x,
                            touch_info.y,
                            1.0f);
    }
}

int ESPIDF_CalibrateTouch(float screenX[], float screenY[], float touchX[], float touchY[])
{
    return 0;
}

void ESPIDF_ChangeTouchMode(int raw)
{
    return;
}

void ESPIDF_ReadTouchRawPosition(float* x, float* y)
{
    return;
}

void ESPIDF_QuitTouch(void)
{
    // ts_close(ts);
}
