/**
 * Nexus3D Time Management Implementation
 * Handles timing, frame rates, and time scaling
 */

#include "nexus3d/core/time.h"
#include <SDL3/SDL.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a time management structure
 */
NexusTime* nexus_time_create(void) {
    /* Allocate time structure */
    NexusTime* time = (NexusTime*)malloc(sizeof(NexusTime));
    if (time == NULL) {
        printf("Failed to allocate memory for time management!\n");
        return NULL;
    }

    /* Initialize time structure */
    memset(time, 0, sizeof(NexusTime));
    
    /* Set default values */
    time->time_scale = 1.0;
    time->fixed_timestep = 1.0 / 60.0; /* 60 Hz */
    time->performance_frequency = SDL_GetPerformanceFrequency();
    time->current_time = SDL_GetPerformanceCounter();
    time->last_time = time->current_time;
    
    return time;
}

/**
 * Destroy a time management structure
 */
void nexus_time_destroy(NexusTime* time) {
    if (time == NULL) {
        return;
    }
    
    /* Free time structure */
    free(time);
}

/**
 * Update time management
 */
void nexus_time_update(NexusTime* time) {
    if (time == NULL) {
        return;
    }
    
    /* Update time */
    time->last_time = time->current_time;
    time->current_time = SDL_GetPerformanceCounter();
    
    /* Calculate delta time in seconds */
    time->delta_time = (double)(time->current_time - time->last_time) / (double)time->performance_frequency;
    
    /* Calculate frame time in milliseconds */
    time->frame_time = time->delta_time * 1000.0;
    
    /* Apply low-pass filter to average frame time */
    const double alpha = 0.2; /* Smoothing factor */
    time->average_frame_time = alpha * time->frame_time + (1.0 - alpha) * time->average_frame_time;
    
    /* Calculate FPS */
    if (time->delta_time > 0.0) {
        time->fps = 1.0 / time->delta_time;
    }
    
    /* Accumulate time for fixed update */
    time->accumulated_time += time->delta_time;
    
    /* Increment frame counter */
    time->frame_count++;
}

/**
 * Get current ticks
 */
uint64_t nexus_time_get_ticks(void) {
    return SDL_GetTicks();
}

/**
 * Get current time in seconds
 */
double nexus_time_get_seconds(void) {
    return (double)SDL_GetTicks() / 1000.0;
}

/**
 * Set time scale
 */
void nexus_time_set_time_scale(NexusTime* time, double scale) {
    if (time == NULL) {
        return;
    }
    
    time->time_scale = scale;
}

/**
 * Get time scale
 */
double nexus_time_get_time_scale(const NexusTime* time) {
    if (time == NULL) {
        return 1.0;
    }
    
    return time->time_scale;
}

/**
 * Get delta time
 */
double nexus_time_get_delta_time(const NexusTime* time) {
    if (time == NULL) {
        return 0.0;
    }
    
    return time->delta_time;
}

/**
 * Get scaled delta time
 */
double nexus_time_get_scaled_delta_time(const NexusTime* time) {
    if (time == NULL) {
        return 0.0;
    }
    
    return time->delta_time * time->time_scale;
}

/**
 * Get frames per second
 */
double nexus_time_get_fps(const NexusTime* time) {
    if (time == NULL) {
        return 0.0;
    }
    
    return time->fps;
}

/**
 * Get frame count
 */
uint64_t nexus_time_get_frame_count(const NexusTime* time) {
    if (time == NULL) {
        return 0;
    }
    
    return time->frame_count;
}

/**
 * Set fixed timestep
 */
void nexus_time_set_fixed_timestep(NexusTime* time, double timestep) {
    if (time == NULL) {
        return;
    }
    
    time->fixed_timestep = timestep;
}

/**
 * Get fixed timestep
 */
double nexus_time_get_fixed_timestep(const NexusTime* time) {
    if (time == NULL) {
        return 1.0 / 60.0; /* Default 60 Hz */
    }
    
    return time->fixed_timestep;
}

/**
 * Check if fixed update should run
 */
bool nexus_time_should_update_fixed(NexusTime* time) {
    if (time == NULL || time->fixed_timestep <= 0.0) {
        return false;
    }
    
    if (time->accumulated_time >= time->fixed_timestep) {
        time->accumulated_time -= time->fixed_timestep;
        return true;
    }
    
    return false;
}
