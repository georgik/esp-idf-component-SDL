#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <inttypes.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Mandelbrot Fractal Visualization for SDL3 ESP-IDF
// Universal board support with adaptive resolution and beautiful color gradients

// Screen dimensions will be determined at runtime from SDL display
static int SCREEN_WIDTH = 320;  // Default fallback
static int SCREEN_HEIGHT = 240; // Default fallback

// Mandelbrot configuration optimized for ESP32-S3
#define MAX_ITERATIONS 32  // Further reduced for better performance on embedded
#define FIXED_SHIFT 12     // Fixed point arithmetic: 20.12 format
#define FIXED_ONE (1 << FIXED_SHIFT)  // 1.0 in fixed point
#define FIXED_FOUR (4 << FIXED_SHIFT) // 4.0 in fixed point

// Convert float to fixed point
#define FLOAT_TO_FIXED(x) ((int32_t)((x) * FIXED_ONE))
// Convert fixed point to integer
#define FIXED_TO_INT(x) ((x) >> FIXED_SHIFT)
// Fixed point multiplication
#define FIXED_MUL(a, b) (((int64_t)(a) * (b)) >> FIXED_SHIFT)

// Fractal parameters using fixed point arithmetic
typedef struct {
    int32_t center_x, center_y;  // Fixed point
    int32_t zoom;                // Fixed point
    int32_t color_offset;        // Fixed point
    int frame_count;
} FractalState;

// Single-threaded incremental rendering (SDL is not thread-safe)

static FractalState fractal_state = {
    .center_x = FLOAT_TO_FIXED(-0.7),  // -0.7 in fixed point
    .center_y = FLOAT_TO_FIXED(0.0),   // 0.0 in fixed point
    .zoom = FIXED_ONE,                 // 1.0 in fixed point
    .color_offset = 0,
    .frame_count = 0
};

// HSV to RGB conversion for beautiful color gradients
typedef struct {
    uint8_t r, g, b, a;
} Color;

Color hsv_to_rgb(float h, float s, float v) {
    float c = v * s;
    float x = c * (1.0f - fabsf(fmodf(h / 60.0f, 2.0f) - 1.0f));
    float m = v - c;
    
    float r_prime, g_prime, b_prime;
    
    if (h >= 0 && h < 60) {
        r_prime = c; g_prime = x; b_prime = 0;
    } else if (h >= 60 && h < 120) {
        r_prime = x; g_prime = c; b_prime = 0;
    } else if (h >= 120 && h < 180) {
        r_prime = 0; g_prime = c; b_prime = x;
    } else if (h >= 180 && h < 240) {
        r_prime = 0; g_prime = x; b_prime = c;
    } else if (h >= 240 && h < 300) {
        r_prime = x; g_prime = 0; b_prime = c;
    } else {
        r_prime = c; g_prime = 0; b_prime = x;
    }
    
    Color result = {
        (uint8_t)((r_prime + m) * 255),
        (uint8_t)((g_prime + m) * 255),
        (uint8_t)((b_prime + m) * 255),
        255
    };
    return result;
}

// Generate simple Mandelbrot colors - optimized for ESP32-S3
Color mandelbrot_color(int iterations, int color_offset) {
    if (iterations == MAX_ITERATIONS) {
        return (Color){0, 0, 0, 255}; // Black for points in the set
    }
    
    // Simple color scheme that's very fast
    uint8_t color_val = (iterations * 255) / MAX_ITERATIONS;
    
    // Cycle through simple color schemes
    int scheme = (color_offset / 32) % 4;  // Change scheme every 32 frames
    
    switch (scheme) {
        case 0: // Red to white gradient
            return (Color){255, color_val, color_val, 255};
        case 1: // Green to white gradient
            return (Color){color_val, 255, color_val, 255};
        case 2: // Blue to white gradient
            return (Color){color_val, color_val, 255, 255};
        default: // Grayscale
            return (Color){color_val, color_val, color_val, 255};
    }
}

// Calculate Mandelbrot iterations using fixed point arithmetic - much faster on ESP32-S3
int mandelbrot_iterations(int32_t cx, int32_t cy) {
    int32_t x = 0, y = 0;
    int iterations = 0;
    
    while (iterations < MAX_ITERATIONS) {
        // Calculate x*x and y*y in fixed point
        int32_t x_squared = FIXED_MUL(x, x);
        int32_t y_squared = FIXED_MUL(y, y);
        
        // Check if magnitude squared >= 4.0
        if ((x_squared + y_squared) >= FIXED_FOUR) {
            break;
        }
        
        // Calculate next iteration: z = z^2 + c
        int32_t temp = x_squared - y_squared + cx;
        y = FIXED_MUL(2 * x, y) + cy;
        x = temp;
        iterations++;
    }
    
    return iterations;
}

