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
struct NexusConfig;
struct NexusRenderer;
struct NexusWindow;
struct NexusInput;
struct NexusPhysics;
struct NexusAudio;

/* Use the pointers to structs in the engine implementation */

/**
 * Main engine context structure
 * Holds all subsystems and state
 */
typedef struct {
    /* Core systems */
    bool running;                      /* Engine running state */
    struct NexusConfig* config;        /* Configuration settings */
    struct NexusWindow* window;        /* Main window */
    
    /* ECS world */
    ecs_world_t* world;                /* Flecs ECS world */
    
    /* Subsystems */
    struct NexusRenderer* renderer;    /* Rendering system */
    struct NexusInput* input;          /* Input system */
    struct NexusPhysics* physics;      /* Physics system */
    struct NexusAudio* audio;          /* Audio system */
    
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

/* Performance metrics */
double nexus_engine_get_fps(void);
double nexus_engine_get_avg_frame_time(void);

/* ECS access */
ecs_world_t* nexus_engine_get_world(void);

/* Subsystem access */
struct NexusRenderer* nexus_engine_get_renderer(void);
struct NexusWindow* nexus_engine_get_window(void);
struct NexusInput* nexus_engine_get_input(void);
struct NexusPhysics* nexus_engine_get_physics(void);
struct NexusAudio* nexus_engine_get_audio(void);
struct NexusConfig* nexus_engine_get_config(void);

#endif /* NEXUS3D_ENGINE_H */
