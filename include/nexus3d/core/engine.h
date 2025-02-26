/**
 * Nexus3D Engine Core
 * Central engine management system
 */

#ifndef NEXUS3D_ENGINE_H
#define NEXUS3D_ENGINE_H

#include <stdbool.h>
#include <SDL3/SDL.h>
#include <flecs.h>

/* Forward declarations */
typedef struct NexusConfig NexusConfig;
typedef struct NexusRenderer NexusRenderer;
typedef struct NexusWindow NexusWindow;
typedef struct NexusInput NexusInput;
typedef struct NexusPhysics NexusPhysics;
typedef struct NexusAudio NexusAudio;

/**
 * Main engine context structure
 * Holds all subsystems and state
 */
typedef struct {
    /* Core systems */
    bool running;                      /* Engine running state */
    NexusConfig* config;               /* Configuration settings */
    NexusWindow* window;               /* Main window */
    
    /* ECS world */
    ecs_world_t* world;                /* Flecs ECS world */
    
    /* Subsystems */
    NexusRenderer* renderer;           /* Rendering system */
    NexusInput* input;                 /* Input system */
    NexusPhysics* physics;             /* Physics system */
    NexusAudio* audio;                 /* Audio system */
    
    /* Timing */
    double delta_time;                 /* Time between frames in seconds */
    double time_scale;                 /* Time scale factor */
    uint64_t frame_count;              /* Total frames since startup */
    
    /* Performance metrics */
    double fps;                        /* Current frames per second */
    double avg_frame_time;             /* Average frame time in milliseconds */
} NexusEngine;

/* Global engine instance */
extern NexusEngine* g_engine;

/* Engine lifecycle functions */
bool nexus_engine_init(void);
void nexus_engine_shutdown(void);
void nexus_engine_update(void);
bool nexus_engine_is_running(void);
void nexus_engine_request_exit(void);

/* Time management */
double nexus_engine_get_delta_time(void);
void nexus_engine_set_time_scale(double scale);
double nexus_engine_get_time_scale(void);
uint64_t nexus_engine_get_frame_count(void);

/* ECS access */
ecs_world_t* nexus_engine_get_world(void);

/* Subsystem access */
NexusRenderer* nexus_engine_get_renderer(void);
NexusWindow* nexus_engine_get_window(void);
NexusInput* nexus_engine_get_input(void);
NexusPhysics* nexus_engine_get_physics(void);
NexusAudio* nexus_engine_get_audio(void);
NexusConfig* nexus_engine_get_config(void);

#endif /* NEXUS3D_ENGINE_H */
