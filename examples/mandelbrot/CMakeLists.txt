cmake_minimum_required(VERSION 3.16)

# ESP-BSP SDL Universal Board Selection
# Usage: idf.py -DBOARD=m5_atom_s3 build
# Available: m5_atom_s3, m5stack_core_s3, esp_box_3
# Default: esp_box_3 (if not specified)
#
# Examples:
#   idf.py build                           # Uses esp_box_3 (default)
#   idf.py -DBOARD=m5_atom_s3 build        # Uses M5 Atom S3
#   idf.py -DBOARD=m5stack_core_s3 build   # Uses M5Stack CoreS3
#
# To switch boards: idf.py -DBOARD=<new_board> reconfigure

# Set default board if not specified and make it a cache variable
if(NOT DEFINED BOARD)
    set(BOARD "esp_box_3")
    set(show_board_help TRUE)
else()
    set(show_board_help FALSE)
endif()

# Make BOARD variable available globally
set(BOARD ${BOARD} CACHE STRING "Target board selection" FORCE)

# Show board selection info
if(show_board_help)
    message(STATUS "===============================================")
    message(STATUS "ESP-BSP SDL Universal Board Selection")
    message(STATUS "===============================================")
    message(STATUS "No board specified, defaulting to: ${BOARD}")
    message(STATUS "")
    message(STATUS "To specify a board, use:")
    message(STATUS "  idf.py -DBOARD=<board_name> build")
    message(STATUS "")
    message(STATUS "Available boards:")
    message(STATUS "  m5_atom_s3      - M5 Atom S3 (128x128, no PSRAM)")
    message(STATUS "  m5stack_core_s3 - M5Stack CoreS3 (320x240, QUAD PSRAM)")
    message(STATUS "  esp_box_3       - ESP32-S3-BOX-3 (320x240, OCTAL PSRAM)")
    message(STATUS "")
    message(STATUS "To switch boards later: idf.py -DBOARD=<board> reconfigure")
    message(STATUS "===============================================")
else()
    message(STATUS "ESP-BSP SDL: Building for board: ${BOARD}")
endif()

# Apply board-specific configuration by copying to sdkconfig.defaults
set(BOARD_DEFAULTS_FILE "${CMAKE_SOURCE_DIR}/sdkconfig.defaults.${BOARD}")
if(EXISTS ${BOARD_DEFAULTS_FILE})
    message(STATUS "Loading board-specific defaults: sdkconfig.defaults.${BOARD}")
    
    # Set IDF_TARGET based on board selection before component resolution
    if(BOARD STREQUAL "esp32_p4_function_ev")
        set(IDF_TARGET "esp32p4")
    elseif(BOARD MATCHES "^(m5_atom_s3|m5stack_core_s3|esp32_s3_lcd_ev|esp32_s3_eye|esp32_s3_korvo_2|esp_box_3|esp_bsp_generic|esp_bsp_devkit)$")
        set(IDF_TARGET "esp32s3")
    else()
        set(IDF_TARGET "esp32s3")  # Default to ESP32-S3 for most boards
    endif()
    message(STATUS "Setting IDF target: ${IDF_TARGET} for board: ${BOARD}")
    
    # Copy board-specific defaults to main sdkconfig.defaults
    configure_file(${BOARD_DEFAULTS_FILE} ${CMAKE_SOURCE_DIR}/sdkconfig.defaults COPYONLY)
    
    # Check if we need to force clean the sdkconfig (board switch detection)
    set(CURRENT_BOARD_FILE "${CMAKE_SOURCE_DIR}/.current_board")
    set(NEED_CLEAN_CONFIG FALSE)
    
    # Read the previously selected board
    if(EXISTS ${CURRENT_BOARD_FILE})
        file(READ ${CURRENT_BOARD_FILE} PREVIOUS_BOARD)
        string(STRIP "${PREVIOUS_BOARD}" PREVIOUS_BOARD)
        if(NOT "${PREVIOUS_BOARD}" STREQUAL "${BOARD}")
            set(NEED_CLEAN_CONFIG TRUE)
            message(STATUS "Board switch detected: ${PREVIOUS_BOARD} -> ${BOARD}")
        endif()
    else()
        set(NEED_CLEAN_CONFIG TRUE)
        message(STATUS "First-time board configuration: ${BOARD}")
    endif()
    
    # Clean sdkconfig if board changed to force regeneration from defaults
    if(NEED_CLEAN_CONFIG)
        message(STATUS "Cleaning sdkconfig to apply new board configuration...")
        file(REMOVE "${CMAKE_SOURCE_DIR}/sdkconfig")
        file(WRITE ${CURRENT_BOARD_FILE} "${BOARD}")
    endif()
    
else()
    message(FATAL_ERROR "===============================================")
    message(FATAL_ERROR "Board defaults file not found: ${BOARD_DEFAULTS_FILE}")
    message(FATAL_ERROR "")
    message(FATAL_ERROR "Available boards: see BOARDS.md for full list")
    message(FATAL_ERROR "")
    message(FATAL_ERROR "Usage: idf.py -DBOARD=<board_name> build")
    message(FATAL_ERROR "===============================================")
endif()

set(COMPONENTS
    main
    esp_bsp_sdl
    georgik__sdl
) # "Trim" the build. Include the minimal set of components; main and anything it depends on.

include($ENV{IDF_PATH}/tools/cmake/project.cmake)

project(mandelbrot)

# get_filename_component(configName "${CMAKE_BINARY_DIR}" NAME)
# list(APPEND EXTRA_COMPONENT_DIRS "${CMAKE_SOURCE_DIR}/components/esp_littlefs")
# littlefs_create_partition_image(assets assets FLASH_IN_PROJECT)
