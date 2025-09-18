#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_heap_caps.h"
#include "esp_system.h"

// Bubble monitor constants (inspired by wmbubble)
#define MAX_BUBBLES 50
#define GRAVITY -0.02f
#define WATER_VISCOSITY 0.98f
#define WATER_VOLATILITY 2.0f
#define BUBBLE_SPEED_VARIANCE 0.3f
#define RIPPLE_STRENGTH 0.3f
#define DUCK_SPEED 0.8f

// Screen dimensions - will be determined at runtime
static int SCREEN_WIDTH = 320;
static int SCREEN_HEIGHT = 240;

// Simulated system load values
typedef struct {
    float memory_usage;      // 0.0 - 1.0
    float cpu_load;          // 0.0 - 1.0  
    float io_load;           // 0.0 - 1.0
    int time_counter;
} SystemLoad;

// Water level point for wave simulation
typedef struct {
    float y;
    float dy;
} WaterLevel;

// Bubble structure
typedef struct {
    float x, y;
    float dx, dy;
    int size;
    float alpha;
    int layer;  // 0 = background, 1 = foreground
} Bubble;

// Duck structure
typedef struct {
    float x, y;
    float dx;
    int direction; // 1 = right, -1 = left
} Duck;

// Global state
static SystemLoad sys_load = {0};
static WaterLevel *water_levels = NULL; // Will be allocated dynamically
static int water_levels_count = 0;
static Bubble bubbles[MAX_BUBBLES];
static int num_bubbles = 0;
static Duck duck = {0};
static float bubble_create_timer = 0.0f;

// Simple noise function for load simulation
float simple_noise(float x) {
    x = sinf(x * 0.1f) * cosf(x * 0.07f) * 0.5f + 0.5f;
    return x;
}

// Update simulated system load
void update_system_load(int dt_ms) {
    sys_load.time_counter += dt_ms;
    float time_sec = sys_load.time_counter / 1000.0f;
    
    // Generate pseudo-random load values using sine waves
    sys_load.memory_usage = simple_noise(time_sec * 2.1f) * 0.6f + 0.2f; // 20-80%
    sys_load.cpu_load = simple_noise(time_sec * 3.7f) * 0.8f + 0.1f;     // 10-90%
    sys_load.io_load = simple_noise(time_sec * 1.3f) * 0.4f + 0.1f;      // 10-50%
    
    // Add some spikes occasionally
    if ((int)(time_sec) % 7 == 0 && fmodf(time_sec, 1.0f) < 0.1f) {
        sys_load.cpu_load = fminf(1.0f, sys_load.cpu_load + 0.3f);
    }
}

// Initialize water levels
void init_water_levels(int width) {
    // Free previous allocation if it exists
    if (water_levels) {
        free(water_levels);
    }
    
    // Allocate on heap (prefer heap over stack for large allocations)
    water_levels = (WaterLevel*)malloc(width * sizeof(WaterLevel));
    if (!water_levels) {
        printf("Failed to allocate water_levels array\n");
        return;
    }
    water_levels_count = width;
    printf("Allocated water levels array: %d elements, %zu bytes\n", width, width * sizeof(WaterLevel));
    
    for (int i = 0; i < width; i++) {
        water_levels[i].y = SCREEN_HEIGHT * 0.7f; // Start at 70% height
        water_levels[i].dy = 0.0f;
    }
}