// Render a strip of the Mandelbrot set using fixed point arithmetic
void render_mandelbrot_strip(SDL_Renderer *renderer, int start_y, int end_y, FractalState *state) {
    // Calculate scale factor in fixed point
    // scale = 4.0 / (zoom * SCREEN_WIDTH)
    // Prevent division by zero
    int32_t zoom_width = FIXED_MUL(state->zoom, SCREEN_WIDTH * FIXED_ONE);
    if (zoom_width == 0) zoom_width = 1;
    
    int32_t scale = (FIXED_FOUR * FIXED_ONE) / zoom_width;
    
    // Precompute half screen dimensions in fixed point
    int32_t half_width = (SCREEN_WIDTH * FIXED_ONE) / 2;
    int32_t half_height = (SCREEN_HEIGHT * FIXED_ONE) / 2;
    
    for (int py = start_y; py < end_y; py++) {
        for (int px = 0; px < SCREEN_WIDTH; px++) {
            // Convert screen coordinates to complex plane using fixed point
            int32_t cx = state->center_x + FIXED_MUL((px * FIXED_ONE - half_width), scale);
            int32_t cy = state->center_y + FIXED_MUL((py * FIXED_ONE - half_height), scale);
            
            // Calculate Mandelbrot iterations
            int iterations = mandelbrot_iterations(cx, cy);
            
            // Generate color
            Color color = mandelbrot_color(iterations, FIXED_TO_INT(state->color_offset));
            
            // Draw pixel
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderPoint(renderer, px, py);
        }
        
        // Yield to other tasks every 2 rows to prevent watchdog timeout
        if ((py - start_y) % 2 == 1) {
            taskYIELD();
        }
    }
}


// Update fractal animation parameters using fixed point
void update_fractal_animation(FractalState *state) {
    // Animate color cycling (simple integer increment)
    state->color_offset += 1;
    if (state->color_offset > 255) state->color_offset = 0;
    
    // Zoom in slowly with periodic resets
    if (state->frame_count % 300 == 0) {  // Reset every 300 frames (5 sec at 60fps)
        // Reset to a new interesting location using fixed point
        const int32_t interesting_points[][2] = {
            {FLOAT_TO_FIXED(-0.7), FLOAT_TO_FIXED(0.0)},      // Main bulb
            {FLOAT_TO_FIXED(-0.8), FLOAT_TO_FIXED(0.156)},    // Spiral region  
            {FLOAT_TO_FIXED(-0.16), FLOAT_TO_FIXED(1.04)},    // Top detail
            {FLOAT_TO_FIXED(0.3), FLOAT_TO_FIXED(0.5)},       // Right side detail
            {FLOAT_TO_FIXED(-1.25), FLOAT_TO_FIXED(0.02)},    // Left spike detail
        };
        
        int location = (state->frame_count / 300) % 5;
        state->center_x = interesting_points[location][0];
        state->center_y = interesting_points[location][1];
        state->zoom = FIXED_ONE;
        printf("\n🎯 Switching to fractal location %d (frame %d)\n", location, state->frame_count);
    } else if (state->frame_count % 60 == 0) {  // Zoom every 60 frames (1 sec)
        // Zoom in using fixed point: zoom = zoom * 1.1
        state->zoom = FIXED_MUL(state->zoom, FLOAT_TO_FIXED(1.1));
        printf("🔍 Zooming in: %" PRId32 "x (frame %d)\n", FIXED_TO_INT(state->zoom), state->frame_count);
    }
}

// Draw information overlay
void draw_info_overlay(SDL_Renderer *renderer, FractalState *state) {
    // Draw simple status indicators
    
    // Zoom level indicator (top-right corner)
    int zoom_level = (int)(log10(state->zoom) * 2);
    if (zoom_level > 20) zoom_level = 20;
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 128);
    for (int i = 0; i < zoom_level; i++) {
        SDL_FRect bar = {SCREEN_WIDTH - 30 + i, 10, 1, 10};
        SDL_RenderFillRect(renderer, &bar);
    }
    
    // Color scheme indicator (bottom-right corner)
    int scheme = (int)(state->color_offset * 4) % 4;
    SDL_SetRenderDrawColor(renderer, 
                          scheme == 0 ? 255 : 0,
                          scheme == 1 ? 255 : 0, 
                          scheme == 2 ? 255 : 0, 255);
    SDL_FRect scheme_indicator = {SCREEN_WIDTH - 20, SCREEN_HEIGHT - 20, 15, 15};
    SDL_RenderFillRect(renderer, &scheme_indicator);
    
    // Frame counter dots (bottom-left corner)
    int frame_indicator = (state->frame_count / 10) % 16;
    SDL_SetRenderDrawColor(renderer, 200, 200, 200, 255);
    for (int i = 0; i < frame_indicator; i++) {
        SDL_FRect dot = {5 + i * 3, SCREEN_HEIGHT - 15, 2, 2};
        SDL_RenderFillRect(renderer, &dot);
    }
}

