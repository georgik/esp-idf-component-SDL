cmake_minimum_required(VERSION 3.16)

# Add esp_sdl_bsp component from repository root and local components
get_filename_component(PROJECT_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../../../" ABSOLUTE)
set(EXTRA_COMPONENT_DIRS "${PROJECT_ROOT}/esp_sdl_bsp" "${CMAKE_CURRENT_SOURCE_DIR}/components" "${CMAKE_CURRENT_SOURCE_DIR}/main/components")

# ESP-BSP SDL Universal Board Selection
# Board selection is handled through bsp_selector (ESP-BSP)
# Navigate to "Component config" -> "ESP-BSP" -> "BSP board selection"

# Board configuration detection for informational messages
if(CONFIG_BSP_SELECT_M5STACK_TAB5)
    message(STATUS "Detected M5Stack Tab5 (ESP32-P4) configuration")
elseif(CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD)
    message(STATUS "Detected ESP32-P4 Function EV Board configuration")
elseif(CONFIG_BSP_SELECT_ESP_BOX_3)
    message(STATUS "Detected ESP32-S3-BOX-3 configuration")
elseif(CONFIG_BSP_SELECT_M5STACK_CORE_S3)
    message(STATUS "Detected M5Stack Core S3 configuration")
elseif(CONFIG_BSP_SELECT_M5_ATOM_S3)
    message(STATUS "Detected M5 Atom S3 configuration")
elseif(CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD)
    message(STATUS "Detected ESP32-S3-LCD-EV-Board configuration")
else()
    message(STATUS "Board configuration will be detected at runtime")
endif()

set(COMPONENTS
    main
) # "Trim" the build. Include the minimal set of components; main and anything it depends on.

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(snow)
