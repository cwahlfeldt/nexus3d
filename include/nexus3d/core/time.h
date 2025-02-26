/**
 * Nexus3D Time Management
 * Handles timing, frame rates, and time scaling
 */

#ifndef NEXUS3D_TIME_H
#define NEXUS3D_TIME_H

#include <stdbool.h>
#include <stdint.h>

/**
 * Time management structure
 */
typedef struct {
    uint64_t last_time;            /* Last frame time */
    uint64_t current_time;         /* Current frame time */
    double delta_time;             /* Time between frames in seconds */
    double time_scale;             /* Time scale factor */
    double fixed_timestep;         /* Fixed timestep for physics/updates */
    double accumulated_time;       /* Accumulated time for fixed update */
    double fps;                    /* Current frames per second */
    double frame_time;             /* Current frame time in milliseconds */
    double average_frame_time;     /* Average frame time over window */
    uint64_t frame_count;          /* Total frames since startup */
    uint64_t performance_frequency; /* Performance counter frequency */
} NexusTime;

/* Time management functions */
NexusTime* nexus_time_create(void);
void nexus_time_destroy(NexusTime* time);
void nexus_time_update(NexusTime* time);
uint64_t nexus_time_get_ticks(void);
double nexus_time_get_seconds(void);
void nexus_time_set_time_scale(NexusTime* time, double scale);
double nexus_time_get_time_scale(const NexusTime* time);
double nexus_time_get_delta_time(const NexusTime* time);
double nexus_time_get_scaled_delta_time(const NexusTime* time);
double nexus_time_get_fps(const NexusTime* time);
uint64_t nexus_time_get_frame_count(const NexusTime* time);
void nexus_time_set_fixed_timestep(NexusTime* time, double timestep);
double nexus_time_get_fixed_timestep(const NexusTime* time);
bool nexus_time_should_update_fixed(NexusTime* time);

#endif /* NEXUS3D_TIME_H */
