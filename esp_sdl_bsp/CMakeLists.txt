cmake_minimum_required(VERSION 3.16)

# ESP-BSP SDL Abstraction Layer Component for M5Stack Tab5

set(COMPONENT_SRCS "src/esp_bsp_sdl_common.c")

if(CONFIG_BSP_SELECT_M5STACK_TAB5)
    list(APPEND COMPONENT_SRCS "src/boards/esp_bsp_sdl_m5stack_tab5.c")
    message(STATUS "ESP-BSP SDL: Building for M5Stack Tab5")
endif()

idf_component_register(
    SRCS ${COMPONENT_SRCS}
    INCLUDE_DIRS "include"
    REQUIRES esp_lcd
    PRIV_REQUIRES espressif__esp_lcd_touch;m5stack_tab5
)