// Update water levels based on memory usage (inspired by wmbubble physics)
void update_water_levels(int width, float dt) {
    // Target water level based on memory usage
    float target_level = SCREEN_HEIGHT * (0.9f - sys_load.memory_usage * 0.6f);
    
    // Set boundary conditions
    water_levels[0].y = target_level;
    water_levels[width - 1].y = target_level;
    
    // Update intermediate points with spring physics
    for (int x = 1; x < width - 1; x++) {
        // Spring force towards average of neighbors
        float target = (water_levels[x - 1].y + water_levels[x + 1].y) / 2.0f;
        float force = (target - water_levels[x].y) * WATER_VOLATILITY * dt;
        
        water_levels[x].dy += force;
        water_levels[x].dy *= WATER_VISCOSITY; // Damping
        
        // Limit speed
        if (water_levels[x].dy > 2.0f) water_levels[x].dy = 2.0f;
        if (water_levels[x].dy < -2.0f) water_levels[x].dy = -2.0f;
        
        // Update position
        water_levels[x].y += water_levels[x].dy * dt;
        
        // Boundary constraints
        if (water_levels[x].y > SCREEN_HEIGHT - 10) {
            water_levels[x].y = SCREEN_HEIGHT - 10;
            water_levels[x].dy = 0;
        }
        if (water_levels[x].y < 10) {
            water_levels[x].y = 10;
            water_levels[x].dy = 0;
        }
        
        // Yield every 100 pixels to prevent watchdog timeout in large displays
        if (x % 100 == 0) {
            taskYIELD();
        }
    }
}

// Create a new bubble
void create_bubble() {
    if (num_bubbles >= MAX_BUBBLES || !water_levels || water_levels_count == 0) return;
    
    int x = rand() % (water_levels_count - 20) + 10;
    
    Bubble *b = &bubbles[num_bubbles];
    b->x = x;
    b->y = water_levels[x].y - 5;
    b->dx = (rand() / (float)RAND_MAX - 0.5f) * 0.5f;
    b->dy = 0;
    b->size = rand() % 4 + 2; // Size 2-5
    b->alpha = 1.0f;
    b->layer = (rand() % 100 < sys_load.cpu_load * 100) ? 1 : 0; // Foreground if high load
    
    num_bubbles++;
    
    // Create ripples in water
    if (x > 1 && x < water_levels_count - 2) {
        water_levels[x - 1].dy -= RIPPLE_STRENGTH;
        water_levels[x].dy -= RIPPLE_STRENGTH * 1.5f;
        water_levels[x + 1].dy -= RIPPLE_STRENGTH;
    }
}

// Update bubbles physics
void update_bubbles(float dt) {
    // Create new bubbles based on CPU load
    bubble_create_timer += dt * sys_load.cpu_load * 3.0f;
    while (bubble_create_timer > 1.0f) {
        create_bubble();
        bubble_create_timer -= 1.0f;
    }
    
    // Update existing bubbles
    for (int i = 0; i < num_bubbles; i++) {
        Bubble *b = &bubbles[i];
        
        // Apply gravity (bubbles rise)
        b->dy += GRAVITY * dt * (b->layer == 0 ? 0.6f : 1.0f); // Background bubbles slower
        
        // Add some horizontal drift
        b->dx += (rand() / (float)RAND_MAX - 0.5f) * 0.1f * dt;
        b->dx *= 0.98f; // Damping
        
        // Update position
        b->x += b->dx * dt;
        b->y += b->dy * dt;
        
        // Remove bubbles that reach water surface or go off screen
        int x_index = (int)fminf(fmaxf(b->x, 0), water_levels_count - 1);
        int water_y = (int)water_levels[x_index].y;
        if (b->y <= water_y || b->x < 0 || b->x >= SCREEN_WIDTH) {
            // Remove bubble by moving last bubble to this position
            bubbles[i] = bubbles[num_bubbles - 1];
            num_bubbles--;
            i--; // Check this position again
        }
    }
}

// Initialize duck
void init_duck() {
    duck.x = SCREEN_WIDTH / 4.0f;
    duck.y = 0; // Will be set to water surface
    duck.dx = DUCK_SPEED;
    duck.direction = 1;
}

// Update duck movement
void update_duck(float dt) {
    // Move duck horizontally
    duck.x += duck.dx * duck.direction * dt;
    
    // Bounce off edges
    if (duck.x <= 10 || duck.x >= SCREEN_WIDTH - 20) {
        duck.direction *= -1;
    }
    
    // Keep duck on water surface
    int duck_x_int = (int)fminf(fmaxf(duck.x, 0), water_levels_count - 1);
    duck.y = water_levels[duck_x_int].y - 8;
}

