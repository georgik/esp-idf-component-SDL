#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Mandelbrot Fractal Visualization for SDL3 ESP-IDF
// Universal board support with adaptive resolution and beautiful color gradients

// Screen dimensions will be determined at runtime from SDL display
static int SCREEN_WIDTH = 320;  // Default fallback
static int SCREEN_HEIGHT = 240; // Default fallback

// Mandelbrot configuration
#define MAX_ITERATIONS 128
#define ZOOM_SPEED 0.98f
#define ANIMATION_SPEED 0.005f

// Fractal parameters
typedef struct {
    double center_x, center_y;
    double zoom;
    double color_offset;
    int frame_count;
} FractalState;

static FractalState fractal_state = {
    .center_x = -0.7,
    .center_y = 0.0,
    .zoom = 1.0,
    .color_offset = 0.0,
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

// Generate beautiful Mandelbrot colors based on iteration count
Color mandelbrot_color(int iterations, double color_offset) {
    if (iterations == MAX_ITERATIONS) {
        return (Color){0, 0, 0, 255}; // Black for points in the set
    }
    
    // Create smooth coloring with multiple color schemes
    float t = (float)iterations / MAX_ITERATIONS;
    
    // Cycle through different color schemes
    int scheme = (int)(color_offset * 4) % 4;
    
    switch (scheme) {
        case 0: {
            // Fire gradient: red -> orange -> yellow
            float hue = 60.0f * (1.0f - t); // Yellow to red
            float saturation = 1.0f;
            float value = t * 0.8f + 0.2f;
            return hsv_to_rgb(hue, saturation, value);
        }
        case 1: {
            // Ocean gradient: blue -> cyan -> white
            float hue = 240.0f - 60.0f * t; // Blue to cyan
            float saturation = 1.0f - t * 0.3f;
            float value = 0.6f + t * 0.4f;
            return hsv_to_rgb(hue, saturation, value);
        }
        case 2: {
            // Sunset gradient: purple -> pink -> orange
            float hue = 270.0f + 60.0f * t; // Purple to orange
            if (hue > 360.0f) hue -= 360.0f;
            float saturation = 0.8f + 0.2f * sinf(t * M_PI);
            float value = 0.5f + t * 0.5f;
            return hsv_to_rgb(hue, saturation, value);
        }
        default: {
            // Psychedelic: rapid color cycling
            float hue = fmodf(360.0f * t * 3 + color_offset * 720.0f, 360.0f);
            float saturation = 0.8f + 0.2f * sinf(t * M_PI * 4);
            float value = 0.4f + 0.6f * t;
            return hsv_to_rgb(hue, saturation, value);
        }
    }
}

// Calculate Mandelbrot iterations for a given complex number
int mandelbrot_iterations(double cx, double cy) {
    double x = 0.0, y = 0.0;
    int iterations = 0;
    
    while (iterations < MAX_ITERATIONS && (x*x + y*y) < 4.0) {
        double temp = x*x - y*y + cx;
        y = 2*x*y + cy;
        x = temp;
        iterations++;
    }
    
    return iterations;
}

// Render a strip of the Mandelbrot set (for progressive rendering)
void render_mandelbrot_strip(SDL_Renderer *renderer, int start_y, int end_y, FractalState *state) {
    double scale = 4.0 / (state->zoom * SCREEN_WIDTH);
    
    for (int py = start_y; py < end_y; py++) {
        for (int px = 0; px < SCREEN_WIDTH; px++) {
            // Convert screen coordinates to complex plane
            double cx = state->center_x + (px - SCREEN_WIDTH/2) * scale;
            double cy = state->center_y + (py - SCREEN_HEIGHT/2) * scale;
            
            // Calculate Mandelbrot iterations
            int iterations = mandelbrot_iterations(cx, cy);
            
            // Generate color
            Color color = mandelbrot_color(iterations, state->color_offset);
            
            // Draw pixel
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            SDL_RenderPoint(renderer, px, py);
        }
    }
}

// Update fractal animation parameters
void update_fractal_animation(FractalState *state) {
    state->frame_count++;
    
    // Animate color cycling
    state->color_offset += ANIMATION_SPEED;
    if (state->color_offset > 1.0) state->color_offset -= 1.0;
    
    // Zoom in slowly with periodic resets
    if (state->frame_count % 600 == 0) {
        // Reset to a new interesting location
        const double interesting_points[][2] = {
            {-0.7, 0.0},           // Main bulb
            {-0.8, 0.156},         // Spiral region
            {-0.16, 1.04},         // Top detail
            {0.3, 0.5},            // Right side detail
            {-1.25, 0.02},         // Left spike detail
            {-0.235125, 0.827215}, // Feather detail
            {-0.4, 0.6},           // Upper bulb connection
        };
        
        int location = (state->frame_count / 600) % 7;
        state->center_x = interesting_points[location][0];
        state->center_y = interesting_points[location][1];
        state->zoom = 1.0;
    } else if (state->frame_count % 100 == 0) {
        // Zoom in
        state->zoom /= ZOOM_SPEED;
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
        printf("Using actual display resolution: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
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
    printf("Features: Adaptive resolution, color animation, auto-zoom\n");

    SDL_Event event;
    int running = 1;
    int render_strips = 4; // Progressive rendering for better responsiveness
    int current_strip = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        // Update animation parameters
        update_fractal_animation(&fractal_state);

        // Progressive rendering: render one strip per frame
        int strip_height = SCREEN_HEIGHT / render_strips;
        int start_y = current_strip * strip_height;
        int end_y = (current_strip == render_strips - 1) ? SCREEN_HEIGHT : (current_strip + 1) * strip_height;

        render_mandelbrot_strip(renderer, start_y, end_y, &fractal_state);

        // Move to next strip
        current_strip = (current_strip + 1) % render_strips;

        // If we completed a full frame, draw overlay and present
        if (current_strip == 0) {
            draw_info_overlay(renderer, &fractal_state);
            SDL_RenderPresent(renderer);
            
            // Print progress periodically
            if (fractal_state.frame_count % 60 == 0) {
                printf("Frame %d, Zoom: %.2f, Location: (%.6f, %.6f)\n", 
                       fractal_state.frame_count, fractal_state.zoom,
                       fractal_state.center_x, fractal_state.center_y);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(16)); // ~60 FPS target (progressive rendering)
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
    pthread_attr_setstacksize(&attr, 32768);  // Standard stack size

    int ret = pthread_create(&sdl_pthread, &attr, sdl_thread, NULL);
    if (ret != 0) {
        printf("Failed to create SDL thread: %d\n", ret);
        return;
    }

    pthread_detach(sdl_pthread);
}