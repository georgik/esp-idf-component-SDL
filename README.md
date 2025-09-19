# Simple Direct Layer - SDL 3.3.0 (ESP-IDF port)

🎮 **Advanced SDL3 Component for ESP32** with universal board support and automatic hardware abstraction.

This is a comprehensive component wrapper for upstream SDL 3.3.0 designed for ESP-IDF, featuring:
- **🌍 Universal Board Support** - Works across ESP32-S3, ESP32-P4, and other variants
- **🎯 Automatic Hardware Detection** - No manual board configuration needed
- **📱 Multiple Display Interfaces** - SPI, RGB, MIPI-DSI support
- **🎨 Built-in Examples** - 4 ready-to-use SDL applications
- **⚡ Zero-Configuration Setup** - Board selection via menuconfig

## 🚀 Quick Start

### Installation
```bash
# Add SDL component from registry
idf.py add-dependency "georgik/sdl=3.3.0~2"

# Add board abstraction layer (GitHub only due to registry limitations)
cd components
git submodule add https://github.com/georgik/esp-idf-component-SDL_bsp.git georgik__sdl_bsp
```

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
idf.py menuconfig  # Navigate to "ESP-BSP SDL Configuration" > "Select Target Board"
idf.py build flash monitor
```

### Alternative: ESPBrew Multi-Board Manager
**🍺 ESPBrew** - TUI tool for managing builds across multiple boards:

```bash
# Install ESPBrew
git clone https://github.com/georgik/espbrew && cd espbrew
cargo build --release

# Use in any SDL example directory
cd your-sdl-project
espbrew .  # Interactive TUI - select boards and press 'b' to build
espbrew --cli-only .  # CLI mode - builds all boards automatically
```

**Features:**
- 🎯 **Auto-Discovery** - Finds all board configurations automatically
- 🖥️ **TUI Interface** - Interactive interface with real-time build monitoring  
- 📊 **Parallel Builds** - Build multiple boards simultaneously
- 📝 **Script Generation** - Creates `build_{board}.sh` and `flash_{board}.sh` scripts
- 📁 **Organized Output** - Separate build directories and logs for each board

**Project**: https://github.com/georgik/espbrew

## 🎯 Featured Examples

This component includes **4 built-in examples** showcasing different SDL features:

### 1. ❄️ **Snow Simulation** (`snow`)
- Beautiful particle physics with falling snow
- Automatic resolution adaptation (128×128 to 1280×720)
- Perfect for demonstrating display capabilities

### 2. 🧬 **Conway's Game of Life** (`conway`) 
- Classic cellular automaton with age-based coloring
- Adaptive grid sizing for any display resolution
- Demonstrates complex algorithmic visualization

### 3. 🌀 **Mandelbrot Fractal** (`mandelbrot`)
- Real-time fractal rendering with color cycling
- Fixed-point arithmetic optimized for embedded systems
- Progressive rendering with zoom animation

### 4. 🫧 **Bubble Physics** (`bubble`)
- Interactive physics simulation with bouncing bubbles
- Touch support on compatible boards
- Showcases fluid animation and user interaction

## 🔧 New ESP-BSP-SDL Abstraction Layer

**⚠️ Important**: This component now requires the **ESP-BSP-SDL abstraction layer** for board support.

### Why ESP-BSP-SDL?
- **🎯 Board Agnostic**: SDL code works on any supported board without modification
- **🔧 Menuconfig Integration**: Board selection through standard ESP-IDF configuration
- **⚡ No Symbol Conflicts**: Clean separation prevents BSP function name conflicts
- **📱 Touch Support**: Optional touch interface abstraction

### Installation (GitHub Required)
```bash
# ESP Component Registry limitations require GitHub installation
cd components
git submodule add https://github.com/georgik/esp-idf-component-SDL_bsp.git georgik__sdl_bsp
```

**📖 Documentation**: https://github.com/georgik/esp-idf-component-SDL_bsp

## 🎛️ Supported Boards

### ✅ Fully Supported (Tested & Working)
- **ESP-Box-3** - 320×240 ILI9341, FT5x06 Touch, OCTAL PSRAM
- **M5 Atom S3** - 128×128 GC9A01, Compact form factor
- **M5Stack Core S3** - 320×240 ILI9341, FT5x06 Touch, QUAD PSRAM
- **ESP32-P4 Function EV** - 1280×800 MIPI-DSI, GT1151 Touch, 32MB PSRAM
- **ESP32-S3-LCD-EV-Board** - 800×480 RGB, GT1151 Touch, OCTAL PSRAM
- **M5Stack Tab5** - 720×1280 MIPI-DSI (portrait), GT911 Touch, 32MB PSRAM

### 🔧 Additional Support
- **ESP32-C6 DevKit** - Generic BSP for custom displays
- **ESP32-C3-LCDkit** - ESP32-C3 development kit
- **Universal ESP BSP Generic** - Works with any ESP32 + external display

## 💡 Examples

### C Examples (External)
- [ESP32 SDL3 Example](https://github.com/georgik/esp32-sdl3-example) - Basic SDL setup
- [OpenTyrian](https://github.com/georgik/OpenTyrian) - Full game port
- [Weather Display](https://github.com/georgik/esp32-weather-display) - IoT dashboard

### Embedded Swift Examples (External) 
- [ESP32 SDL3 Swift Example](https://github.com/georgik/esp32-sdl3-swift-example/)
- [Swift Weather Display](https://github.com/georgik/esp32-swift-weather-display)

## 🏗️ Architecture

```
┌─────────────────┐
│  Your SDL App   │
└─────────┬───────┘
          │
┌─────────▼───────┐
│ georgik/sdl     │ ← This component (board-agnostic)
│ (3.3.0~2)       │
└─────────┬───────┘
          │  
┌─────────▼───────┐
│ georgik/sdl_bsp │ ← Abstraction layer (GitHub only)
└─────────┬───────┘
          │
┌─────────▼───────┐
│ ESP-BSP         │ ← Board-specific drivers
│ Components      │
└─────────────────┘
```

### Key Benefits
1. **🎯 Write Once, Run Anywhere** - Same SDL code on all boards
2. **⚡ Zero Board Configuration** - Automatic hardware detection
3. **🔧 Clean Architecture** - Clear separation of concerns
4. **📱 Touch Ready** - Optional touch support abstraction
5. **🎨 Rich Examples** - 4 built-in demonstrations

## 📖 Documentation

- **Component Registry**: https://components.espressif.com/components/georgik/sdl
- **ESP-BSP-SDL Layer**: https://github.com/georgik/esp-idf-component-SDL_bsp
- **SDL3 Official Docs**: https://wiki.libsdl.org/SDL3/


