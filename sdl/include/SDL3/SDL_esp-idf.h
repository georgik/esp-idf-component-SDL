/*
    ESP-IDF specific headers for enhanced functionality.
    
    Note: Hardware acceleration (PPA) is now integrated through SDL's
    standard render API. Use SDL_SetRenderScale() or 
    SDL_SetRenderLogicalPresentation() instead of direct hardware calls.
*/

#ifdef CONFIG_IDF_TARGET_ESP32P4
// Hardware acceleration is available through SDL's render system
// No direct PPA functions are exposed - use SDL_CreateRenderer() with
// SDL_RENDERER_ACCELERATED flag to enable hardware scaling

// Display rotation function for ESP32-P4 PPA
void set_display_rotation(int angle);
// Display scale factor function for ESP32-P4 PPA-aware framebuffer
void set_display_scale_factor(int factor);
#endif