// Draw water surface
void draw_water(SDL_Renderer *renderer) {
    if (!water_levels) return; // Safety check
    
    // Draw water body
    SDL_SetRenderDrawColor(renderer, 0, 100, 200, 180); // Blue water
    
    for (int x = 0; x < water_levels_count - 1; x++) {
        int y1 = (int)water_levels[x].y;
        int y2 = (int)water_levels[x + 1].y;
        
        // Draw water surface line
        SDL_SetRenderDrawColor(renderer, 100, 150, 255, 255); // Light blue surface
        SDL_RenderLine(renderer, x, y1, x + 1, y2);
        
        // Fill water area
        SDL_SetRenderDrawColor(renderer, 30, 80, 150, 200); // Dark blue water
        for (int y = fmaxf(y1, y2); y < SCREEN_HEIGHT; y++) {
            SDL_RenderPoint(renderer, x, y);
        }
        
        // Yield every 50 pixels to prevent watchdog timeout
        if (x % 50 == 0) {
            taskYIELD();
        }
    }
    
    // Draw some simple "weeds" at bottom for IO load indication
    SDL_SetRenderDrawColor(renderer, 0, 150, 50, 200); // Green weeds
    int weed_height = (int)(sys_load.io_load * 30);
    for (int x = 0; x < water_levels_count; x += 8) {
        int local_height = weed_height + (rand() % 10 - 5);
        if (local_height > 0) {
            for (int y = SCREEN_HEIGHT - local_height; y < SCREEN_HEIGHT; y++) {
                if ((x + y) % 3 == 0) { // Sparse pattern
                    SDL_RenderPoint(renderer, x, y);
                }
            }
        }
    }
}

// Draw bubbles
void draw_bubbles(SDL_Renderer *renderer, int layer) {
    for (int i = 0; i < num_bubbles; i++) {
        if (bubbles[i].layer != layer) continue;
        
        Bubble *b = &bubbles[i];
        int alpha = layer == 0 ? 100 : 200; // Background bubbles more transparent
        
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha);
        
        // Draw bubble as a circle (simplified as filled rectangle for now)
        SDL_FRect bubble_rect = {
            b->x - b->size/2, 
            b->y - b->size/2,
            b->size, 
            b->size
        };
        SDL_RenderFillRect(renderer, &bubble_rect);
        
        // Draw highlight
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, alpha/2);
        SDL_RenderPoint(renderer, b->x - b->size/4, b->y - b->size/4);
    }
}

// Draw simple duck sprite
void draw_duck(SDL_Renderer *renderer) {
    int x = (int)duck.x;
    int y = (int)duck.y;
    
    // Duck body (yellow/orange)
    SDL_SetRenderDrawColor(renderer, 255, 200, 0, 255);
    SDL_FRect body = {x, y, 12, 8};
    SDL_RenderFillRect(renderer, &body);
    
    // Duck head
    SDL_FRect head = {x + (duck.direction > 0 ? 8 : -4), y - 3, 6, 6};
    SDL_RenderFillRect(renderer, &head);
    
    // Duck bill
    SDL_SetRenderDrawColor(renderer, 255, 140, 0, 255);
    SDL_FRect bill = {x + (duck.direction > 0 ? 12 : -2), y - 1, 3, 2};
    SDL_RenderFillRect(renderer, &bill);
    
    // Duck eye
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderPoint(renderer, x + (duck.direction > 0 ? 10 : 2), y - 2);
}

