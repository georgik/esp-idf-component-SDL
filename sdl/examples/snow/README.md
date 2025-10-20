# ❄️ Snow Simulation - SDL3 Example

**Simple particle simulation** with falling white pixels that resemble snow.

🎯 **Cross-Platform** - The same SDL3 code runs on different ESP32 boards with various display sizes.

## 🎮 What This Demonstrates

- **❄️ Basic Particle System** - Moving white pixels falling down the screen
- **📱 Resolution Adaptation** - Automatically scales to different display sizes  
- **⚡ Simple Animation** - Basic SDL rendering loop for embedded systems
- **🎛️ Board Abstraction** - Shows how SDL works across different ESP32 boards

## 🚀 Quick Start

### Create Project from Example
```bash
# Create snow project from component registry
idf.py create-project-from-example "georgik/sdl:snow"

# Configure board and build
idf.py menuconfig  # Select your board in "ESP-BSP SDL Configuration"
idf.py build flash monitor
```

### Alternative: Using ESPBrew
```bash
# Multi-board build manager with TUI
espbrew .  # Interactive mode - select boards and press 'b'
espbrew --cli-only .  # Automatic mode - builds all detected boards
```

**ESPBrew** automatically detects all board configurations and can build/flash multiple boards:
- **Auto-Discovery** - Finds `sdkconfig.defaults.*` files automatically  
- **TUI Interface** - Interactive selection and real-time build monitoring
- **Parallel Builds** - Build multiple boards simultaneously
- **Project**: https://github.com/georgik/espbrew

![Architecture Overview](docs/architecture.png)

## 🚀 Quick Start

### Board Selection Using SDKCONFIG_DEFAULTS Parameter

```bash
# M5 Atom S3 (128×128, No PSRAM) - Default
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" build flash monitor

# ESP32-S3-BOX-3 (320×240, OCTAL PSRAM)
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp-box-3" build flash monitor

# M5Stack CoreS3 (320×240, QUAD PSRAM) 
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_core_s3" build flash monitor

# M5Stack Tab5 (1280×720, 32MB PSRAM, ESP32-P4)
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_tab5" build flash monitor

# ESP32-P4 RISC-V (up to 1280×800, 32MB PSRAM)
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp32_p4_function_ev_board" build flash monitor

# Universal: ANY ESP32 DevKit + configurable display
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_generic" menuconfig  # Configure your hardware
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_generic" build flash monitor

# DevKit testing (virtual display, no physical display needed)
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_devkit" build flash monitor
```

### First Time Setup (Default Board)

```bash
# Uses M5 Atom S3 by default
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" build flash monitor
```

## 🎛️ Supported Boards

| Board | Architecture | Display | PSRAM | Key Features |
|-------|-------------|---------|-------|--------------|
| **`esp_bsp_generic`** | **Any ESP32** | **Configurable** | **Configurable** | **🔧 Universal DevKit + Display** |
| `m5_atom_s3` | ESP32-S3 (Xtensa) | 128×128 GC9A01 | None | ✅ **Working** - Default board |
| `m5stack_core_s3` | ESP32-S3 (Xtensa) | 320×240 ILI9341 | QUAD | Touch interface, large display |
| `m5stack_tab5` | **ESP32-P4 (RISC-V)** | 1280×720 IPS via MIPI-DSI | 32MB | 5-inch display, GT911 touch |
| `esp32_p4_function_ev` | **ESP32-P4 (RISC-V)** | up to 1280×800 | 32MB | PPA, H264, MIPI interfaces |
| `esp32_s3_lcd_ev` | ESP32-S3 (Xtensa) | Multiple LCD types | 16MB OCTAL | Flexible interface support |
| `esp32_s3_eye` | ESP32-S3 (Xtensa) | 240×240 circular | 8MB OCTAL | Camera + microphone |
| `esp32_s3_korvo_2` | ESP32-S3 (Xtensa) | LCD + Camera | OCTAL | Audio focus, dual microphones |
| `esp_bsp_devkit` | Any ESP32 | Virtual 240×320 | Configurable | LEDs/buttons, no display |
| `esp_box_3` | ESP32-S3 (Xtensa) | 320×240 ILI9341 | OCTAL | ✅ **Working** |

## 🌟 ESP BSP Generic - Universal DevKit Support

The **most flexible option** - works with **any ESP32 DevKit**!