void* sdl_thread(void* args) {
    printf("Mandelbrot Fractal Visualization - SDL3 on ESP32\n");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }
    printf("SDL initialized successfully\n");

    // Query the actual display dimensions from SDL
    const SDL_DisplayMode *display_mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    if (display_mode) {
        SCREEN_WIDTH = display_mode->w;
        SCREEN_HEIGHT = display_mode->h;
        printf("SDL Display mode: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
        
        // For M5Stack Tab5, convert from landscape (1280x720) to portrait (720x1280)
        // This gives better screen utilization even though orientation is flipped
        if (SCREEN_WIDTH == 1280 && SCREEN_HEIGHT == 720) {
            printf("Converting from landscape to portrait orientation for better screen usage\n");
            int temp = SCREEN_WIDTH;
            SCREEN_WIDTH = SCREEN_HEIGHT;
            SCREEN_HEIGHT = temp;
            printf("Using portrait dimensions: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
        }
    } else {
        printf("Failed to get display mode, using defaults: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    SDL_Window *window = SDL_CreateWindow("Mandelbrot Fractal", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        SDL_Quit();
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return NULL;
    }

    printf("Starting Mandelbrot fractal visualization...\n");
    printf("Features: Adaptive resolution, color animation, auto-zoom, streaming progressive rendering\n");

    SDL_Event event;
    int running = 1;
    printf("Screen resolution: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    printf("Max iterations: %d\n", MAX_ITERATIONS);
    printf("Fixed point shift: %d bits\n", FIXED_SHIFT);
    printf("FIXED_ONE = %d\n", FIXED_ONE);
    
    // Progressive streaming rendering (single-threaded, SDL is not thread-safe)
    int current_strip = 0;
    const int STRIP_HEIGHT = 8; // Very small strips for responsive streaming
    int total_strips = (SCREEN_HEIGHT + STRIP_HEIGHT - 1) / STRIP_HEIGHT;
    printf("Streaming progressive rendering: %d strips of %d rows each\n", total_strips, STRIP_HEIGHT);
    
    // Initial screen clear (only once)
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderPresent(renderer);
    printf("Screen initialized. Starting continuous streaming render...\n");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        // Update animation parameters every 120 frames for stability
        if (fractal_state.frame_count % 120 == 0) {
            update_fractal_animation(&fractal_state);
        }

        // Start new frame rendering
        if (current_strip == 0) {
            fractal_state.frame_count++;
            printf("Starting frame %d...\n", fractal_state.frame_count);
        }

        // Render one strip at a time (single-threaded, safe for SDL)
        if (current_strip < total_strips) {
            int start_y = current_strip * STRIP_HEIGHT;
            int end_y = (current_strip + 1) * STRIP_HEIGHT;
            if (end_y > SCREEN_HEIGHT) end_y = SCREEN_HEIGHT;
            
            // Render the strip
            render_mandelbrot_strip(renderer, start_y, end_y, &fractal_state);
            
            // Stream the rendered strip to display immediately (progressive rendering)
            SDL_RenderPresent(renderer);
            
            current_strip++;
            
            // Show progress less frequently for smoother output
            if (current_strip % 10 == 0) {
                printf("Rendering strip %d/%d (%.1f%%)\n", current_strip, total_strips, 
                       (float)current_strip * 100.0f / total_strips);
            }
        }
        
        // If we've finished all strips, complete the frame
        if (current_strip >= total_strips) {
            // Draw overlay on top of the completed fractal
            draw_info_overlay(renderer, &fractal_state);
            SDL_RenderPresent(renderer);
            
            // Reset for next frame
            current_strip = 0;
            
            // Print frame completion info
            printf("✓ Frame %d complete, Zoom: %" PRId32 " (fixed), Location: (%" PRId32 ", %" PRId32 ") (fixed)\n", 
                   fractal_state.frame_count, FIXED_TO_INT(fractal_state.zoom),
                   FIXED_TO_INT(fractal_state.center_x), FIXED_TO_INT(fractal_state.center_y));
        }

        // Minimal delay for watchdog reset and task switching
        vTaskDelay(pdMS_TO_TICKS(1));
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return NULL;
}

void app_main(void) {
    pthread_t sdl_pthread;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 65536);  // Larger stack size for floating point calculations

    int ret = pthread_create(&sdl_pthread, &attr, sdl_thread, NULL);
    if (ret != 0) {
        printf("Failed to create SDL thread: %d\n", ret);
        return;
    }

    pthread_detach(sdl_pthread);
}