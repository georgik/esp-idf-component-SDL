# ESP-BSP SDL Abstraction Layer

Board-agnostic display and touch abstraction for SDL on ESP32.

## Overview

This component provides a unified interface between SDL and ESP-BSP, enabling SDL applications to work across multiple ESP32 boards without modification. It integrates with the `bsp_selector` architecture from ESP-BSP for automatic board detection.

## Supported Boards

- **M5 Atom S3** - 128x128 GC9A01
- **ESP-Box-3** - 320x240 ILI9341, FT5x06 Touch
- **M5Stack Core S3** - 320x240 ILI9341, FT5x06 Touch
- **ESP32-P4 Function EV Board** - 1280x800 MIPI-DSI, GT1151 Touch
- **ESP32-S3-LCD-EV-Board** - 800x480 RGB, GT1151 Touch
- **M5Stack Tab5** - 720x1280 MIPI-DSI (landscape), GT911 Touch

## Architecture

```
┌─────────────────┐
│  SDL App        │
└─────────┬───────┘
          │
┌─────────▼───────┐
│ SDL (georgik/sdl)│
└─────────┬───────┘
          │
┌─────────▼───────┐
│ esp_sdl_bsp     │ ← This component (board-agnostic)
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

## Configuration

Board selection is handled via `bsp_selector` from ESP-BSP:

1. Run `idf.py menuconfig`
2. Navigate to `Component config` → `ESP-BSP` → `BSP board selection`
3. Select your board
4. Navigate to `Component config` → `ESP-BSP` → `Graphics library selection`
5. Select `SDL`

## API

```c
#include "esp_bsp_sdl.h"

// Initialize display (called automatically by SDL video driver)
esp_bsp_sdl_display_config_t config;
esp_lcd_panel_handle_t panel;
esp_lcd_panel_io_handle_t io;
esp_bsp_sdl_init(&config, &panel, &io);

// Control backlight
esp_bsp_sdl_backlight_on();
esp_bsp_sdl_backlight_off();

// Touch support (optional)
esp_bsp_sdl_touch_init();
esp_bsp_sdl_touch_info_t touch;
esp_bsp_sdl_touch_read(&touch);
```

## Component Structure

```
esp_sdl_bsp/
├── include/
│   └── esp_bsp_sdl.h         # Public API
├── src/
│   ├── esp_bsp_sdl_common.c   # Runtime board detection
│   └── boards/                # Board-specific implementations
│       ├── esp_bsp_sdl_m5_atom_s3.c
│       ├── esp_bsp_sdl_esp_box_3.c
│       ├── esp_bsp_sdl_m5stack_core_s3.c
│       ├── esp_bsp_sdl_esp32_p4_function_ev.c
│       ├── esp_bsp_sdl_esp32_s3_lcd_ev_board.c
│       └── esp_bsp_sdl_m5stack_tab5.c
├── CMakeLists.txt
├── Kconfig
└── idf_component.yml
```

## Board Detection

The component automatically detects the selected board at runtime using the `CONFIG_BSP_SELECT_*` variables set by bsp_selector. No application code changes are needed when switching between boards.

## License

MIT License and Apache License 2.0
