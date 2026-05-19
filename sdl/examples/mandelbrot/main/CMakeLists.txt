# Add the selected BSP component to PRIV_REQUIRES so BSP headers are available
set(MAIN_PRIV_REQUIRES "")

# Add the BSP component that was selected by bsp_selector
if(CONFIG_BSP_SELECT_M5_ATOM_S3)
    list(APPEND MAIN_PRIV_REQUIRES "espressif__m5_atom_s3_noglib")
elseif(CONFIG_BSP_SELECT_ESP_BOX_3)
    list(APPEND MAIN_PRIV_REQUIRES "espressif__esp-box-3_noglib")
elseif(CONFIG_BSP_SELECT_M5STACK_CORE_S3)
    list(APPEND MAIN_PRIV_REQUIRES "espressif__m5stack_core_s3_noglib")
elseif(CONFIG_BSP_SELECT_ESP32_P4_FUNCTION_EV_BOARD)
    list(APPEND MAIN_PRIV_REQUIRES "espressif__esp32_p4_function_ev_board_noglib")
elseif(CONFIG_BSP_SELECT_ESP32_S3_LCD_EV_BOARD)
    list(APPEND MAIN_PRIV_REQUIRES "espressif__esp32_s3_lcd_ev_board_noglib")
elseif(CONFIG_BSP_SELECT_M5STACK_TAB5)
    list(APPEND MAIN_PRIV_REQUIRES "espressif__m5stack_tab5_noglib")
endif()

idf_component_register(
    SRCS
        "mandelbrot.c"
        "board_init.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES ${MAIN_PRIV_REQUIRES}
)
