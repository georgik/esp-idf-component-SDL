# Snow Example

SDL3 example with falling snow for ESP32.

## Create Project from Example Code

```shell
idf.py create-project-from-example "georgik/sdl:snow"
```

## Build

### Build for ESP32-S3-BOX-3

```shell
idf.py @boards/esp-box-3.cfg build flash monitor
```

### Build for ESP32-S3-BOX (prior Dec. 2023)

```shell
idf.py @boards/esp-box.cfg build flash monitor
```

### Build for ESP32-P4

```shell
idf.py @boards/esp32_p4_function_ev_board.cfg build flash monitor
```

### Build for M5Stack-CoreS3

```shell
idf.py @boards/m5stack_core_s3.cfg build flash monitor
```

### Build for ESP32-C6-DevKit or other custom DevKit

The configuration of this board is based on [ESP-BSP Generic](https://developer.espressif.com/blog/using-esp-bsp-with-devkits/) which allows configuration using menuconfig.

SPI Display configuration:

```ini
CONFIG_BSP_DISPLAY_ENABLED=y
CONFIG_BSP_DISPLAY_SCLK_GPIO=6
CONFIG_BSP_DISPLAY_MOSI_GPIO=7
CONFIG_BSP_DISPLAY_MISO_GPIO=-1
CONFIG_BSP_DISPLAY_CS_GPIO=20
CONFIG_BSP_DISPLAY_DC_GPIO=21
CONFIG_BSP_DISPLAY_RST_GPIO=3
CONFIG_BSP_DISPLAY_DRIVER_ILI9341=y
```

You can change the configuration by running:

```shell
idf.py @boards/esp32_c6_devkit.cfg menuconfig
```
