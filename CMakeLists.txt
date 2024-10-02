set(extra_reqs "")
if(IDF_TARGET STREQUAL "esp32p4")
    list(APPEND extra_reqs esp_driver_ppa)
endif()

idf_component_register(SRCS
                        "SDL/src/SDL.c"
                        "SDL/src/atomic/SDL_atomic.c"
                        "SDL/src/atomic/SDL_spinlock.c"
                        "SDL/src/events/SDL_displayevents.c"
                        "SDL/src/events/SDL_events.c"
                        "SDL/src/events/SDL_keyboard.c"
                        "SDL/src/events/SDL_keymap.c"
                        "SDL/src/events/SDL_mouse.c"
                        "SDL/src/events/SDL_pen.c"
                        "SDL/src/events/SDL_quit.c"
                        "SDL/src/events/SDL_touch.c"
                        "SDL/src/events/SDL_windowevents.c"
                        "SDL/src/filesystem/SDL_filesystem.c"
                        "SDL/src/file/SDL_iostream.c"
                        "SDL/src/stdlib/SDL_iconv.c"
                        "SDL/src/stdlib/SDL_getenv.c"
                        "SDL/src/stdlib/SDL_malloc.c"
                        "SDL/src/stdlib/SDL_memset.c"
                        "SDL/src/stdlib/SDL_stdlib.c"
                        "SDL/src/stdlib/SDL_string.c"
                        "SDL/src/timer/SDL_timer.c"

                        # pthread implementation from SDL
                        "SDL/src/thread/SDL_thread.c"
                        "SDL/src/thread/pthread/SDL_syscond.c"
                        "SDL/src/thread/pthread/SDL_sysmutex.c"
                        "SDL/src/thread/pthread/SDL_sysrwlock.c"
                        "SDL/src/thread/pthread/SDL_syssem.c"
                        "SDL/src/thread/pthread/SDL_systhread.c"
                        "SDL/src/thread/pthread/SDL_systls.c"

                        # ESP-IDF workaround to add pthread functions missing in ESP-IDF
                        "src/thread/pthread/pthread_espidf.c"

                        # Alternative implementation to pthread using ESP-IDF tasks
                        #"src/thread/esp-idf/SDL_syscond.c"
                        #"src/thread/esp-idf/SDL_sysmutex.c"
                        #"src/thread/esp-idf/SDL_sysrwlock.c"
                        #"src/thread/esp-idf/SDL_syssem.c"
                        #"src/thread/esp-idf/SDL_systhread.c"
                        #"src/thread/esp-idf/SDL_systls.c"

                        "SDL/src/cpuinfo/SDL_cpuinfo.c"
                        "SDL/src/SDL_assert.c"
                        "SDL/src/SDL_error.c"
                        "SDL/src/SDL_hashtable.c"
                        "SDL/src/SDL_hints.c"
                        "SDL/src/SDL_log.c"

                        "SDL/src/render/SDL_render.c"
                        "SDL/src/render/software/SDL_blendfillrect.c"
                        "SDL/src/render/software/SDL_blendline.c"
                        "SDL/src/render/software/SDL_blendpoint.c"
                        "SDL/src/render/software/SDL_drawline.c"
                        "SDL/src/render/software/SDL_drawpoint.c"
                        "SDL/src/render/software/SDL_rotate.c"
                        "SDL/src/render/software/SDL_render_sw.c"
                        "SDL/src/render/software/SDL_triangle.c"
                        "SDL/src/render/SDL_yuv_sw.c"

                        "SDL/src/video/SDL_RLEaccel.c"
                        "SDL/src/video/SDL_blit.c"
                        "SDL/src/video/SDL_blit_0.c"
                        "SDL/src/video/SDL_blit_1.c"
                        "SDL/src/video/SDL_blit_A.c"
                        "SDL/src/video/SDL_blit_N.c"
                        "SDL/src/video/SDL_blit_auto.c"
                        "SDL/src/video/SDL_blit_copy.c"
                        "SDL/src/video/SDL_blit_slow.c"
                        "SDL/src/video/SDL_bmp.c"
                        "SDL/src/video/SDL_fillrect.c"
                        "SDL/src/video/SDL_pixels.c"
                        "SDL/src/video/SDL_rect.c"
                        "SDL/src/video/SDL_stretch.c"
                        "SDL/src/video/SDL_surface.c"
                        "SDL/src/video/SDL_video.c"
                        "SDL/src/video/SDL_yuv.c"
                        "SDL/src/video/yuv2rgb/yuv_rgb_std.c"

                        # Timer
                        "src/timer/esp-idf/SDL_systimer.c"

                        # Render: ESP-IDF HW Accelerated renderers
                        #"SDL/src/render/esp-idf/SDL_espidfppa.c"

                        # Video: ESP-IDF BSP based driver
                        "src/video/esp-idf/SDL_espidfevents.c"
                        "src/video/esp-idf/SDL_espidfframebuffer.c"
                        "src/video/esp-idf/SDL_espidfvideo.c"

                        # Touch: ESP-IDF
                        "src/video/esp-idf/SDL_espidftouch.c"

                        "SDL/src/SDL_properties.c"
                        "SDL/src/SDL_utils.c"
                    INCLUDE_DIRS
                        "include"
                        "SDL/include"
                        "SDL/src"
                        "SDL/include/SDL3"
                    REQUIRES
                        $ENV{BUILD_BOARD}
                    PRIV_REQUIRES
                        ${extra_reqs}
)

