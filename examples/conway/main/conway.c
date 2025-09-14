#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include "SDL3/SDL.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

// Conway's Game of Life for SDL3 ESP-IDF
// Universal board support - adapts to any display resolution

// Grid configuration - will adapt to display resolution
#define MIN_GRID_WIDTH 32
#define MIN_GRID_HEIGHT 24
#define MAX_GRID_WIDTH 128
#define MAX_GRID_HEIGHT 96
#define CELL_MIN_SIZE 3
#define CELL_MAX_SIZE 12
#define RESET_AFTER_GENERATIONS 500

// Screen dimensions will be determined at runtime from SDL display
static int SCREEN_WIDTH = 320;  // Default fallback
static int SCREEN_HEIGHT = 240; // Default fallback

// Grid configuration - computed at runtime
static int GRID_WIDTH = 64;
static int GRID_HEIGHT = 48;
static int CELL_SIZE = 5;

typedef struct {
    uint8_t **current;
    uint8_t **next;
    int width;
    int height;
    int generation;
} GameGrid;

static GameGrid game_grid = {0};

// Color gradient for cell age visualization
typedef struct {
    uint8_t r, g, b, a;
} Color;

// Age-based color mapping (similar to Rust version)
Color age_to_color(uint8_t age) {
    if (age == 0) {
        return (Color){0, 0, 0, 255}; // Black for dead cells
    }
    
    const uint8_t max_age = 15;
    uint8_t clamped_age = (age > max_age) ? max_age : age;
    
    // Blue to cyan to white gradient
    uint8_t r = (255 * clamped_age) / max_age;
    uint8_t g = (255 * clamped_age) / max_age;
    uint8_t b = 255; // Keep blue channel high
    
    return (Color){r, g, b, 255};
}

// Allocate 2D grid
uint8_t** allocate_grid(int width, int height) {
    uint8_t **grid = malloc(height * sizeof(uint8_t*));
    if (!grid) return NULL;
    
    for (int y = 0; y < height; y++) {
        grid[y] = calloc(width, sizeof(uint8_t));
        if (!grid[y]) {
            // Cleanup on failure
            for (int i = 0; i < y; i++) {
                free(grid[i]);
            }
            free(grid);
            return NULL;
        }
    }
    return grid;
}

// Free 2D grid
void free_grid(uint8_t **grid, int height) {
    if (!grid) return;
    for (int y = 0; y < height; y++) {
        free(grid[y]);
    }
    free(grid);
}

// Initialize grid with random pattern
void randomize_grid(GameGrid *grid) {
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            grid->current[y][x] = (rand() % 3 == 0) ? 1 : 0; // ~33% alive
        }
    }
    
    // Add classic glider pattern in the center
    int center_x = grid->width / 2;
    int center_y = grid->height / 2;
    
    // Glider pattern (relative coordinates)
    int glider[][2] = {{1, 0}, {2, 1}, {0, 2}, {1, 2}, {2, 2}};
    int glider_size = sizeof(glider) / sizeof(glider[0]);
    
    for (int i = 0; i < glider_size; i++) {
        int x = center_x + glider[i][0] - 1;
        int y = center_y + glider[i][1] - 1;
        if (x >= 0 && x < grid->width && y >= 0 && y < grid->height) {
            grid->current[y][x] = 1;
        }
    }
}

// Count live neighbors (with wraparound)
int count_neighbors(GameGrid *grid, int x, int y) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dx == 0 && dy == 0) continue;
            
            int nx = (x + dx + grid->width) % grid->width;
            int ny = (y + dy + grid->height) % grid->height;
            
            if (grid->current[ny][nx] > 0) {
                count++;
            }
        }
    }
    return count;
}

// Update Game of Life rules
void update_game_of_life(GameGrid *grid) {
    // Clear next generation
    for (int y = 0; y < grid->height; y++) {
        memset(grid->next[y], 0, grid->width);
    }
    
    // Apply Conway's rules
    for (int y = 0; y < grid->height; y++) {
        for (int x = 0; x < grid->width; x++) {
            int neighbors = count_neighbors(grid, x, y);
            uint8_t current_age = grid->current[y][x];
            
            if (current_age > 0) {
                // Live cell
                if (neighbors == 2 || neighbors == 3) {
                    // Survive and age (with saturation)
                    grid->next[y][x] = (current_age < 255) ? current_age + 1 : 255;
                } else {
                    // Die
                    grid->next[y][x] = 0;
                }
            } else {
                // Dead cell
                if (neighbors == 3) {
                    // Birth
                    grid->next[y][x] = 1;
                }
            }
        }
    }
    
    // Swap current and next
    uint8_t **temp = grid->current;
    grid->current = grid->next;
    grid->next = temp;
    
    grid->generation++;
}

