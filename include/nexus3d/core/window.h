/**
 * Nexus3D Window Management
 * Handles window creation and management using SDL3
 */

#ifndef NEXUS3D_WINDOW_H
#define NEXUS3D_WINDOW_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * Window configuration structure
 */
typedef struct {
    const char* title;         /* Window title */
    int width;                 /* Window width in pixels */
    int height;                /* Window height in pixels */
    bool fullscreen;           /* Fullscreen flag */
    bool resizable;            /* Resizable flag */
    bool vsync;                /* Vertical sync flag */
    int display_index;         /* Display index */
} NexusWindowConfig;

/**
 * Window structure
 */
typedef struct NexusWindow {
    SDL_Window* sdl_window;    /* SDL window handle */
    NexusWindowConfig config;  /* Window configuration */
    bool should_close;         /* Close flag */
    int actual_width;          /* Actual window width (may differ from config in some cases) */
    int actual_height;         /* Actual window height (may differ from config in some cases) */
} NexusWindow;

/* Window functions */
NexusWindow* nexus_window_create(const NexusWindowConfig* config);
void nexus_window_destroy(NexusWindow* window);
void nexus_window_update(NexusWindow* window);
void nexus_window_close(NexusWindow* window);
bool nexus_window_should_close(const NexusWindow* window);
void nexus_window_set_title(NexusWindow* window, const char* title);
void nexus_window_set_size(NexusWindow* window, int width, int height);
void nexus_window_get_size(const NexusWindow* window, int* width, int* height);
void nexus_window_set_fullscreen(NexusWindow* window, bool fullscreen);
bool nexus_window_is_fullscreen(const NexusWindow* window);
void nexus_window_set_vsync(NexusWindow* window, bool vsync);
bool nexus_window_get_vsync(const NexusWindow* window);
SDL_Window* nexus_window_get_native_handle(const NexusWindow* window);

#endif /* NEXUS3D_WINDOW_H */
