/**
 * Nexus3D ECS Components Implementation
 * Component definitions for the Entity Component System
 */

#include "nexus3d/ecs/components.h"
#include <flecs.h>
#include <stdio.h>
#include <string.h>

/* Component registration function */
void nexus_ecs_register_components(ecs_world_t* world) {
    if (world == NULL) {
        printf("Failed to register components: world is NULL\n");
        return;
    }

    /* Register components manually */
    ecs_entity_t pos_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusPositionComponent"}),
        .type = {.size = sizeof(NexusPositionComponent), .alignment = 4}
    });
    
    ecs_entity_t rot_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusRotationComponent"}),
        .type = {.size = sizeof(NexusRotationComponent), .alignment = 4}
    });
    
    ecs_entity_t scale_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusScaleComponent"}),
        .type = {.size = sizeof(NexusScaleComponent), .alignment = 4}
    });
    
    ecs_entity_t transform_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusTransformComponent"}),
        .type = {.size = sizeof(NexusTransformComponent), .alignment = 4}
    });

    ecs_entity_t renderable_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusRenderableComponent"}),
        .type = {.size = sizeof(NexusRenderableComponent), .alignment = 8}
    });

    ecs_entity_t camera_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusCameraComponent"}),
        .type = {.size = sizeof(NexusCameraComponent), .alignment = 8}
    });

    ecs_entity_t light_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusLightComponent"}),
        .type = {.size = sizeof(NexusLightComponent), .alignment = 4}
    });

    ecs_entity_t velocity_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusVelocityComponent"}),
        .type = {.size = sizeof(NexusVelocityComponent), .alignment = 4}
    });

    ecs_entity_t rigidbody_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusRigidBodyComponent"}),
        .type = {.size = sizeof(NexusRigidBodyComponent), .alignment = 4}
    });

    ecs_entity_t audiosource_id = ecs_component_init(world, &(ecs_component_desc_t){
        .entity = ecs_entity(world, {.name = "NexusAudioSourceComponent"}),
        .type = {.size = sizeof(NexusAudioSourceComponent), .alignment = 4}
    });

    /* Register tags */
    ecs_entity_t static_tag = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "NexusStaticTag"
    });

    ecs_entity_t dynamic_tag = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "NexusDynamicTag"
    });

    ecs_entity_t maincamera_tag = ecs_entity_init(world, &(ecs_entity_desc_t){
        .name = "NexusMainCameraTag"
    });

    printf("Successfully registered ECS components\n");
}
