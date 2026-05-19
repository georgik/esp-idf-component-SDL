# Simple Direct Layer - SDL 3.3.0 (ESP-IDF port)

Advanced SDL3 Component for ESP32 with universal board support and automatic hardware abstraction.

This is a comprehensive component wrapper for upstream SDL 3.3.0 designed for ESP-IDF, featuring:
- Universal Board Support - Works across ESP32-S3, ESP32-P4, and other variants
- Unified BSP Selector - Uses ESP-BSP's bsp_selector for automatic board detection
- Multiple Display Interfaces - SPI, RGB, MIPI-DSI support
- Built-in Examples - 4 ready-to-use SDL applications
- Zero-Configuration Setup - Board selection via menuconfig

## Quick Start

### Installation

```bash
# Add SDL component from registry
idf.py add-dependency "georgik/sdl"

# Add bsp_selector for unified board selection
idf.py add-dependency "espressif/bsp_selector"
```

**Note**: The ESP-BSP SDL abstraction layer (`esp_sdl_bsp`) is now included in this repository - no separate installation needed!

### Create Project from Examples

```bash
# Create snow simulation project
idf.py create-project-from-example "georgik/sdl:snow"

# Create Conway's Game of Life
idf.py create-project-from-example "georgik/sdl:conway"

# Create Mandelbrot fractal visualizer
idf.py create-project-from-example "georgik/sdl:mandelbrot"

# Create bubble physics simulation
idf.py create-project-from-example "georgik/sdl:bubble"
```

### Basic Setup

```bash
# Configure your board
idf.py menuconfig
# Navigate to "Component config" -> "ESP-BSP" -> "BSP board selection"
# Navigate to "Component config" -> "ESP-BSP" -> "Graphics library selection" -> "SDL"

idf.py build flash monitor
```

### Alternative: ESPBrew Multi-Board Manager

**ESPBrew** - Advanced TUI tool for managing builds across multiple boards with SDL examples:

```bash
# Install ESPBrew
git clone https://github.com/georgik/espbrew && cd espbrew
cargo build --release

# Use in any SDL example directory
cd your-sdl-project
espbrew .  # Interactive TUI - select boards and press 'b' to build
espbrew --cli-only .  # CLI mode - builds all boards automatically
```

**Perfect for SDL Examples**: ESPBrew automatically discovers all board configurations and lets you build SDL examples for multiple boards simultaneously.

## Featured Examples

This component includes **4 built-in examples** showcasing different SDL features:

### 1. Snow Simulation (`snow`)
- Beautiful particle physics with falling snow
- Automatic resolution adaptation (128x128 to 1280x720)
- Perfect for demonstrating display capabilities

### 2. Conway's Game of Life (`conway`)
- Classic cellular automaton with age-based coloring
- Adaptive grid sizing for any display resolution
- Demonstrates complex algorithmic visualization

### 3. Mandelbrot Fractal (`mandelbrot`)
- Real-time fractal rendering with color cycling
- Fixed-point arithmetic optimized for embedded systems
- Progressive rendering with zoom animation

### 4. Bubble Physics (`bubble`)
- Interactive physics simulation with bouncing bubbles
- Touch support on compatible boards
- Showcases fluid animation and user interaction

## Architecture: BSP Selector Integration

**Important**: This component now uses the **unified bsp_selector** from ESP-BSP for board selection.

### Why BSP Selector?

- **Board Agnostic**: SDL code works on any supported board without modification
- **Menuconfig Integration**: Board selection through standard ESP-IDF configuration
- **Unified Selection**: Same selection mechanism used across raylib, LVGL, and SDL
- **Touch Support**: Optional touch interface abstraction

### Architecture Diagram

```
┌─────────────────┐
│  Your SDL App   │
└─────────┬───────┘
          │
┌─────────▼───────┐
│ georgik/sdl     │ ← This component (board-agnostic)
└─────────┬───────┘
          │
┌─────────▼───────┐
│ esp_sdl_bsp     │ ← Abstraction layer (included in repo)
└─────────┬───────┘
          │
┌─────────▼───────┐
│ bsp_selector    │ ← Unified board selection
└─────────┬───────┘
          │
┌─────────▼───────┐
│ ESP-BSP         │ ← Board-specific drivers
└─────────────────┘
```

### Key Benefits

1. **Write Once, Run Anywhere** - Same SDL code on all boards
2. **Zero Board Configuration** - Automatic hardware detection
3. **Clean Architecture** - Clear separation of concerns
4. **Touch Ready** - Optional touch support abstraction
5. **Rich Examples** - 4 built-in demonstrations

## Supported Boards

### Fully Supported (Tested & Working)
- **ESP-Box-3** - 320x240 ILI9341, FT5x06 Touch, OCTAL PSRAM
- **M5 Atom S3** - 128x128 GC9A01, Compact form factor
- **M5Stack Core S3** - 320x240 ILI9341, FT5x06 Touch, QUAD PSRAM
- **ESP32-P4 Function EV Board** - 1280x800 MIPI-DSI, GT1151 Touch, 32MB PSRAM
- **ESP32-S3-LCD-EV-Board** - 800x480 RGB, GT1151 Touch, OCTAL PSRAM
- **M5Stack Tab5** - 720x1280 MIPI-DSI (portrait), GT911 Touch, 32MB PSRAM

### Additional Support
- **ESP32-C6 DevKit** - Generic BSP for custom displays
- **ESP32-C3-LCDkit** - ESP32-C3 development kit
- **Universal ESP BSP Generic** - Works with any ESP32 + external display

## Examples

### C Examples (External)
- [ESP32 SDL3 Example](https://github.com/georgik/esp32-sdl3-example) - Basic SDL setup
- [OpenTyrian](https://github.com/georgik/OpenTyrian) - Full game port
- [Weather Display](https://github.com/georgik/esp32-weather-display) - IoT dashboard

### Embedded Swift Examples (External)
- [ESP32 SDL3 Swift Example](https://github.com/georgik/esp32-sdl3-swift-example/)
- [Swift Weather Display](https://github.com/georgik/esp32-swift-weather-display)

## Documentation

- **Component Registry**: https://components.espressif.com/components/georgik/sdl
- **ESP-BSP Repository**: https://github.com/espressif/esp-bsp
- **SDL3 Official Docs**: https://wiki.libsdl.org/SDL3/

## Repository Structure

```
esp-idf-component-SDL/
├── sdl/                    # Main SDL component
│   ├── CMakeLists.txt
│   ├── idf_component.yml
│   ├── src/
│   │   └── video/esp-idf/  # ESP-IDF video driver
│   └── examples/
│       ├── snow/
│       ├── conway/
│       ├── mandelbrot/
│       └── bubble/
├── esp_sdl_bsp/            # BSP abstraction layer
│   ├── CMakeLists.txt
│   ├── include/            # esp_bsp_sdl.h
│   ├── src/
│   │   ├── esp_bsp_sdl_common.c
│   │   └── boards/         # Board-specific implementations
│   └── README.md
└── README.md
```

## Migration from Old Architecture

If you were using the old `SDL_BSP_*` configuration system:

1. **Update dependencies**: Remove individual BSP dependencies, add `espressif/bsp_selector`
2. **Update board selection**: Use menuconfig to select board under `ESP-BSP` instead of custom `SDL_BSP_*` options
3. **Update CMakeLists.txt**: Use `CONFIG_BSP_SELECT_*` instead of `CONFIG_SDL_BSP_*`
4. **No code changes needed**: Your SDL application code remains the same!

See `plan-integration.txt` for detailed migration guide.

## License

- SDL: Zlib License
- ESP-IDF port: MIT License
