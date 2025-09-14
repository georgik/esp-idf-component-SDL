# Conway's Game of Life - SDL3 ESP-IDF Example

**🎮 Conway's Game of Life with age-based color visualization**

A beautiful implementation of Conway's Game of Life cellular automaton using SDL3 on ESP32 microcontrollers, featuring:

- **Universal Board Support** - Automatically adapts to any display resolution
- **Age-Based Coloring** - Living cells change color as they age (blue → cyan → white)
- **Intelligent Grid Scaling** - Optimal cell size calculation for each display
- **Classic Patterns** - Includes the famous glider pattern
- **Auto-Reset** - Generates new random patterns every 500 generations

![Conway's Game of Life on ESP32](preview.gif)

## ✨ Features

### 🎯 Universal Resolution Adaptation
- **M5 Atom S3**: 42×42 grid with 3×3 pixel cells (128×128 display)
- **M5Stack CoreS3**: 64×48 grid with 5×5 pixel cells (320×240 display)
- **ESP32-S3-BOX-3**: 64×48 grid with 5×5 pixel cells (320×240 display)
- **ESP32-P4**: Up to 106×80 grid with 12×12 pixel cells (1280×800 display)

### 🎨 Visual Features
- **Age-based coloring**: Older cells become brighter and more cyan
- **Generation counter**: Visual dots indicating current generation (mod 16)
- **Grid border**: Subtle border around the game area
- **Smooth animation**: 10 FPS for optimal viewing experience

### 🧬 Game Rules
Classic Conway's Game of Life rules with enhancements:
- **Birth**: Dead cell with exactly 3 neighbors becomes alive
- **Survival**: Live cell with 2 or 3 neighbors survives (and ages)
- **Death**: Live cell with <2 or >3 neighbors dies
- **Aging**: Living cells accumulate age for color visualization

## 🚀 Quick Start

### Using Component Registry
```bash
idf.py create-project-from-example "georgik/sdl:conway"
cd conway
idf.py build flash monitor
```

### Building for Specific Boards
```bash
# M5 Atom S3 (128×128, compact grid)
idf.py -DBOARD=m5_atom_s3 build flash monitor

# M5Stack CoreS3 (320×240, standard grid)
idf.py -DBOARD=m5stack_core_s3 build flash monitor

# ESP32-P4 Function EV (large high-resolution display)
idf.py -DBOARD=esp32_p4_function_ev build flash monitor

# Universal DevKit with custom display
idf.py -DBOARD=esp_bsp_generic build
idf.py menuconfig  # Configure your display
idf.py build flash monitor
```

## 📱 Display Adaptation Examples

| Board | Resolution | Grid | Cell Size | Colors |
|-------|------------|------|-----------|---------|
| M5 Atom S3 | 128×128 | 42×42 | 3×3 px | Age-based gradient |
| M5Stack CoreS3 | 320×240 | 64×48 | 5×5 px | Full color range |
| ESP32-S3-BOX-3 | 320×240 | 64×48 | 5×5 px | Full color range |
| ESP32-P4 Function EV | 1280×800 | 106×80 | 12×12 px | Rich detail |

## 🎛️ Configuration

The example automatically calculates optimal grid dimensions based on display resolution:

```c
// Grid size limits
#define MIN_GRID_WIDTH 32    // Minimum cells horizontally
#define MIN_GRID_HEIGHT 24   // Minimum cells vertically  
#define MAX_GRID_WIDTH 128   // Maximum cells horizontally
#define MAX_GRID_HEIGHT 96   // Maximum cells vertically
#define CELL_MIN_SIZE 3      // Minimum cell size in pixels
#define CELL_MAX_SIZE 12     // Maximum cell size in pixels
```

## 💾 Memory Usage

- **Stack**: 40KB thread stack (larger than snow example for game logic)
- **Heap**: Dynamic allocation for grid arrays (~6-24KB depending on resolution)
- **PSRAM**: Utilized on supported boards for optimal performance

## 🔬 Technical Details

### Algorithm Implementation
- **Double Buffering**: Uses current/next grid buffers for smooth updates
- **Toroidal Topology**: Grid wraps around edges (no boundaries)
- **Age Tracking**: Each cell stores age value (0-255) for visual effects
- **Memory Efficient**: Dynamic grid allocation based on display size

### Color Mapping
```c
// Age-based color gradient
uint8_t r = (255 * age) / max_age;  // Red component increases with age
uint8_t g = (255 * age) / max_age;  // Green component increases with age
uint8_t b = 255;                    // Blue component stays high
```

### Pattern Generation
- **Random Initialization**: ~33% initial alive probability
- **Glider Injection**: Classic glider pattern added at center
- **Auto-Reset**: New random pattern every 500 generations

## 🎓 Educational Value

Perfect for demonstrating:
- **Cellular Automata**: Classic Conway's Game of Life rules
- **Emergence**: Complex patterns from simple rules
- **Universal Computation**: Turing-complete system
- **Graphics Programming**: Real-time rendering and color gradients
- **Embedded Systems**: Efficient algorithms for resource-constrained devices

## 🚀 Extensions

Easy to extend with:
- **Touch Interaction**: Add/remove cells by touching screen
- **Pattern Library**: Load famous patterns (gliders, oscillators, etc.)
- **Speed Control**: Variable animation speed
- **Different Rules**: B3/S23 variations
- **Sound**: Audio feedback for birth/death events

## 📊 Performance

- **Frame Rate**: 10 FPS (100ms per generation)
- **CPU Usage**: Moderate (O(n²) per generation)
- **Memory**: Scales with display resolution
- **Power**: Efficient for continuous operation

## 🔧 Troubleshooting

### Grid Not Visible
- Check display initialization in serial output
- Verify board selection matches your hardware
- Ensure PSRAM is configured correctly for large grids

### Performance Issues
- Large displays may need slower update rates
- Consider reducing MAX_GRID_WIDTH/HEIGHT for very high resolutions
- Monitor heap usage with `esp_get_free_heap_size()`

---

Experience the beauty of emergence and complexity arising from simple rules, perfectly adapted to your ESP32 development board! 🎮✨