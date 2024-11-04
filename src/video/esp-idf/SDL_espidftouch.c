#include "events/SDL_touch_c.h"
#include "video/SDL_sysvideo.h"
#include "SDL_espidftouch.h"
#include <stdbool.h>

#include "bsp/esp-bsp.h"
#include "bsp/display.h"
#if BSP_CAPS_TOUCH == 1
#include "bsp/touch.h"
#endif
#include "esp_log.h"

#define ESPIDF_TOUCH_ID         1
#define ESPIDF_TOUCH_FINGER     1

esp_lcd_touch_handle_t touch_handle;   // LCD touch handle

void ESPIDF_InitTouch(void)
{
#if BSP_CAPS_TOUCH == 1
    bsp_i2c_init();

    /* Initialize touch */
    bsp_touch_new(NULL, &touch_handle);

    SDL_AddTouch(ESPIDF_TOUCH_ID, SDL_TOUCH_DEVICE_DIRECT, "Touchscreen");
    ESP_LOGI("SDL", "ESPIDF_InitTouch");
#endif
}

void ESPIDF_PumpTouchEvent(void)
{
#if BSP_CAPS_TOUCH == 1
    SDL_Window *window;
    SDL_VideoDisplay *display;
    static bool was_pressed = false;
    bool pressed;

    uint16_t touchpad_x[1] = {0};
    uint16_t touchpad_y[1] = {0};
    uint8_t touchpad_cnt = 0;

    esp_lcd_touch_read_data(touch_handle);
    bool touchpad_pressed = esp_lcd_touch_get_coordinates(touch_handle, touchpad_x, touchpad_y, NULL, &touchpad_cnt, 1);
    pressed = (touchpad_x[0] != 0 || touchpad_y[0] != 0);

    display = NULL;
    window = display ? display->fullscreen_window : NULL;

    if (pressed != was_pressed) {
        was_pressed = pressed;
        ESP_LOGD("SDL", "touchpad_pressed: %d, [%d, %d]", touchpad_pressed, touchpad_x[0], touchpad_y[0]);
        SDL_SendTouch(0, ESPIDF_TOUCH_ID, ESPIDF_TOUCH_FINGER,
                      window,
                      pressed,
                      touchpad_x[0],
                      touchpad_y[0],
                      pressed ? 1.0f : 0.0f);
    } else if (pressed) {
        SDL_SendTouchMotion(0, ESPIDF_TOUCH_ID, ESPIDF_TOUCH_FINGER,
                            window,
                            touchpad_x[0],
                            touchpad_y[0],
                            1.0f);
    }
#endif
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
