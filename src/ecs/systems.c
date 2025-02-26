/**
 * Nexus3D ECS Systems Implementation
 * System definitions for the Entity Component System
 */

#include "nexus3d/ecs/systems.h"
#include "nexus3d/ecs/components.h"
#include "nexus3d/math/math_utils.h"
#include <stdio.h>
#include <string.h>

/* System phase tags */
ECS_TAG_DECLARE(NexusPhaseInit);
ECS_TAG_DECLARE(NexusPhaseInput);
ECS_TAG_DECLARE(NexusPhasePhysics);
ECS_TAG_DECLARE(NexusPhaseLogic);
ECS_TAG_DECLARE(NexusPhaseAnimation);
ECS_TAG_DECLARE(NexusPhasePreRender);
ECS_TAG_DECLARE(NexusPhaseRender);
ECS_TAG_DECLARE(NexusPhasePostRender);
ECS_TAG_DECLARE(NexusPhaseCleanup);

/* Define phase tags to allow referencing them in systems */
ECS_TAG_DEFINE(NexusPhaseInit);
ECS_TAG_DEFINE(NexusPhaseInput);
ECS_TAG_DEFINE(NexusPhasePhysics);
ECS_TAG_DEFINE(NexusPhaseLogic);
ECS_TAG_DEFINE(NexusPhaseAnimation);
ECS_TAG_DEFINE(NexusPhasePreRender);
ECS_TAG_DEFINE(NexusPhaseRender);
ECS_TAG_DEFINE(NexusPhasePostRender);
ECS_TAG_DEFINE(NexusPhaseCleanup);

/* Transform system - updates transform matrices from position, rotation, and scale */
void nexus_transform_system(ecs_iter_t* it) {
    NexusPositionComponent* positions = ecs_field(it, NexusPositionComponent, 1);
    NexusRotationComponent* rotations = ecs_field(it, NexusRotationComponent, 2);
    NexusScaleComponent* scales = ecs_field(it, NexusScaleComponent, 3);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 4);
    
    for (int i = 0; i < it->count; i++) {
        /* Check if transform is marked as dirty or components have changed */
        if (transforms[i].dirty) {
            /* Create transform matrix from position, rotation, and scale */
            nexus_mat4_transform(
                transforms[i].local,
                positions[i].value,
                rotations[i].euler,
                scales[i].value
            );
            
            /* Copy local transform to world transform (will be updated by hierarchy system) */
            memcpy(transforms[i].world, transforms[i].local, sizeof(mat4));
            
            /* Mark transform as clean */
            transforms[i].dirty = false;
        }
    }
}

/* Hierarchy system - updates world transforms based on parent-child relationships */
void nexus_hierarchy_system(ecs_iter_t* it) {
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 1);
    
    for (int i = 0; i < it->count; i++) {
        ecs_entity_t entity = it->entities[i];
        
        /* Check if entity has a parent */
        ecs_entity_t parent = ecs_get_parent(it->world, entity);
        if (parent != 0) {
            /* Get parent's transform component */
            const NexusTransformComponent* parent_transform = ecs_get(it->world, parent, NexusTransformComponent);
            
            if (parent_transform != NULL) {
                /* Multiply parent's world transform by local transform */
                glm_mat4_mul(parent_transform->world, transforms[i].local, transforms[i].world);
            }
        }
    }
}

/* Physics system - updates physics simulation */
void nexus_physics_system(ecs_iter_t* it) {
    NexusPositionComponent* positions = ecs_field(it, NexusPositionComponent, 1);
    NexusVelocityComponent* velocities = ecs_field(it, NexusVelocityComponent, 2);
    
    /* Simple velocity integration */
    for (int i = 0; i < it->count; i++) {
        /* Update position based on velocity */
        positions[i].value[0] += velocities[i].linear[0] * it->delta_time;
        positions[i].value[1] += velocities[i].linear[1] * it->delta_time;
        positions[i].value[2] += velocities[i].linear[2] * it->delta_time;
        
        /* Mark entity's transform as dirty if it has one */
        ecs_entity_t entity = it->entities[i];
        NexusTransformComponent* transform = ecs_get_mut(it->world, entity, NexusTransformComponent);
        if (transform != NULL) {
            transform->dirty = true;
        }
    }
}

/* Renderer system - renders entities with render components */
void nexus_renderer_system(ecs_iter_t* it) {
    /* This system would be implemented to interact with the renderer subsystem */
    /* We'll just provide a placeholder implementation for now */
    
    NexusRenderableComponent* renderables = ecs_field(it, NexusRenderableComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (renderables[i].visible && renderables[i].mesh != NULL && renderables[i].material != NULL) {
            /* Get entity's world transform */
            const float* transform = transforms[i].world;
            
            /* This would call into the renderer subsystem */
            /* nexus_renderer_render_mesh(renderer, renderables[i].mesh, renderables[i].material, transform); */
        }
    }
}

/* Light system - updates light parameters for rendering */
void nexus_light_system(ecs_iter_t* it) {
    /* This system would be implemented to update light parameters for rendering */
    /* We'll just provide a placeholder implementation for now */
    
    NexusLightComponent* lights = ecs_field(it, NexusLightComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);
    
    for (int i = 0; i < it->count; i++) {
        /* Get light position from transform */
        vec3 position;
        nexus_mat4_get_position(transforms[i].world, position);
        
        /* This would call into the renderer subsystem to update light parameters */
        /* nexus_renderer_update_light(renderer, lights[i], position); */
    }
}

