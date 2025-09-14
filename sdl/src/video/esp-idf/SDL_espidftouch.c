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
        ESP_LOGI("SDL", "ESPIDF_InitTouch - Touch support enabled");
    } else if (ret == ESP_ERR_NOT_SUPPORTED) {
        ESP_LOGI("SDL", "ESPIDF_InitTouch - Touch not supported on this board");
    } else {
        ESP_LOGE("SDL", "ESPIDF_InitTouch - Touch initialization failed: %s", esp_err_to_name(ret));
    }
}

void ESPIDF_PumpTouchEvent(void)
{
    if (!display_config.has_touch) {
        return;
    }
    
    SDL_Window *window;
    SDL_VideoDisplay *display;
    static bool was_pressed = false;
    esp_bsp_sdl_touch_info_t touch_info;

    esp_err_t ret = esp_bsp_sdl_touch_read(&touch_info);
    if (ret != ESP_OK) {
        return;
    }

    display = NULL;
    window = display ? display->fullscreen_window : NULL;

    if (touch_info.pressed != was_pressed) {
        was_pressed = touch_info.pressed;
        ESP_LOGD("SDL", "touch state: %d, [%d, %d]", touch_info.pressed, touch_info.x, touch_info.y);
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
