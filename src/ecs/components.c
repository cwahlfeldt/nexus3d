/**
 * Nexus3D ECS Components Implementation
 * Component definitions for the Entity Component System
 */

#include "nexus3d/ecs/components.h"
#include <flecs.h>
#include <stdio.h>
#include <string.h>

/* Global component IDs - we need to make these accessible to flecs */
ECS_COMPONENT_DECLARE(NexusPositionComponent);
ECS_COMPONENT_DECLARE(NexusRotationComponent);
ECS_COMPONENT_DECLARE(NexusScaleComponent);
ECS_COMPONENT_DECLARE(NexusTransformComponent);
ECS_COMPONENT_DECLARE(NexusRenderableComponent);
ECS_COMPONENT_DECLARE(NexusCameraComponent);
ECS_COMPONENT_DECLARE(NexusLightComponent);
ECS_COMPONENT_DECLARE(NexusVelocityComponent);
ECS_COMPONENT_DECLARE(NexusRigidBodyComponent);
ECS_COMPONENT_DECLARE(NexusAudioSourceComponent);

/* Component registration function */
void nexus_ecs_register_components(ecs_world_t* world) {
    if (world == NULL) {
        printf("Failed to register components: world is NULL\n");
        return;
    }

    /* Register components using flecs macros */
    ECS_COMPONENT_DEFINE(world, NexusPositionComponent);
    ECS_COMPONENT_DEFINE(world, NexusRotationComponent);
    ECS_COMPONENT_DEFINE(world, NexusScaleComponent);
    ECS_COMPONENT_DEFINE(world, NexusTransformComponent);
    ECS_COMPONENT_DEFINE(world, NexusRenderableComponent);
    ECS_COMPONENT_DEFINE(world, NexusCameraComponent);
    ECS_COMPONENT_DEFINE(world, NexusLightComponent);
    ECS_COMPONENT_DEFINE(world, NexusVelocityComponent);
    ECS_COMPONENT_DEFINE(world, NexusRigidBodyComponent);
    ECS_COMPONENT_DEFINE(world, NexusAudioSourceComponent);

    /* Register tags */
    // ECS_TAG_DEFINE(world, NexusStaticTag);
    // ECS_TAG_DEFINE(world, NexusDynamicTag);
    // ECS_TAG_DEFINE(world, NexusMainCameraTag);

    printf("Successfully registered ECS components\n");
}
