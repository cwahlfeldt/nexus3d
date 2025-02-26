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

    /* Register Transform components */
    ECS_COMPONENT_DEFINE(world, NexusPositionComponent);
    ECS_COMPONENT_DEFINE(world, NexusRotationComponent);
    ECS_COMPONENT_DEFINE(world, NexusScaleComponent);
    ECS_COMPONENT_DEFINE(world, NexusTransformComponent);

    /* Register Rendering components */
    ECS_COMPONENT_DEFINE(world, NexusRenderableComponent);
    ECS_COMPONENT_DEFINE(world, NexusCameraComponent);
    ECS_COMPONENT_DEFINE(world, NexusLightComponent);

    /* Register Physics components */
    ECS_COMPONENT_DEFINE(world, NexusVelocityComponent);
    ECS_COMPONENT_DEFINE(world, NexusRigidBodyComponent);

    /* Register Audio components */
    ECS_COMPONENT_DEFINE(world, NexusAudioSourceComponent);

    /* Register Tag components */
    ECS_TAG_DEFINE(world, NexusStaticTag);
    ECS_TAG_DEFINE(world, NexusDynamicTag);
    ECS_TAG_DEFINE(world, NexusMainCameraTag);

    /* Add component metadata for reflection */
    ecs_struct(world, {
        .entity = ecs_id(NexusPositionComponent),
        .members = {
            { .name = "value", .type = ecs_id(ecs_f32_t), .count = 3 }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(NexusRotationComponent),
        .members = {
            { .name = "euler", .type = ecs_id(ecs_f32_t), .count = 3 },
            { .name = "quaternion", .type = ecs_id(ecs_f32_t), .count = 4 }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(NexusScaleComponent),
        .members = {
            { .name = "value", .type = ecs_id(ecs_f32_t), .count = 3 }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(NexusTransformComponent),
        .members = {
            { .name = "local", .type = ecs_id(ecs_f32_t), .count = 16 },
            { .name = "world", .type = ecs_id(ecs_f32_t), .count = 16 },
            { .name = "dirty", .type = ecs_id(ecs_bool_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(NexusRenderableComponent),
        .members = {
            { .name = "mesh", .type = ecs_id(ecs_uptr_t) },
            { .name = "material", .type = ecs_id(ecs_uptr_t) },
            { .name = "visible", .type = ecs_id(ecs_bool_t) },
            { .name = "cast_shadows", .type = ecs_id(ecs_bool_t) },
            { .name = "receive_shadows", .type = ecs_id(ecs_bool_t) }
        }
    });

    ecs_struct(world, {
        .entity = ecs_id(NexusCameraComponent),
        .members = {
            { .name = "camera", .type = ecs_id(ecs_uptr_t) },
            { .name = "primary", .type = ecs_id(ecs_bool_t) },
            { .name = "active", .type = ecs_id(ecs_bool_t) }
        }
    });

    /* Define relationships between components */
    
    /* Transform components relationship - having Position, Rotation, Scale 
       will automatically add Transform component */
    ecs_observer(world, {
        .filter.terms = {
            { .id = ecs_id(NexusPositionComponent) },
            { .id = ecs_id(NexusRotationComponent) },
            { .id = ecs_id(NexusScaleComponent) }
        },
        .events = { EcsOnAdd },
        .callback = [](ecs_iter_t *it) {
            for (int i = 0; i < it->count; i++) {
                ecs_entity_t entity = it->entities[i];
                
                /* Only add Transform if it doesn't already have one */
                if (!ecs_has(it->world, entity, NexusTransformComponent)) {
                    /* Create identity transform matrices */
                    NexusTransformComponent transform = {0};
                    
                    /* Set identity matrix for local and world transforms */
                    for (int j = 0; j < 4; j++) {
                        transform.local[j * 4 + j] = 1.0f;
                        transform.world[j * 4 + j] = 1.0f;
                    }
                    
                    /* Mark as dirty to recalculate on first update */
                    transform.dirty = true;
                    
                    /* Add component */
                    ecs_set_ptr(it->world, entity, NexusTransformComponent, &transform);
                }
            }
        }
    });

    /* When a Camera Component is added with primary flag, ensure only one camera is primary */
    ecs_observer(world, {
        .filter.terms = {
            { .id = ecs_id(NexusCameraComponent) }
        },
        .events = { EcsOnAdd, EcsOnSet },
        .callback = [](ecs_iter_t *it) {
            NexusCameraComponent *cameras = ecs_field(it, NexusCameraComponent, 1);
            
            for (int i = 0; i < it->count; i++) {
                ecs_entity_t entity = it->entities[i];
                
                /* Check if this camera is being set as primary */
                if (cameras[i].primary) {
                    /* Find all other cameras marked as primary and unmark them */
                    ecs_query_t *query = ecs_query(it->world, {
                        .filter.terms = {
                            { .id = ecs_id(NexusCameraComponent) }
                        }
                    });
                    
                    if (query) {
                        ecs_iter_t qit = ecs_query_iter(it->world, query);
                        while (ecs_query_next(&qit)) {
                            NexusCameraComponent *other_cameras = ecs_field(&qit, NexusCameraComponent, 1);
                            for (int j = 0; j < qit.count; j++) {
                                ecs_entity_t other_entity = qit.entities[j];
                                
                                /* Skip the current entity */
                                if (other_entity == entity) {
                                    continue;
                                }
                                
                                /* Unmark as primary if currently marked */
                                if (other_cameras[j].primary) {
                                    other_cameras[j].primary = false;
                                }
                            }
                        }
                        
                        ecs_query_fini(query);
                    }
                }
            }
        }
    });

    printf("Successfully registered ECS components\n");
}
