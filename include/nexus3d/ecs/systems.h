/**
 * Nexus3D ECS Systems
 * System definitions for the Entity Component System
 */

#ifndef NEXUS3D_SYSTEMS_H
#define NEXUS3D_SYSTEMS_H

#include <flecs.h>
#include "nexus3d/core/engine.h"

/**
 * Core system phases
 */
extern ecs_entity_t NexusPhaseInit;        /* Initialization phase */
extern ecs_entity_t NexusPhaseInput;       /* Input processing phase */
extern ecs_entity_t NexusPhasePhysics;     /* Physics update phase */
extern ecs_entity_t NexusPhaseLogic;       /* Game logic phase */
extern ecs_entity_t NexusPhaseAnimation;   /* Animation phase */
extern ecs_entity_t NexusPhasePreRender;   /* Pre-render phase */
extern ecs_entity_t NexusPhaseRender;      /* Render phase */
extern ecs_entity_t NexusPhasePostRender;  /* Post-render phase */
extern ecs_entity_t NexusPhaseCleanup;     /* Cleanup phase */

/**
 * Core system registration
 * Registers all built-in systems with the ECS world
 * @return true if successful, false if there was an error
 */
bool nexus_ecs_register_systems(ecs_world_t* world);

/**
 * Transform system
 * Updates transform matrices based on position, rotation, and scale components
 */
void nexus_transform_system(ecs_iter_t* it);

/**
 * Hierarchy system
 * Updates world transforms based on parent-child relationships
 */
void nexus_hierarchy_system(ecs_iter_t* it);

/**
 * Physics system
 * Updates physics simulation
 */
void nexus_physics_system(ecs_iter_t* it);

/**
 * Renderer system
 * Handles rendering of all entities with render components
 */
void nexus_renderer_system(ecs_iter_t* it);

/**
 * Light system
 * Updates light parameters for rendering
 */
void nexus_light_system(ecs_iter_t* it);

/**
 * Camera system
 * Updates camera matrices and parameters
 */
void nexus_camera_system(ecs_iter_t* it);

/**
 * Audio system
 * Updates audio sources and listeners
 */
void nexus_audio_system(ecs_iter_t* it);

/**
 * Animation system
 * Updates animation states
 */
void nexus_animation_system(ecs_iter_t* it);

#endif /* NEXUS3D_SYSTEMS_H */