### ✨ Features
- **🌍 Universal**: Any ESP32 variant (ESP32, ESP32-S2, ESP32-S3, ESP32-C3, ESP32-C6, ESP32-H2)
- **🔧 Configurable Display**: ST7789, ILI9341, GC9A01 with custom pins & resolution
- **📱 Touch Support**: I2C touch controllers  
- **🎛️ Buttons & LEDs**: Up to 5 buttons, GPIO/RGB/addressable LEDs
- **💾 File Systems**: SPIFFS, microSD card support
- **🎨 LVGL Ready**: Full LVGL integration support

### 🛠️ Configuration
```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_generic" menuconfig  
# Navigate to: Component config → BSP Generic
# Configure display, pins, resolution, touch, etc.
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_generic" build flash monitor
```

**📖 Complete Guide**: See [ESP_BSP_GENERIC_GUIDE.md](ESP_BSP_GENERIC_GUIDE.md) for detailed setup instructions.

## 🏗️ Architecture Highlights

### 🎯 Board-Agnostic SDL Layer
- **SDL3 Component**: Remains completely hardware-independent
- **ESP-BSP SDL Abstraction**: Handles all board-specific operations
- **Automatic Resolution Detection**: SDL queries real display dimensions
- **Dynamic Configuration**: Applications adapt to any display size

### ⚡ Smart Board Switching
- **Automatic Detection**: CMake detects board changes and cleans configuration
- **Target Mapping**: Automatically sets correct ESP-IDF target (ESP32-S3, ESP32-P4, etc.)
- **Memory Optimization**: Board-specific PSRAM and heap configurations
- **Clean Builds**: Forces configuration regeneration when switching boards

### 🔧 Multi-Architecture Support
- **Xtensa (ESP32-S3)**: All ESP32-S3 based development boards
- **RISC-V (ESP32-P4)**: Next-gen ESP32-P4 with advanced multimedia features
- **Universal**: ESP BSP Generic works with any ESP32 variant

## 🚀 Advanced Usage

### Board Switching
```bash
# Switch between M5 Atom S3 and ESP32-S3-BOX-3
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" build flash monitor
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp-box-3" build flash monitor

# Switch to M5Stack Tab5
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_tab5" build flash monitor

# Switch to other boards
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp32_p4_function_ev_board" build flash monitor

# Clean switch if needed (recommended when switching between different boards)
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" fullclean build flash monitor
```

### Custom Board Development
```bash
# Use ESP BSP Generic as starting point
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_generic" build
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp_bsp_generic" menuconfig  # Customize hardware configuration
# Save configuration for reuse
cp sdkconfig sdkconfig.defaults.my_custom_board
```

### IDE Integration

**VS Code ESP-IDF Extension:**
```json
{
    "idf.cmakeCompilerArgs": ["-D", "SDKCONFIG_DEFAULTS=sdkconfig.defaults.m5_atom_s3"]
}
```

**CLion:** Project Settings → CMake → CMake options: `-D SDKCONFIG_DEFAULTS=sdkconfig.defaults.m5_atom_s3`

## 🎨 Application Benefits

### Resolution Independence
```c
// This code works on ANY supported board!
const SDL_DisplayMode *display_mode = SDL_GetCurrentDisplayMode(SDL_GetPrimaryDisplay());
if (display_mode) {
    SCREEN_WIDTH = display_mode->w;   // Automatically: 128, 240, 320, or 800+
    SCREEN_HEIGHT = display_mode->h;  // Automatically: 128, 240, 320, or 600+
    printf("Running on %dx%d display\\n", SCREEN_WIDTH, SCREEN_HEIGHT);
}
```

### Board Information Access  
```c
#include "esp_bsp_sdl.h"
printf("Running on: %s\\n", esp_bsp_sdl_get_board_name());
// Output: "M5 Atom S3", "ESP32-P4 Function EV", etc.
```

## 📚 Documentation

- **[BOARDS.md](BOARDS.md)** - Complete board specifications and usage guide
- **[ESP_BSP_GENERIC_GUIDE.md](ESP_BSP_GENERIC_GUIDE.md)** - Universal DevKit configuration guide  
- **[BSP_SYMBOL_CONFLICTS.md](BSP_SYMBOL_CONFLICTS.md)** - 🔧 Technical solution for BSP symbol conflicts
- **[Architecture Details](docs/architecture.md)** - Technical implementation details
- **[Contributing Guide](CONTRIBUTING.md)** - Add support for new boards

## 🔧 Troubleshooting

### BSP Symbol Conflicts Resolution ✅

The project includes an advanced solution for BSP symbol conflicts that allows seamless switching between boards:

