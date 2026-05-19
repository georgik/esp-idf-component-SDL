cmake_minimum_required(VERSION 3.16)

# ESP-BSP SDL Abstraction Layer Component
# Board selection and BSP dependencies handled by bsp_selector

# Common source files
set(COMPONENT_SRCS "src/esp_bsp_sdl_common.c")

# Include board-specific sources based on bsp_selector selection
if(CONFIG_BSP_SELECT_M5_ATOM_S3)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_m5_atom_s3.c")
elseif(CONFIG_BSP_SELECT_ESP_BOX_3)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_esp_box_3.c")
elseif(CONFIG_BSP_SELECT_M5STACK_CORE_S3)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_m5stack_core_s3.c")
elseif(CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_esp32_p4_function_ev.c")
elseif(CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_esp32_s3_lcd_ev_board.c")
elseif(CONFIG_BSP_SELECT_M5STACK_TAB5)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_m5stack_tab5.c")
endif()

# Register component with all possible BSP dependencies
# Note: PRIV_REQUIRES must be explicit string for all boards that might be selected
# Managed components use espressif__ prefix, local BSP components use __ prefix
idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd
    PRIV_REQUIRES
        espressif__esp_lcd_touch
        espressif__m5_atom_s3_noglib
        espressif__esp-box-3_noglib
        espressif__m5stack_core_s3_noglib
        espressif__esp32_p4_function_ev_board_noglib
        espressif__esp32_s3_lcd_ev_board_noglib
)

# Board detection message
if(CONFIG_BSP_SELECT_M5_ATOM_S3)
    message(STATUS "ESP-BSP SDL: Building for M5 Atom S3")
elseif(CONFIG_BSP_SELECT_ESP_BOX_3)
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3-BOX-3")
elseif(CONFIG_BSP_SELECT_M5STACK_CORE_S3)
    message(STATUS "ESP-BSP SDL: Building for M5Stack CoreS3")
elseif(CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD)
    message(STATUS "ESP-BSP SDL: Building for ESP32-P4 Function EV Board")
elseif(CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD)
    message(STATUS "ESP-BSP SDL: Building for ESP32-S3-LCD-EV-Board")
elseif(CONFIG_BSP_SELECT_M5STACK_TAB5)
    message(STATUS "ESP-BSP SDL: Building for M5Stack Tab5")
endif()
