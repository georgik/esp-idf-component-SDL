#include "SDL_internal.h"

#ifdef SDL_VIDEO_DRIVER_PRIVATE

#include "video/SDL_sysvideo.h"
#include "SDL_properties_c.h"
#include "SDL_espidfframebuffer.h"
#include "esp_err.h"
#include "esp_check.h"
#include "esp_lcd_panel_ops.h"
#include "SDL_espidfshared.h"
#include "esp_heap_caps.h"
#include "freertos/semphr.h"
#ifdef CONFIG_IDF_TARGET_ESP32P4
#include "driver/ppa.h"
#include "esp_lcd_mipi_dsi.h"
#endif

static const char *TAG = "SDL_espidfframebuffer";

#define ESPIDF_SURFACE "SDL.internal.window.surface"

static SemaphoreHandle_t lcd_semaphore;
static int max_chunk_height = 4;  // Configurable chunk height
#ifdef CONFIG_IDF_TARGET_ESP32P4
static ppa_client_handle_t ppa_srm_handle = NULL;  // PPA client handle
static uint8_t *ppa_out_buf = NULL;  // Reusable PPA output buffer
static size_t ppa_out_buf_size = 0;  // Size of the PPA output buffer
static int rotation_angle = 0;  // Rotation angle (0, 90, 180, 270)
static bool rotation_enabled = false;
static int scale_factor = 1;
static bool scaling_enabled = false;

// Function to set rotation angle (called from application)
void set_display_rotation(int angle) {
    rotation_angle = angle;
    rotation_enabled = (angle != 0);
    ESP_LOGI(TAG, "Display rotation set to %d degrees", angle);
}

void set_display_scale_factor(int factor) {
    scale_factor = factor;
    ESP_LOGI(TAG, "Display scale factor set to %dx", factor);
}

// Function to set scale factor with aspect ratio preservation
void set_display_scaling(int factor) {
    scale_factor = factor;
    scaling_enabled = (factor > 1);
    ESP_LOGI(TAG, "Display scaling set to %dx", factor);
}

#else
static uint16_t *rgb565_buffer = NULL;
#endif

#ifdef CONFIG_IDF_TARGET_ESP32P4
static bool lcd_event_callback(esp_lcd_panel_handle_t panel_io, esp_lcd_dpi_panel_event_data_t *edata, void *user_ctx)
{
    xSemaphoreGive(lcd_semaphore);
    return false;
}
#else
static bool lcd_event_callback(esp_lcd_panel_io_handle_t io, esp_lcd_panel_io_event_data_t *event_data, void *user_ctx)
{
    xSemaphoreGive(lcd_semaphore);
    return false;
}
#endif

void esp_idf_log_free_dma(void) {
    size_t free_dma = heap_caps_get_free_size(MALLOC_CAP_DMA);
    ESP_LOGI(TAG, "Free DMA memory: %d bytes", free_dma);
}