// Initialize game grid based on screen resolution
int init_game_grid(int screen_width, int screen_height) {
    // Calculate optimal grid size and cell size
    CELL_SIZE = CELL_MIN_SIZE;
    
    // Try to find a good fit
    for (int size = CELL_MAX_SIZE; size >= CELL_MIN_SIZE; size--) {
        int potential_width = screen_width / size;
        int potential_height = screen_height / size;
        
        if (potential_width >= MIN_GRID_WIDTH && 
            potential_height >= MIN_GRID_HEIGHT &&
            potential_width <= MAX_GRID_WIDTH && 
            potential_height <= MAX_GRID_HEIGHT) {
            CELL_SIZE = size;
            GRID_WIDTH = potential_width;
            GRID_HEIGHT = potential_height;
            break;
        }
    }
    
    printf("Conway's Game of Life: Grid %dx%d, Cell size %d pixels\n", 
           GRID_WIDTH, GRID_HEIGHT, CELL_SIZE);
    
    // Allocate grids
    game_grid.width = GRID_WIDTH;
    game_grid.height = GRID_HEIGHT;
    game_grid.generation = 0;
    
    game_grid.current = allocate_grid(GRID_WIDTH, GRID_HEIGHT);
    game_grid.next = allocate_grid(GRID_WIDTH, GRID_HEIGHT);
    
    if (!game_grid.current || !game_grid.next) {
        printf("Failed to allocate game grids!\n");
        return 0;
    }
    
    randomize_grid(&game_grid);
    return 1;
}

// Cleanup game grid
void cleanup_game_grid() {
    free_grid(game_grid.current, game_grid.height);
    free_grid(game_grid.next, game_grid.height);
    game_grid.current = NULL;
    game_grid.next = NULL;
}

// Draw the game grid
void draw_grid(SDL_Renderer *renderer) {
    for (int y = 0; y < game_grid.height; y++) {
        for (int x = 0; x < game_grid.width; x++) {
            uint8_t age = game_grid.current[y][x];
            Color color = age_to_color(age);
            
            SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
            
            // Draw cell as filled rectangle
            SDL_FRect rect = {
                x * CELL_SIZE,
                y * CELL_SIZE,
                CELL_SIZE,
                CELL_SIZE
            };
            
            SDL_RenderFillRect(renderer, &rect);
        }
    }
}

// Draw generation counter and title
void draw_info(SDL_Renderer *renderer) {
    // This is a simplified version - in a real implementation you'd want
    // to render text using SDL_ttf or a bitmap font
    // For now, we'll just draw some basic indicators
    
    // Draw a simple generation indicator using rectangles
    // Top-left corner: small squares representing generation count (mod 16)
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    int gen_indicator = game_grid.generation % 16;
    for (int i = 0; i < gen_indicator; i++) {
        SDL_FRect dot = {i * 3 + 2, 2, 2, 2};
        SDL_RenderFillRect(renderer, &dot);
    }
    
    // Draw a border around the game area
    SDL_SetRenderDrawColor(renderer, 64, 64, 64, 255);
    SDL_FRect border = {0, 0, GRID_WIDTH * CELL_SIZE, GRID_HEIGHT * CELL_SIZE};
    SDL_RenderRect(renderer, &border);
}

void* sdl_thread(void* args) {
    printf("Conway's Game of Life - SDL3 on ESP32\n");

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

    // Initialize game grid based on screen resolution
    if (!init_game_grid(SCREEN_WIDTH, SCREEN_HEIGHT)) {
        printf("Failed to initialize game grid\n");
        SDL_Quit();
        return NULL;
    }

    SDL_Window *window = SDL_CreateWindow("Conway's Game of Life", SCREEN_WIDTH, SCREEN_HEIGHT, 0);
    if (!window) {
        printf("Failed to create window: %s\n", SDL_GetError());
        cleanup_game_grid();
        SDL_Quit();
        return NULL;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        printf("Failed to create renderer: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        cleanup_game_grid();
        SDL_Quit();
        return NULL;
    }

    SDL_Event event;
    int running = 1;
    printf("Conway's Game of Life - Generation 0\n");

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = 0;
            }
        }

        // Update game logic
        update_game_of_life(&game_grid);
        
        // Reset if too many generations have passed
        if (game_grid.generation >= RESET_AFTER_GENERATIONS) {
            printf("Resetting after %d generations\n", RESET_AFTER_GENERATIONS);
            randomize_grid(&game_grid);
            game_grid.generation = 0;
        }
        
        // Print generation periodically
        if (game_grid.generation % 50 == 0) {
            printf("Generation: %d\n", game_grid.generation);
        }

        // Render
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Black background
        SDL_RenderClear(renderer);

        draw_grid(renderer);
        draw_info(renderer);

        SDL_RenderPresent(renderer);

        vTaskDelay(pdMS_TO_TICKS(100)); // ~10 FPS for Conway's Game of Life
    }

    cleanup_game_grid();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return NULL;
}

void app_main(void) {
    pthread_t sdl_pthread;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    pthread_attr_setstacksize(&attr, 40960);  // Larger stack for game logic

    int ret = pthread_create(&sdl_pthread, &attr, sdl_thread, NULL);
    if (ret != 0) {
        printf("Failed to create SDL thread: %d\n", ret);
        return;
    }

    pthread_detach(sdl_pthread);
}