// Draw info display
void draw_info(SDL_Renderer *renderer) {
    // Draw simple load indicators as bars
    int bar_width = SCREEN_WIDTH / 4;
    int bar_height = 4;
    
    // Memory usage bar (top)
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
    SDL_FRect mem_bg = {5, 5, bar_width, bar_height};
    SDL_RenderFillRect(renderer, &mem_bg);
    
    SDL_SetRenderDrawColor(renderer, 0, 255, 100, 255);
    SDL_FRect mem_bar = {5, 5, bar_width * sys_load.memory_usage, bar_height};
    SDL_RenderFillRect(renderer, &mem_bar);
    
    // CPU load bar
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
    SDL_FRect cpu_bg = {5, 12, bar_width, bar_height};
    SDL_RenderFillRect(renderer, &cpu_bg);
    
    SDL_SetRenderDrawColor(renderer, 255, 100, 0, 255);
    SDL_FRect cpu_bar = {5, 12, bar_width * sys_load.cpu_load, bar_height};
    SDL_RenderFillRect(renderer, &cpu_bar);
    
    // IO load bar
    SDL_SetRenderDrawColor(renderer, 100, 100, 100, 200);
    SDL_FRect io_bg = {5, 19, bar_width, bar_height};
    SDL_RenderFillRect(renderer, &io_bg);
    
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255);
    SDL_FRect io_bar = {5, 19, bar_width * sys_load.io_load, bar_height};
    SDL_RenderFillRect(renderer, &io_bar);
}

void* sdl_thread(void* args) {
    printf("Bubble Monitor - SDL3 on ESP32\n");

    if (!SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS)) {
        printf("Unable to initialize SDL: %s\n", SDL_GetError());
        return NULL;
    }
    printf("SDL initialized successfully\n");

    // Query display dimensions
    const SDL_DisplayMode *display_mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    if (display_mode) {
        SCREEN_WIDTH = display_mode->w;
        SCREEN_HEIGHT = display_mode->h;
        printf("Using display resolution: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    } else {
        printf("Failed to get display mode, using defaults: %dx%d\n", SCREEN_WIDTH, SCREEN_HEIGHT);
    }

    SDL_Window *window = SDL_CreateWindow("Bubble Monitor", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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

    // Initialize simulation
    init_water_levels(SCREEN_WIDTH);
    init_duck();
    
    SDL_Event event;
    int running = 1;
    Uint64 last_time = SDL_GetTicks();
    
    printf("Entering main loop...\n");
    
    int frame_counter = 0;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        // Calculate delta time
        Uint64 current_time = SDL_GetTicks();
        float dt = (current_time - last_time) / 1000.0f;
        last_time = current_time;
        
        // Limit delta time to prevent large jumps
        if (dt > 0.1f) dt = 0.1f;

        // Update simulation
        update_system_load((int)(dt * 1000));
        update_water_levels(SCREEN_WIDTH, dt * 60.0f); // Speed up water physics
        update_bubbles(dt * 60.0f);
        update_duck(dt * 60.0f);

        // Render
        SDL_SetRenderDrawColor(renderer, 20, 30, 40, 255); // Dark blue-grey background (sky)
        SDL_RenderClear(renderer);

        // Draw background elements
        draw_water(renderer);
        draw_bubbles(renderer, 0); // Background bubbles

        // Draw foreground elements
        draw_duck(renderer);
        draw_bubbles(renderer, 1); // Foreground bubbles
        
        // Draw info display
        draw_info(renderer);

        SDL_RenderPresent(renderer);
        
        // Monitor heap every 5 seconds (~300 frames at 60 FPS)
        if (++frame_counter % 300 == 0) {
            printf("Frame %d - Free heap: %lu bytes, Free PSRAM: %lu bytes\n", 
                   frame_counter, 
                   (unsigned long)esp_get_free_heap_size(),
                   (unsigned long)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
        }
        
        // Yield to other tasks every frame to prevent watchdog timeout
        taskYIELD();
        vTaskDelay(pdMS_TO_TICKS(16)); // ~60 FPS
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return NULL;
}

void app_main(void) {
    printf("=== Heap Status Before SDL Thread Creation ===\n");
    printf("Free heap: %lu bytes\n", (unsigned long)esp_get_free_heap_size());
    printf("Free PSRAM: %lu bytes\n", (unsigned long)heap_caps_get_free_size(MALLOC_CAP_SPIRAM));
    
    pthread_t sdl_pthread;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 65536);  // Increased stack size to 64KB for SDL rendering

    int ret = pthread_create(&sdl_pthread, &attr, sdl_thread, NULL);
    if (ret != 0) {
        printf("Failed to create SDL thread: %d\n", ret);
        return;
    }

    pthread_detach(sdl_pthread);
}