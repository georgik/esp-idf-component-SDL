# Mandelbrot Fractal Visualization - SDL3 ESP-IDF Example

**🌈 Beautiful Mandelbrot set visualization with dynamic colors and animation**

A stunning real-time implementation of the Mandelbrot fractal using SDL3 on ESP32 microcontrollers, featuring:

- **Universal Board Support** - Automatically adapts to any display resolution
- **4 Color Schemes** - Fire, Ocean, Sunset, and Psychedelic gradients
- **Progressive Rendering** - Smooth animation even on slower hardware
- **Auto-Zoom Journey** - Explores 7 mathematically interesting locations
- **HSV Color Space** - Rich, smooth color transitions

![Mandelbrot Fractal on ESP32](preview.gif)

## ✨ Features

### 🎨 Dynamic Color Schemes
1. **Fire** - Red → Orange → Yellow gradient
2. **Ocean** - Blue → Cyan → White gradient  
3. **Sunset** - Purple → Pink → Orange gradient
4. **Psychedelic** - Rapid rainbow cycling

### 🔍 Exploration Journey
Automatically visits famous Mandelbrot locations:
- **Main Bulb** (-0.7, 0.0) - Classic view
- **Spiral Region** (-0.8, 0.156) - Beautiful spirals
- **Top Detail** (-0.16, 1.04) - Intricate patterns
- **Right Side** (0.3, 0.5) - Edge details
- **Left Spike** (-1.25, 0.02) - Sharp formations
- **Feather Detail** (-0.235125, 0.827215) - Fine structures
- **Upper Connection** (-0.4, 0.6) - Bulb connections

### ⚡ Performance Optimizations
- **Progressive Rendering** - Renders 1/4 of screen per frame for 60 FPS
- **Adaptive Resolution** - Automatically optimizes for display size
- **Efficient Math** - Fast complex number calculations
- **Memory Efficient** - Minimal heap usage

## 🚀 Quick Start

### Using Component Registry
```bash
idf.py create-project-from-example "georgik/sdl:mandelbrot"
cd mandelbrot
idf.py build flash monitor
```

### Building for Specific Boards
```bash
# M5 Atom S3 (128×128, compact details)
idf.py -DBOARD=m5_atom_s3 build flash monitor

# M5Stack CoreS3 (320×240, full beauty)  
idf.py -DBOARD=m5stack_core_s3 build flash monitor

# ESP32-P4 Function EV (1280×800, incredible detail)
idf.py -DBOARD=esp32_p4_function_ev build flash monitor

# Universal DevKit with custom display
idf.py -DBOARD=esp_bsp_generic build
idf.py menuconfig  # Configure your display
idf.py build flash monitor
```

## 📱 Display Adaptation Examples

| Board | Resolution | Detail Level | Colors | Performance |
|-------|------------|--------------|---------|-------------|
| M5 Atom S3 | 128×128 | Good | Full HSV | 60 FPS |
| M5Stack CoreS3 | 320×240 | Excellent | Full HSV | 60 FPS |
| ESP32-S3-BOX-3 | 320×240 | Excellent | Full HSV | 60 FPS |
| ESP32-P4 Function EV | 1280×800 | Incredible | Full HSV | 15 FPS |

## 🎛️ Visual Indicators

### Zoom Level Bar (Top-Right)
- White bars showing current zoom level
- More bars = deeper zoom (up to 20 levels)

### Color Scheme Indicator (Bottom-Right)  
- **Red Square** - Fire scheme
- **Blue Square** - Ocean scheme
- **Purple Square** - Sunset scheme
- **Rainbow** - Psychedelic scheme

### Frame Counter (Bottom-Left)
- Gray dots showing animation progress
- Resets every 16 frames

## 🔬 Technical Details

### Mandelbrot Algorithm
```c
// For each pixel (cx, cy) in complex plane:
double x = 0.0, y = 0.0;
int iterations = 0;
while (iterations < MAX_ITERATIONS && (x*x + y*y) < 4.0) {
    double temp = x*x - y*y + cx;
    y = 2*x*y + cy;
    x = temp;
    iterations++;
}
```

### Color Generation
```c
// HSV to RGB conversion for smooth gradients
Color hsv_to_rgb(float hue, float saturation, float value)
// Multiple color schemes based on iteration count
Color mandelbrot_color(int iterations, double color_offset)
```

### Animation Timeline
- **Color Cycling**: Continuous gradient shift
- **Zoom Progress**: Every 100 frames zoom deeper  
- **Location Change**: Every 600 frames jump to new location
- **Full Cycle**: ~1 hour to visit all locations

## 💾 Memory Usage

- **Stack**: 32KB thread stack
- **Heap**: Minimal (only state variables)
- **ROM**: ~45KB code + mathematical constants
- **RAM**: Progressive rendering keeps memory usage constant

## 🎓 Mathematical Beauty

The Mandelbrot set demonstrates:
- **Fractal Geometry** - Infinite detail at all scales
- **Complex Dynamics** - Beautiful patterns from z² + c iteration
- **Computational Art** - Mathematics creating visual beauty  
- **Chaos Theory** - Order and chaos at fractal boundaries
- **Universal Patterns** - Self-similarity across scales

### Famous Properties
- **Self-Similar** - Patterns repeat at different scales
- **Infinitely Complex** - Zoom reveals endless detail
- **Mathematically Pure** - Simple formula, infinite complexity
- **Universally Beautiful** - Appeals across cultures

## 🚀 Extensions

Easy to enhance with:
- **Touch Interaction** - Tap to zoom into specific areas
- **Julia Sets** - Switch between Mandelbrot and Julia fractals
- **Custom Color Maps** - User-defined color schemes
- **Zoom Speed Control** - Variable zoom rates
- **Location Bookmarks** - Save favorite fractal locations
- **Export Images** - Save beautiful frames to SD card

## 📊 Performance Analysis

### Computational Complexity
- **Per Pixel**: O(MAX_ITERATIONS) worst case
- **Per Frame**: O(WIDTH × HEIGHT × MAX_ITERATIONS)
- **Memory**: O(1) - no framebuffers stored

### Optimization Techniques
- **Progressive Rendering** - Splits work across frames
- **Early Bailout** - Stops iteration when divergence detected
- **Integer Math** - Fixed-point arithmetic where possible
- **Strip Rendering** - Processes horizontal strips for cache efficiency

## 🎨 Color Science

### HSV Color Space Advantages
- **Perceptual Uniformity** - Colors transition smoothly
- **Easy Manipulation** - Hue shifts create natural gradients
- **Rich Saturation** - Vibrant colors throughout spectrum
- **Mathematical Beauty** - Color mathematics matches fractal mathematics

### Scheme Design
- **Fire**: Mimics thermal radiation spectrum
- **Ocean**: Evokes water depth and clarity
- **Sunset**: Natural atmospheric color progression
- **Psychedelic**: Maximum color diversity and contrast

## 🔧 Troubleshooting

### Slow Performance
- Progressive rendering maintains 60 FPS UI even with slow computation
- Larger displays automatically reduce detail for performance
- Monitor frame rates in serial output

### Color Issues
- Color schemes cycle automatically every ~2.5 minutes
- All schemes use full RGB range for maximum vibrancy
- Black areas indicate points inside the Mandelbrot set

### Memory Problems
- Very low memory usage due to progressive rendering
- No large buffers allocated
- PSRAM not required but improves responsiveness

---

Explore the infinite beauty of mathematical fractals, perfectly adapted to your ESP32 development board! 🌈✨

*"Mathematics is the art of giving the same name to different things."* - Henri Poincaré