
#include "SDL_internal.h"
#include "SDL_espidfvideo.h"
#include "SDL_espidfshared.h"
#include "SDL_espidfframebuffer.h"
#include "SDL_espidfevents.h"
#include "SDL_espidftouch.h"

// BSP includes are handled by the abstraction layer (SDL_espidfshared.h)
#include "esp_log.h"

#ifdef SDL_VIDEO_DRIVER_PRIVATE

#include "video/SDL_sysvideo.h"
#include "video/SDL_pixels_c.h"
#include "events/SDL_events_c.h"

#define ESPIDFVID_DRIVER_NAME "espidf"

esp_lcd_panel_handle_t panel_handle = NULL;
esp_lcd_panel_io_handle_t panel_io_handle = NULL;

static bool ESPIDF_VideoInit(SDL_VideoDevice *_this);
static void ESPIDF_VideoQuit(SDL_VideoDevice *_this);

static bool ESPIDF_SetWindowPosition(SDL_VideoDevice *_this, SDL_Window *window)
{
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_MOVED, window->floating.x, window->floating.y);
    return true;
}

static void ESPIDF_SetWindowSize(SDL_VideoDevice *_this, SDL_Window *window)
{
    SDL_SendWindowEvent(window, SDL_EVENT_WINDOW_RESIZED, window->floating.w, window->floating.h);
}

static SDL_VideoDevice *ESPIDF_CreateDevice(void)
{
    SDL_VideoDevice *device = (SDL_VideoDevice *)SDL_calloc(1, sizeof(SDL_VideoDevice));
    if (!device) return NULL;

    device->VideoInit = ESPIDF_VideoInit;
    device->VideoQuit = ESPIDF_VideoQuit;
    device->SetWindowPosition = ESPIDF_SetWindowPosition;
    device->SetWindowSize = ESPIDF_SetWindowSize;
    device->PumpEvents = ESPIDF_PumpEvents;
    device->CreateWindowFramebuffer = SDL_ESPIDF_CreateWindowFramebuffer;
    device->UpdateWindowFramebuffer = SDL_ESPIDF_UpdateWindowFramebuffer;
    device->DestroyWindowFramebuffer = SDL_ESPIDF_DestroyWindowFramebuffer;

    return device;
}

VideoBootStrap PRIVATE_bootstrap = {
    ESPIDFVID_DRIVER_NAME, "SDL esp-idf video driver",
    ESPIDF_CreateDevice, NULL
};

static bool ESPIDF_VideoInit(SDL_VideoDevice *_this)
{
    esp_bsp_sdl_display_config_t display_config;
    
    // Initialize the abstraction layer
    ESP_ERROR_CHECK(esp_bsp_sdl_init(&display_config, &panel_handle, &panel_io_handle));
    
    // Set up SDL display mode using abstraction layer data
    SDL_DisplayMode mode;
    SDL_zero(mode);
    mode.format = display_config.pixel_format;
    mode.w = display_config.width;
    mode.h = display_config.height;
    printf("ESP-IDF video init: %dx%d\n", mode.w, mode.h);
    
    if (SDL_AddBasicVideoDisplay(&mode) == 0) {
        return false;
    }

    // Turn on backlight
    ESP_ERROR_CHECK(esp_bsp_sdl_backlight_on());

    // Enable display
    ESP_ERROR_CHECK(esp_bsp_sdl_display_on_off(true));

    // Initialize touch if available
    if (display_config.has_touch) {
        ESPIDF_InitTouch();
    }
    
    return true;
}

static void ESPIDF_VideoQuit(SDL_VideoDevice *_this)
{
    // Clean up BSP resources if needed
}

#endif /* SDL_VIDEO_DRIVER_PRIVATE */
