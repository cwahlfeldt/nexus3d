/**
 * Nexus3D Window Implementation
 * Handles window creation and management using SDL3
 */

#include "nexus3d/core/window.h"
#include "nexus3d/utils/logger.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a window
 */
NexusWindow* nexus_window_create(const NexusWindowConfig* config) {
    /* Allocate window structure */
    NexusWindow* window = (NexusWindow*)malloc(sizeof(NexusWindow));
    if (window == NULL) {
        printf("Failed to allocate memory for window!\n");
        return NULL;
    }

    /* Initialize window structure */
    memset(window, 0, sizeof(NexusWindow));
    
    /* Copy configuration */
    if (config != NULL) {
        window->config = *config;
    } else {
        /* Default configuration */
        window->config.title = "Nexus3D";
        window->config.width = 1280;
        window->config.height = 720;
        window->config.fullscreen = false;
        window->config.resizable = true;
        window->config.vsync = true;
        window->config.display_index = 0;
    }
    
    /* Set window flags */
    SDL_WindowFlags flags = SDL_WINDOW_HIGH_PIXEL_DENSITY;
    
    if (window->config.fullscreen) {
        flags |= SDL_WINDOW_FULLSCREEN;
    }
    
    if (window->config.resizable) {
        flags |= SDL_WINDOW_RESIZABLE;
    }
    
    /* Create SDL window */
    window->sdl_window = SDL_CreateWindow(
        window->config.title, 
        window->config.width, 
        window->config.height, 
        flags
    );
    
    if (window->sdl_window == NULL) {
        printf("Failed to create SDL window: %s\n", SDL_GetError());
        
        /* Try to create a hidden window as fallback for headless environments */
        printf("Attempting to create a hidden window...\n");
        window->sdl_window = SDL_CreateWindow(
            window->config.title,
            window->config.width,
            window->config.height,
            flags | SDL_WINDOW_HIDDEN
        );
        
        if (window->sdl_window == NULL) {
            printf("Failed to create hidden window: %s\n", SDL_GetError());
            free(window);
            return NULL;
        }
        
        printf("Created hidden window for headless environment\n");
    }
    
    /* Store actual window size */
    SDL_GetWindowSize(window->sdl_window, &window->actual_width, &window->actual_height);
    
    /* Set initial state */
    window->should_close = false;
    
    printf("Window created: %dx%d\n", window->actual_width, window->actual_height);
    
    return window;
}

/**
 * Destroy a window
 */
void nexus_window_destroy(NexusWindow* window) {
    if (window == NULL) {
        return;
    }
    
    /* Destroy SDL window */
    if (window->sdl_window != NULL) {
        SDL_DestroyWindow(window->sdl_window);
        window->sdl_window = NULL;
    }
    
    /* Free window structure */
    free(window);
}

/**
 * Update a window
 */
void nexus_window_update(NexusWindow* window) {
    if (window == NULL || window->sdl_window == NULL) {
        return;
    }
    
    /* Update actual window size */
    SDL_GetWindowSize(window->sdl_window, &window->actual_width, &window->actual_height);
}

/**
 * Close a window
 */
void nexus_window_close(NexusWindow* window) {
    if (window == NULL) {
        return;
    }
    
    window->should_close = true;
}

/**
 * Check if window should close
 */
bool nexus_window_should_close(const NexusWindow* window) {
    if (window == NULL) {
        return true;
    }
    
    return window->should_close;
}

/**
 * Set window title
 */
void nexus_window_set_title(NexusWindow* window, const char* title) {
    if (window == NULL || window->sdl_window == NULL || title == NULL) {
        return;
    }
    
    SDL_SetWindowTitle(window->sdl_window, title);
    window->config.title = title; /* Note: This assumes title string stays valid */
}

/**
 * Set window size
 */
void nexus_window_set_size(NexusWindow* window, int width, int height) {
    if (window == NULL || window->sdl_window == NULL) {
        return;
    }
    
    SDL_SetWindowSize(window->sdl_window, width, height);
    window->config.width = width;
    window->config.height = height;
    
    /* Update actual window size */
    SDL_GetWindowSize(window->sdl_window, &window->actual_width, &window->actual_height);
}

/**
 * Get window size
 */
void nexus_window_get_size(const NexusWindow* window, int* width, int* height) {
    if (window == NULL) {
        if (width) *width = 0;
        if (height) *height = 0;
        return;
    }
    
    if (width) *width = window->actual_width;
    if (height) *height = window->actual_height;
}

/**
 * Set window fullscreen
 */
void nexus_window_set_fullscreen(NexusWindow* window, bool fullscreen) {
    if (window == NULL || window->sdl_window == NULL) {
        return;
    }
    
    SDL_SetWindowFullscreen(window->sdl_window, fullscreen);
    window->config.fullscreen = fullscreen;
}

/**
 * Check if window is fullscreen
 */
bool nexus_window_is_fullscreen(const NexusWindow* window) {
    if (window == NULL) {
        return false;
    }
    
    return window->config.fullscreen;
}

/**
 * Set VSync
 */
void nexus_window_set_vsync(NexusWindow* window, bool vsync) {
    if (window == NULL) {
        return;
    }
    
    window->config.vsync = vsync;
    /* Actual VSync application happens in the renderer */
}

/**
 * Get VSync
 */
bool nexus_window_get_vsync(const NexusWindow* window) {
    if (window == NULL) {
        return false;
    }
    
    return window->config.vsync;
}

/**
 * Get SDL window handle
 */
SDL_Window* nexus_window_get_native_handle(const NexusWindow* window) {
    if (window == NULL) {
        return NULL;
    }
    
    return window->sdl_window;
}
