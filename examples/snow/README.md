# ESP-BSP SDL Universal Board Selection System

**🌟 The world's most comprehensive embedded SDL board abstraction system 🌟**

A revolutionary SDL3-based framework that provides **true hardware universality** - write once, run on any ESP32-based development board with automatic display resolution adaptation.

🎯 **One Application, Any Hardware** - The same SDL3 code seamlessly runs on displays from 128×128 to 1280×800 across both Xtensa and RISC-V architectures.

## ⭐ Featured Demo: Snow Simulation

Watch beautiful falling snow particles automatically adapt to your display resolution - from the compact M5 Atom S3 (128×128) to massive ESP32-P4 displays (up to 1280×800).

![Architecture Overview](docs/architecture.png)

## 🚀 Quick Start

### Single Command Board Selection

```bash
# M5 Atom S3 (128×128, No PSRAM) - Default, no environment variable needed
idf.py -DBOARD=m5_atom_s3 build flash monitor

# ESP32-S3-BOX-3 (320×240, OCTAL PSRAM) - Requires BUILD_FOR_BOX3=1
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 build flash monitor

# M5Stack CoreS3 (320×240, QUAD PSRAM) 
idf.py -DBOARD=m5stack_core_s3 build flash monitor

# ESP32-P4 RISC-V (up to 1280×800, 32MB PSRAM)
idf.py -DBOARD=esp32_p4_function_ev build flash monitor

# Universal: ANY ESP32 DevKit + configurable display
idf.py -DBOARD=esp_bsp_generic build
idf.py menuconfig  # Configure your hardware
idf.py build flash monitor

# DevKit testing (virtual display, no physical display needed)
idf.py -DBOARD=esp_bsp_devkit build flash monitor
```

### First Time Setup (Default Board)

```bash
# Uses M5 Atom S3 by default (no additional setup required)
idf.py build flash monitor

# For ESP32-S3-BOX-3, use environment variable:
BUILD_FOR_BOX3=1 idf.py build flash monitor
```

## 🎛️ Supported Boards

| Board | Architecture | Display | PSRAM | Key Features |
|-------|-------------|---------|-------|--------------|
| **`esp_bsp_generic`** | **Any ESP32** | **Configurable** | **Configurable** | **🔧 Universal DevKit + Display** |
| `m5_atom_s3` | ESP32-S3 (Xtensa) | 128×128 GC9A01 | None | ✅ **Working** - Default board, no setup needed |
| `m5stack_core_s3` | ESP32-S3 (Xtensa) | 320×240 ILI9341 | QUAD | Touch interface, large display |
| `esp32_p4_function_ev` | **ESP32-P4 (RISC-V)** | up to 1280×800 | 32MB HEX | PPA, H264, MIPI interfaces |
| `esp32_s3_lcd_ev` | ESP32-S3 (Xtensa) | Multiple LCD types | 16MB OCTAL | Flexible interface support |
| `esp32_s3_eye` | ESP32-S3 (Xtensa) | 240×240 circular | 8MB OCTAL | Camera + microphone |
| `esp32_s3_korvo_2` | ESP32-S3 (Xtensa) | LCD + Camera | OCTAL | Audio focus, dual microphones |
| `esp_bsp_devkit` | Any ESP32 | Virtual 240×320 | Configurable | LEDs/buttons, no display |
| `esp_box_3` | ESP32-S3 (Xtensa) | 320×240 ILI9341 | OCTAL | ✅ **Working** - Requires `BUILD_FOR_BOX3=1` |

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
idf.py -DBOARD=esp_bsp_generic build
idf.py menuconfig  
# Navigate to: Component config → BSP Generic
# Configure display, pins, resolution, touch, etc.
idf.py build flash monitor
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
idf.py -DBOARD=m5_atom_s3 build flash monitor
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 build flash monitor

# Switch to other boards
idf.py -DBOARD=esp32_p4_function_ev build flash monitor

# Clean switch if needed (recommended when switching between different boards)
idf.py -DBOARD=m5_atom_s3 fullclean build flash monitor
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 fullclean build flash monitor
```

### Custom Board Development
```bash
# Use ESP BSP Generic as starting point
idf.py -DBOARD=esp_bsp_generic build
idf.py menuconfig  # Customize hardware configuration
# Save configuration for reuse
cp sdkconfig sdkconfig.my_custom_board
```

### IDE Integration

**VS Code ESP-IDF Extension:**
```json
{
    "idf.cmakeCompilerArgs": ["-DBOARD=m5_atom_s3"]
}
```

**CLion:** Project Settings → CMake → CMake options: `-DBOARD=esp32_p4_function_ev`

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
rm -f .current_board sdkconfig
idf.py -DBOARD=<new_board> build
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
idf.py -DBOARD=m5_atom_s3 build  # Default - no env var needed
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 build  # Requires env var

# Test other boards
idf.py -DBOARD=esp32_p4_function_ev build
idf.py -DBOARD=m5stack_core_s3 build
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
idf.py -DBOARD=<your_board> build flash monitor
```

---

## ✅ Verified Working Boards

The following boards have been **tested and confirmed working** with the BSP symbol conflict solution:

### 🚀 **M5 Atom S3** (Default)
```bash
idf.py -DBOARD=m5_atom_s3 build flash monitor
```
- **Display**: 128×128 GC9A01 round display
- **PSRAM**: None (uses internal RAM optimization)
- **Setup**: No additional configuration required
- **Status**: ✅ **Fully Working** - Default board selection

### 📦 **ESP32-S3-BOX-3** 
```bash
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 build flash monitor
```
- **Display**: 320×240 ILI9341 TFT display
- **PSRAM**: 8MB OCTAL PSRAM
- **Setup**: Requires `BUILD_FOR_BOX3=1` environment variable
- **Status**: ✅ **Fully Working** - BSP symbol conflicts resolved

### 🔄 **Seamless Board Switching**

Switch between boards without conflicts:
```bash
# Switch to M5 Atom S3
idf.py -DBOARD=m5_atom_s3 fullclean build flash monitor

# Switch to ESP32-S3-BOX-3  
BUILD_FOR_BOX3=1 idf.py -DBOARD=esp_box_3 fullclean build flash monitor
```

**Key Benefits:**
- ✅ **No Symbol Conflicts** - Advanced conditional BSP loading
- ✅ **Automatic Reconfiguration** - Board switching detected and handled
- ✅ **Clean Builds** - Proper dependency resolution
- ✅ **Runtime Detection** - Correct board identification in logs

---

## 🏆 Achievement Unlocked

**You now have the most advanced embedded SDL system available** - capable of running identical code across radically different hardware configurations, from tiny monochrome displays to massive multimedia screens, spanning multiple processor architectures!

**🌟 Write once, run everywhere - embedded graphics redefined! 🌟**