```bash
# M5 Atom S3 - Default (no environment variable needed)
idf.py -DBOARD=m5_atom_s3 build flash monitor

# ESP32-S3-BOX-3 - Requires environment variable to avoid symbol conflicts
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 build flash monitor
```

**How it works:**
- **Conditional BSP Loading**: Only the required BSP component is linked at build time
- **Environment Variable Control**: `BUILD_FOR_BOX3=1` switches BSP selection
- **No Symbol Conflicts**: Forward declarations are conditional based on selected BSP
- **Clean Builds**: Automatic detection and reconfiguration when switching boards

This solution eliminates the common problem of multiple BSPs defining identical function names.

### Board Switch Issues
```bash
# Clean configuration when switching boards
rm -f sdkconfig
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.<new_board>" build
```

### Memory Issues
- **M5 Atom S3**: Uses internal RAM optimization (no PSRAM)
- **All others**: Configured for PSRAM with appropriate heap allocation policies

### Display Problems
1. **Check board selection** matches your hardware
2. **Verify connections** for ESP BSP Generic custom configurations  
3. **Review logs** for BSP initialization messages

## 🛠️ Development

### Adding New Boards

1. **Create configuration**: `sdkconfig.defaults.<board_name>`
2. **Add board implementation**: `components/esp_bsp_sdl/src/boards/esp_bsp_sdl_<board_name>.c`
3. **Update CMakeLists.txt**: Add board to conditional compilation  
4. **Update documentation**: Add to BOARDS.md and README.md

### Testing
```bash
# Test current configuration
idf.py build

# Test board switching (working boards)
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" build
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp-box-3" build

# Test other boards
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp32_p4_function_ev_board" build
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_core_s3" build
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_tab5" build
```

## 📊 Project Structure

```
snow/
├── components/
│   ├── esp_bsp_sdl/                    # 🎯 Universal board abstraction
│   │   ├── include/esp_bsp_sdl.h       # Board-agnostic API
│   │   ├── src/boards/                 # Board-specific implementations
│   │   │   ├── esp_bsp_sdl_m5_atom_s3.c
│   │   │   ├── esp_bsp_sdl_esp32_p4_function_ev.c
│   │   │   └── esp_bsp_sdl_esp_bsp_generic.c
│   │   └── CMakeLists.txt               # Conditional board compilation
│   └── georgik__sdl/                   # Board-agnostic SDL3 component
├── sdkconfig.defaults.<board>/         # Board-specific configurations
├── main/                              # Universal SDL3 application
├── BOARDS.md                          # Complete board guide
├── ESP_BSP_GENERIC_GUIDE.md           # Universal DevKit guide  
└── README.md                          # This file
```

## 🎖️ Create Your Own Project

```bash
# Create from template
idf.py create-project-from-example "georgik/sdl:snow"

# Or clone and customize
git clone <repository>
cd snow
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.<your_board>" build flash monitor
```

---

## ✅ Verified Working Boards

The following boards have been **tested and confirmed working** with the BSP symbol conflict solution:

### 🚀 **M5 Atom S3** (Default)
```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" build flash monitor
```
- **Display**: 128×128 GC9A01 round display
- **PSRAM**: None (uses internal RAM optimization)
- **Setup**: No additional configuration required
- **Status**: ✅ **Fully Working** - Default board selection

### 📦 **ESP32-S3-BOX-3** 
```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp-box-3" build flash monitor
```
- **Display**: 320×240 ILI9341 TFT display
- **PSRAM**: 8MB OCTAL PSRAM
- **Setup**: No additional environment variables needed
- **Status**: ✅ **Fully Working** - BSP symbol conflicts resolved

### 🖥️ **M5Stack Tab5**
```bash
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_tab5" build flash monitor
```
- **Display**: 1280×720 IPS display via MIPI-DSI
- **PSRAM**: 32MB HEX PSRAM
- **Processor**: ESP32-P4 dual-core RISC-V
- **Touch**: GT911 touch controller
- **Status**: ✅ **Fully Working** - Complete 5-inch tablet experience

### 🔄 **Seamless Board Switching**

Switch between boards without conflicts:
```bash
# Switch to M5 Atom S3
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5_atom_s3" fullclean build flash monitor

# Switch to ESP32-S3-BOX-3  
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.esp-box-3" fullclean build flash monitor

# Switch to M5Stack Tab5
idf.py -D SDKCONFIG_DEFAULTS="sdkconfig.defaults.m5stack_tab5" fullclean build flash monitor
```

