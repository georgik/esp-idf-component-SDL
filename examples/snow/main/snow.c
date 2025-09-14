#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define MAX_SNOWFLAKES 100
// Screen dimensions will be determined at runtime from SDL display
static int SCREEN_WIDTH = 320;  // Default fallback
static int SCREEN_HEIGHT = 240; // Default fallback

typedef struct {
    float x, y, speed;
} Snowflake;

Snowflake snowflakes[MAX_SNOWFLAKES];

void initialize_snowflakes() {
    for (int i = 0; i < MAX_SNOWFLAKES; i++) {
        snowflakes[i].x = rand() % SCREEN_WIDTH;
        snowflakes[i].y = rand() % SCREEN_HEIGHT;
        snowflakes[i].speed = (rand() % 50 + 50) / 100.0f; // Speed between 0.5 and 1.0
    }
}

void update_snowflakes() {
    for (int i = 0; i < MAX_SNOWFLAKES; i++) {
        snowflakes[i].y += snowflakes[i].speed;
        if (snowflakes[i].y > SCREEN_HEIGHT) {
            snowflakes[i].y = 0;
            snowflakes[i].x = rand() % SCREEN_WIDTH;
            snowflakes[i].speed = (rand() % 50 + 50) / 100.0f;
        }
    }
}

void draw_snowflakes(SDL_Renderer *renderer) {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255); // White color for snowflakes
    for (int i = 0; i < MAX_SNOWFLAKES; i++) {
        SDL_RenderPoint(renderer, snowflakes[i].x, snowflakes[i].y);
    }
}

void* sdl_thread(void* args) {
    printf("SDL3 on ESP32\n");

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

    SDL_Window *window = SDL_CreateWindow("Snow Simulation", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
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

    // Initialize snowflakes with the correct screen dimensions
    initialize_snowflakes();

    SDL_Event event;
    int running = 1;
    printf("Entering main loop...\n");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        update_snowflakes();

        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        draw_snowflakes(renderer);

        SDL_RenderPresent(renderer);

        vTaskDelay(pdMS_TO_TICKS(16)); // ~60 FPS
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
    pthread_attr_setstacksize(&attr, 32768);  // Set the stack size for the thread

    int ret = pthread_create(&sdl_pthread, &attr, sdl_thread, NULL);
    if (ret != 0) {
        printf("Failed to create SDL thread: %d\n", ret);
        return;
    }

    pthread_detach(sdl_pthread);
}