bool SDL_ESPIDF_CreateWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window, SDL_PixelFormat *format, void **pixels, int *pitch)
{
    SDL_Surface *surface;
    int w, h;

    SDL_GetWindowSizeInPixels(window, &w, &h);
    // Create framebuffer surface matching the window size - let application handle game resolution scaling
    surface = SDL_CreateSurface(w, h, SDL_PIXELFORMAT_RGB565);
    if (!surface) {
        return false;
    }

    SDL_SetSurfaceProperty(SDL_GetWindowProperties(window), ESPIDF_SURFACE, surface);
    *format = SDL_PIXELFORMAT_RGB565;
    *pixels = surface->pixels;
    *pitch = surface->pitch;

#ifndef CONFIG_IDF_TARGET_ESP32P4
    // Allocate RGB565 buffer in IRAM
    rgb565_buffer = heap_caps_malloc(w * max_chunk_height * sizeof(uint16_t), MALLOC_CAP_32BIT | MALLOC_CAP_INTERNAL);
    if (!rgb565_buffer) {
        SDL_DestroySurface(surface);
        return SDL_SetError("Failed to allocate memory for RGB565 buffer");
    }
#endif

    // Create a semaphore to synchronize LCD transactions
    lcd_semaphore = xSemaphoreCreateBinary();
    if (!lcd_semaphore) {
        SDL_DestroySurface(surface);
        return SDL_SetError("Failed to create semaphore");
    }

    // Initialize PPA for rotation (ESP32-P4 only)
#ifdef CONFIG_IDF_TARGET_ESP32P4
    if (!ppa_srm_handle) {
        ppa_client_config_t ppa_srm_config = {
            .oper_type = PPA_OPERATION_SRM,
            .max_pending_trans_num = 1,
        };
        ESP_ERROR_CHECK(ppa_register_client(&ppa_srm_config, &ppa_srm_handle));
    }

    // Allocate PPA output buffer for rotation (PPA will handle scaling)
    if (rotation_enabled && !ppa_out_buf) {
        // Use original game resolution for input, PPA will scale during processing
        // After 90/270° rotation: 320x200 becomes 200x320, then scaled by scale_factor
        int game_width = 320;
        int game_height = 200;
        int rotated_width = (rotation_angle == 90 || rotation_angle == 270) ? game_height : game_width;
        int rotated_height = (rotation_angle == 90 || rotation_angle == 270) ? game_width : game_height;
        // Output dimensions after rotation AND scaling
        int output_width = rotated_width * scale_factor;
        int output_height = rotated_height * scale_factor;
        
        // PPA requires proper buffer alignment - align to 64 bytes for safety
        size_t raw_size = output_width * output_height * sizeof(uint16_t);
        ppa_out_buf_size = (raw_size + 63) & ~63;  // Align to 64 bytes
        
        // Allocate DMA-capable buffer with proper alignment
        ppa_out_buf = heap_caps_aligned_alloc(64, ppa_out_buf_size, MALLOC_CAP_DMA | MALLOC_CAP_SPIRAM);
        if (!ppa_out_buf) {
            return SDL_SetError("Failed to allocate aligned PPA output buffer for rotation");
        }
        ESP_LOGI(TAG, "PPA rotation buffer allocated: %dx%d (%zu bytes raw, %zu aligned, scaled game resolution %dx)", 
                 output_width, output_height, raw_size, ppa_out_buf_size, scale_factor);
    }

    const esp_lcd_dpi_panel_event_callbacks_t callback = {
        .on_color_trans_done = lcd_event_callback,
    };
    esp_lcd_dpi_panel_register_event_callbacks(panel_handle, &callback, NULL);
#else
    esp_lcd_panel_io_register_event_callbacks(panel_io_handle, &(esp_lcd_panel_io_callbacks_t){ .on_color_trans_done = lcd_event_callback }, NULL);
#endif

    return true;
}

