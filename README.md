# Simple Direct Layer - SDL3 (ESP-IDF port)

** Work in progress **

This is component wrapper for SDL3 for ESP-IDF.

The HW layer is using [ESP-BSP](https://components.espressif.com/components?q=tags:bsp).

In the project CMakeLists.txt it's necessary to set `ENV{BUILD_BOARD}` to name of BSP.

```
set(ENV{BUILD_BOARD} ${BUILD_BOARD})
``` 

## Supported

- ESP32-S3-BOX-3
- M5Stack CoreS3
- ESP32-P4 Evaluation Kit 
  - with PPA for scaling the screen


