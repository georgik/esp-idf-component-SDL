# Simple Direct Layer - SDL 3.3.0 (ESP-IDF Component)

🎮 **Universal SDL3 component** for ESP-IDF with automatic board abstraction and zero-configuration setup.

## ⚡ Quick Setup

### Installation via Component Registry
```bash
idf.py add-dependency "georgik/sdl"
```

### Required: ESP-BSP-SDL Abstraction Layer
**⚠️ Important**: This SDL component requires the ESP-BSP-SDL abstraction layer for board support:

```bash
# Add SDL component from registry
idf.py add-dependency "georgik/sdl"

# Add board abstraction layer (GitHub only due to Component Registry limitations)
cd components
git clone --depth 1 https://github.com/georgik/esp-idf-component-SDL_bsp.git georgik__sdl_bsp

# Or add as git submodule
cd components
git submodule add https://github.com/georgik/esp-idf-component-SDL_bsp.git georgik__sdl_bsp
```

**Documentation**: https://github.com/georgik/esp-idf-component-SDL_bsp

## 🎯 Features

- **🌍 Universal Board Support** - Works on ESP32-S3, ESP32-P4, and more
- **📱 Multiple Display Interfaces** - SPI, RGB, MIPI-DSI
- **🎨 Touch Support** - Optional touch interface abstraction
- **⚡ Zero Configuration** - Board selection via menuconfig
- **🔧 Clean Architecture** - No BSP symbol conflicts

## 🎛️ Supported Boards

### ✅ Fully Tested & Working
- **ESP-Box-3** - 320×240 ILI9341, FT5x06 Touch, OCTAL PSRAM
- **M5 Atom S3** - 128×128 GC9A01, Compact design
- **M5Stack Core S3** - 320×240 ILI9341, FT5x06 Touch, QUAD PSRAM
- **ESP32-P4 Function EV** - 1280×800 MIPI-DSI, GT1151 Touch, 32MB PSRAM
- **ESP32-S3-LCD-EV-Board** - 800×480 RGB, GT1151 Touch, OCTAL PSRAM
- **M5Stack Tab5** - 720×1280 MIPI-DSI (portrait), GT911 Touch, 32MB PSRAM @ 200MHz

### 🔧 Additional Support
- **ESP32-C6 DevKit** - Generic BSP for custom displays
- **ESP32-C3-LCDkit** - ESP32-C3 development kit
- **Universal ESP BSP Generic** - Any ESP32 + external display

## 🎨 Usage

### Basic SDL Application
```c
#include "SDL3/SDL.h"
#include "esp_bsp_sdl.h"

void app_main(void)
{
    // Initialize board abstraction layer
    esp_bsp_sdl_display_config_t config;
    esp_lcd_panel_handle_t panel_handle;
    esp_lcd_panel_io_handle_t panel_io_handle;
    
    ESP_ERROR_CHECK(esp_bsp_sdl_init(&config, &panel_handle, &panel_io_handle));
    
    // Initialize SDL with automatic board detection
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        printf("SDL_Init failed: %s\n", SDL_GetError());
        return;
    }
    
    // Get actual display resolution (automatically detected)
    const SDL_DisplayMode *mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
    printf("Display: %dx%d on %s\n", mode->w, mode->h, esp_bsp_sdl_get_board_name());
    
    // Your SDL code here...
    
    SDL_Quit();
}
```

### Board Configuration
```bash
# Configure board via menuconfig (no CMakeLists.txt changes needed!)
idf.py menuconfig
# Navigate to: "ESP-BSP SDL Configuration" > "Select Target Board"

# Build for selected board
idf.py build flash monitor
```

## 💡 Examples

### Built-in Examples
This component includes 4 ready-to-use examples:
```bash
idf.py create-project-from-example "georgik/sdl:snow"       # Snow simulation
idf.py create-project-from-example "georgik/sdl:conway"     # Conway's Game of Life  
idf.py create-project-from-example "georgik/sdl:mandelbrot" # Mandelbrot fractal
idf.py create-project-from-example "georgik/sdl:bubble"     # Bubble physics
```

### External Projects
- [ESP32 SDL3 Example](https://github.com/georgik/esp32-sdl3-example) - Basic SDL setup
- [OpenTyrian](https://github.com/georgik/OpenTyrian) - Full game port
- [Weather Display](https://github.com/georgik/esp32-weather-display) - IoT dashboard
- [ESP32 Swift Examples](https://github.com/georgik/esp32-sdl3-swift-example/) - Embedded Swift + SDL

## 🏗️ Architecture

### Layer Separation
```
Your SDL Application
        ↓
georgik/sdl (3.3.0~2)     ← This component (board-agnostic)
        ↓
georgik/sdl_bsp           ← Board abstraction layer
        ↓  
ESP-BSP Components        ← Board-specific drivers
```

### Benefits
- **🎯 Write Once, Run Anywhere** - Same SDL code on all boards
- **⚡ Automatic Hardware Detection** - No manual configuration
- **🔧 Clean Dependencies** - No BSP symbol conflicts
- **📱 Touch Ready** - Optional touch interface support

## 🍺 ESPBrew Multi-Board Development

**ESPBrew** - Advanced TUI tool for building SDL projects across multiple boards:

```bash
# Install ESPBrew
git clone https://github.com/georgik/espbrew && cd espbrew
cargo build --release

# Use in any SDL project
cd your-sdl-project
espbrew .  # Interactive TUI - select boards and build
espbrew --cli-only .  # Automated builds for all boards
```

**Perfect for SDL Examples**: Build all SDL examples (`snow`, `conway`, `mandelbrot`, `bubble`) across multiple boards simultaneously. ESPBrew automatically discovers board configurations and manages parallel builds.

**Learn More**: https://github.com/georgik/espbrew

## 🔧 Migration from Old Approach

### Before (Manual BSP Selection)
```cmake
# Old approach - CMakeLists.txt
set(ENV{BUILD_BOARD} "esp-box-3_noglib")
```

### After (Menuconfig Selection)
```bash
# New approach - menuconfig
idf.py menuconfig  # Select board once
idf.py build       # No CMakeLists.txt changes needed
```

## 📖 Documentation

- **SDL3 Official**: https://wiki.libsdl.org/SDL3/
- **ESP-BSP-SDL Layer**: https://github.com/georgik/esp-idf-component-SDL_bsp
- **Component Registry**: https://components.espressif.com/components/georgik/sdl


