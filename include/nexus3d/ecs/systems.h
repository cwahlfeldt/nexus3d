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
extern ECS_TAG_DECLARE(NexusPhaseInit);        /* Initialization phase */
extern ECS_TAG_DECLARE(NexusPhaseInput);       /* Input processing phase */
extern ECS_TAG_DECLARE(NexusPhasePhysics);     /* Physics update phase */
extern ECS_TAG_DECLARE(NexusPhaseLogic);       /* Game logic phase */
extern ECS_TAG_DECLARE(NexusPhaseAnimation);   /* Animation phase */
extern ECS_TAG_DECLARE(NexusPhasePreRender);   /* Pre-render phase */
extern ECS_TAG_DECLARE(NexusPhaseRender);      /* Render phase */
extern ECS_TAG_DECLARE(NexusPhasePostRender);  /* Post-render phase */
extern ECS_TAG_DECLARE(NexusPhaseCleanup);     /* Cleanup phase */

/**
 * Core system registration
 * Registers all built-in systems with the ECS world
 */
void nexus_ecs_register_systems(ecs_world_t* world);

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
