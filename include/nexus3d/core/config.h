/**
 * Nexus3D Configuration System
 * Handles engine configuration settings
 */

#ifndef NEXUS3D_CONFIG_H
#define NEXUS3D_CONFIG_H

#include <stdbool.h>
#include "nexus3d/core/window.h"

/* Graphics configuration */
typedef struct {
    bool enable_shadows;           /* Enable shadow rendering */
    bool enable_msaa;              /* Enable multi-sample anti-aliasing */
    int msaa_samples;              /* MSAA sample count (2, 4, 8, etc.) */
    bool enable_vsync;             /* Enable vertical sync */
    int max_fps;                   /* Maximum frames per second (0 = unlimited) */
    bool enable_hdr;               /* Enable high dynamic range */
} NexusGraphicsConfig;

/* Audio configuration */
typedef struct {
    bool enable_audio;             /* Enable audio */
    int max_channels;              /* Maximum audio channels */
    float master_volume;           /* Master volume (0.0 - 1.0) */
} NexusAudioConfig;

/* Physics configuration */
typedef struct {
    bool enable_physics;           /* Enable physics */
    float fixed_timestep;          /* Fixed timestep for physics updates (e.g., 1/60) */
    int max_substeps;              /* Maximum physics substeps per frame */
    float gravity;                 /* Gravity value (e.g., 9.81) */
} NexusPhysicsConfig;

/* Input configuration */
typedef struct {
    bool enable_gamepad;           /* Enable gamepad input */
    bool enable_keyboard;          /* Enable keyboard input */
    bool enable_mouse;             /* Enable mouse input */
} NexusInputConfig;

/* Debug configuration */
typedef struct {
    bool enable_debug_logging;     /* Enable debug logs */
    bool enable_physics_debug;     /* Enable physics debug rendering */
    bool enable_profiling;         /* Enable performance profiling */
} NexusDebugConfig;

/* Main configuration structure */
typedef struct NexusConfig {
    NexusWindowConfig window;      /* Window configuration */
    NexusGraphicsConfig graphics;  /* Graphics configuration */
    NexusAudioConfig audio;        /* Audio configuration */
    NexusPhysicsConfig physics;    /* Physics configuration */
    NexusInputConfig input;        /* Input configuration */
    NexusDebugConfig debug;        /* Debug configuration */
} NexusConfig;

/* Configuration functions */
NexusConfig* nexus_config_create(void);
void nexus_config_destroy(NexusConfig* config);
bool nexus_config_load_from_file(NexusConfig* config, const char* filepath);
bool nexus_config_save_to_file(const NexusConfig* config, const char* filepath);
void nexus_config_set_defaults(NexusConfig* config);

#endif /* NEXUS3D_CONFIG_H */