/* Camera system - updates camera matrices and parameters */
void nexus_camera_system(ecs_iter_t* it) {
    /* This system would be implemented to update camera parameters for rendering */
    /* We'll just provide a placeholder implementation for now */
    
    NexusCameraComponent* cameras = ecs_field(it, NexusCameraComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (cameras[i].active && cameras[i].camera != NULL) {
            /* Get camera position and orientation from transform */
            vec3 position;
            versor rotation;
            nexus_mat4_get_position(transforms[i].world, position);
            nexus_mat4_get_rotation(transforms[i].world, rotation);
            
            /* Update camera */
            NexusCamera* camera = cameras[i].camera;
            
            /* Set camera position */
            camera->position[0] = position[0];
            camera->position[1] = position[1];
            camera->position[2] = position[2];
            
            /* Extract forward, up, and right vectors from rotation */
            vec3 forward = {0.0f, 0.0f, -1.0f};
            vec3 up = {0.0f, 1.0f, 0.0f};
            vec3 right = {1.0f, 0.0f, 0.0f};
            
            glm_quat_rotatev(rotation, forward, camera->forward);
            glm_quat_rotatev(rotation, up, camera->up);
            glm_quat_rotatev(rotation, right, camera->right);
            
            /* Look at a point in front of the camera */
            vec3 target;
            glm_vec3_add(camera->position, camera->forward, target);
            
            /* Update camera's view matrix */
            glm_lookat(camera->position, target, camera->up, camera->view);
            
            /* Recalculate view-projection matrix */
            glm_mat4_mul(camera->projection, camera->view, camera->view_projection);
        }
    }
}

/* Audio system - updates audio sources and listeners */
void nexus_audio_system(ecs_iter_t* it) {
    /* This system would be implemented to update audio sources and listeners */
    /* We'll just provide a placeholder implementation for now */
    
    NexusAudioSourceComponent* audio_sources = ecs_field(it, NexusAudioSourceComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);
    
    for (int i = 0; i < it->count; i++) {
        if (audio_sources[i].playing) {
            /* Get audio source position from transform */
            vec3 position;
            nexus_mat4_get_position(transforms[i].world, position);
            
            /* This would call into the audio subsystem to update source position */
            /* nexus_audio_set_source_position(audio, audio_sources[i].sound_id, position); */
        }
    }
}

/* Animation system - updates animation states */
void nexus_animation_system(ecs_iter_t* it) {
    /* This system would be implemented to update animation states */
    /* We'll omit the implementation for now as it requires more animation components */
}

/* Register all ECS systems */
void nexus_ecs_register_systems(ecs_world_t* world) {
    if (world == NULL) {
        printf("Failed to register systems: world is NULL\n");
        return;
    }

    /* Set up pipeline with custom phases */
    ecs_entity_t pipeline = ecs_pipeline(world, {
        .query.filter.terms = {
            { .id = EcsPhase, .src.id = EcsSelf }
        }
    });
    
    /* Set pipeline phases */
    ecs_pipeline_op(world, {
        .entity = pipeline,
        .desc = (ecs_pipeline_desc_t[]){
            { .id = NexusPhaseInit },
            { .id = NexusPhaseInput },
            { .id = NexusPhasePhysics },
            { .id = NexusPhaseLogic },
            { .id = NexusPhaseAnimation },
            { .id = NexusPhasePreRender },
            { .id = NexusPhaseRender },
            { .id = NexusPhasePostRender },
            { .id = NexusPhaseCleanup },
            { .id = 0 }
        }
    });
    
    /* Set default pipeline */
    ecs_set_pipeline(world, pipeline);
    
    /* Register transform system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusTransformSystem",
            .add = { NexusPhaseLogic }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusPositionComponent) },
            { .id = ecs_id(NexusRotationComponent) },
            { .id = ecs_id(NexusScaleComponent) },
            { .id = ecs_id(NexusTransformComponent) }
        },
        .callback = nexus_transform_system
    });
    
    /* Register hierarchy system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusHierarchySystem", 
            .add = { NexusPhaseLogic }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusTransformComponent) }
        },
        .callback = nexus_hierarchy_system
    });
    
    /* Register physics system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusPhysicsSystem",
            .add = { NexusPhasePhysics }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusPositionComponent) },
            { .id = ecs_id(NexusVelocityComponent) }
        },
        .callback = nexus_physics_system
    });
    
    /* Register renderer system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusRendererSystem",
            .add = { NexusPhaseRender }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusRenderableComponent) },
            { .id = ecs_id(NexusTransformComponent) }
        },
        .callback = nexus_renderer_system
    });
    
    /* Register light system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusLightSystem",
            .add = { NexusPhasePreRender }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusLightComponent) },
            { .id = ecs_id(NexusTransformComponent) }
        },
        .callback = nexus_light_system
    });
    
    /* Register camera system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusCameraSystem",
            .add = { NexusPhasePreRender }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusCameraComponent) },
            { .id = ecs_id(NexusTransformComponent) }
        },
        .callback = nexus_camera_system
    });
    
    /* Register audio system */
    ecs_system(world, {
        .entity = ecs_entity(world, {
            .name = "NexusAudioSystem",
            .add = { NexusPhaseLogic }
        }),
        .query.filter.terms = {
            { .id = ecs_id(NexusAudioSourceComponent) },
            { .id = ecs_id(NexusTransformComponent) }
        },
        .callback = nexus_audio_system
    });
    
    printf("Successfully registered ECS systems\n");
}