IRAM_ATTR bool SDL_ESPIDF_UpdateWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window, const SDL_Rect *rects, int numrects)
{
    SDL_Surface *surface = (SDL_Surface *)SDL_GetPointerProperty(SDL_GetWindowProperties(window), ESPIDF_SURFACE, NULL);
    if (!surface) {
        return SDL_SetError("Couldn't find ESPIDF surface for window");
    }

#ifdef CONFIG_IDF_TARGET_ESP32P4
    // ESP32-P4 with optional PPA rotation and scaling
    if (rotation_enabled && ppa_out_buf) {
        // Game content dimensions (original, PPA will scale)
        int game_width = 320;
        int game_height = 200;
        
        // Use PPA to rotate only the game content (320x200) from the larger framebuffer
        ppa_srm_rotation_angle_t ppa_rotation;
        switch (rotation_angle) {
            case 90:  ppa_rotation = PPA_SRM_ROTATION_ANGLE_90; break;
            case 180: ppa_rotation = PPA_SRM_ROTATION_ANGLE_180; break;
            case 270: ppa_rotation = PPA_SRM_ROTATION_ANGLE_270; break;
            default:  ppa_rotation = PPA_SRM_ROTATION_ANGLE_0; break;
        }

        // Calculate output dimensions after rotation and scaling
        int rotated_width = (rotation_angle == 90 || rotation_angle == 270) ? game_height : game_width;
        int rotated_height = (rotation_angle == 90 || rotation_angle == 270) ? game_width : game_height;
        int out_width = rotated_width * scale_factor;
        int out_height = rotated_height * scale_factor;
        
        // Center the rotated game content on display
        // For M5Stack Tab5: 720x1280 display, rotated 320x200 becomes 200x320
        int display_width = 720;
        int display_height = 1280;
        
        int offset_x = (display_width - out_width) / 2;
        int offset_y = (display_height - out_height) / 2;
        
        // Ensure we don't go negative
        offset_x = (offset_x < 0) ? 0 : offset_x;
        offset_y = (offset_y < 0) ? 0 : offset_y;

        ppa_srm_oper_config_t srm_config = {
            .in.buffer = surface->pixels,  // Start of framebuffer
            .in.pic_w = surface->w,        // Full framebuffer width for pitch calculation
            .in.pic_h = surface->h,        // Full framebuffer height
            .in.block_w = game_width,      // Only process game content width (320)
            .in.block_h = game_height,     // Only process game content height (200)
            .in.block_offset_x = 0,        // Game content starts at top-left
            .in.block_offset_y = 0,
            .in.srm_cm = PPA_SRM_COLOR_MODE_RGB565,

            .out.buffer = ppa_out_buf,
            .out.buffer_size = ppa_out_buf_size,
            .out.pic_w = out_width,        // Rotated dimensions
            .out.pic_h = out_height,
            .out.block_offset_x = 0,
            .out.block_offset_y = 0,
            .out.srm_cm = PPA_SRM_COLOR_MODE_RGB565,

            .rotation_angle = ppa_rotation,
            .scale_x = scale_factor,  // Hardware scaling during rotation
            .scale_y = scale_factor,
            .mirror_x = 0,
            .mirror_y = 0,
            .rgb_swap = 0,
            .byte_swap = 0,
            .mode = PPA_TRANS_MODE_BLOCKING,
        };

        // Execute PPA rotation for game content only
        esp_err_t result = ppa_do_scale_rotate_mirror(ppa_srm_handle, &srm_config);
        if (result != ESP_OK) {
            ESP_LOGE(TAG, "PPA rotation failed: %s", esp_err_to_name(result));
            // Fall back to non-rotated chunked rendering
            goto fallback_rendering;
        }

        // Draw the rotated game content centered on display (display is cleared by having black around content)
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, offset_x, offset_y, 
                                                 offset_x + out_width, offset_y + out_height, ppa_out_buf));
        
        // Wait for transmission to complete
        xSemaphoreTake(lcd_semaphore, portMAX_DELAY);
    } else {
    fallback_rendering:
        // Fallback: chunked output without rotation
        for (int y = 0; y < surface->h; y += max_chunk_height) {
            int height = (y + max_chunk_height > surface->h) ? (surface->h - y) : max_chunk_height;
            uint16_t *src_pixels = (uint16_t *)surface->pixels + (y * surface->w);
            
            // Direct output without rotation
            ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, y, surface->w, y + height, src_pixels));
            
            // Wait for the current chunk to finish transmission
            xSemaphoreTake(lcd_semaphore, portMAX_DELAY);
        }
    }
#else
    // Without PPA, send chunks directly from src_pixels
    for (int y = 0; y < surface->h; y += max_chunk_height) {
        int height = (y + max_chunk_height > surface->h) ? (surface->h - y) : max_chunk_height;

        for (int i = 0; i < surface->w * max_chunk_height; i++) {
            uint16_t rgba = ((uint16_t *)surface->pixels)[y * surface->w + i];
            uint8_t g = (rgba >> 11) & 0xFF;
            uint8_t r = (rgba >> 5) & 0xFF;
            uint8_t b = (rgba >> 0) & 0xFF;

            rgb565_buffer[i] = ((b & 0xF8) << 8) | ((g & 0xFC) << 3) | (r >> 3);
        }
        // Send directly to LCD
        ESP_ERROR_CHECK(esp_lcd_panel_draw_bitmap(panel_handle, 0, y, surface->w, y + height, rgb565_buffer));

        // Wait for the current chunk to finish transmission
        xSemaphoreTake(lcd_semaphore, portMAX_DELAY);
    }
#endif

    return true;
}

void SDL_ESPIDF_DestroyWindowFramebuffer(SDL_VideoDevice *_this, SDL_Window *window)
{
    SDL_ClearProperty(SDL_GetWindowProperties(window), ESPIDF_SURFACE);

    // Delete the semaphore
    if (lcd_semaphore) {
        vSemaphoreDelete(lcd_semaphore);
        lcd_semaphore = NULL;
    }

#ifdef CONFIG_IDF_TARGET_ESP32P4
    // Free PPA output buffer
    if (ppa_out_buf) {
        heap_caps_free(ppa_out_buf);
        ppa_out_buf = NULL;
        ppa_out_buf_size = 0;
    }

    if (ppa_srm_handle) {
        ESP_ERROR_CHECK(ppa_unregister_client(ppa_srm_handle));
        ppa_srm_handle = NULL;
    }
#else
    // Free the RGB565 buffer
    if (rgb565_buffer) {
        heap_caps_free(rgb565_buffer);
        rgb565_buffer = NULL;
    }
#endif
}

#endif /* SDL_VIDEO_DRIVER_PRIVATE */
