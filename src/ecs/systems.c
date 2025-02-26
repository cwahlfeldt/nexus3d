/**
 * Nexus3D ECS Systems Implementation
 * System definitions for the Entity Component System
 */

#include <flecs.h>
#include "nexus3d/ecs/systems.h"
#include "nexus3d/ecs/components.h"
#include "nexus3d/math/math_utils.h"
#include "nexus3d/renderer/renderer.h"
#include <stdio.h>
#include <string.h>

/* System phase tags */
/* Using simple global variables instead of ECS_TAG_DEFINE which requires world */
ecs_entity_t NexusPhaseInit;
ecs_entity_t NexusPhaseInput;
ecs_entity_t NexusPhasePhysics;
ecs_entity_t NexusPhaseLogic;
ecs_entity_t NexusPhaseAnimation;
ecs_entity_t NexusPhasePreRender;
ecs_entity_t NexusPhaseRender;
ecs_entity_t NexusPhasePostRender;
ecs_entity_t NexusPhaseCleanup;

/**
 * Core system registration
 * Registers all built-in systems with the ECS world
 * @return true if successful, false if there was an error
 */
 bool nexus_ecs_register_systems(ecs_world_t* world) {
         printf("shit");
      if (world == NULL) {
          printf("Failed to register systems: world is NULL\n");
          return false;
      }

      /* Create phase entities */
      NexusPhaseInit = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhaseInit"
      });
      NexusPhaseInput = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhaseInput"
      });
      NexusPhasePhysics = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhasePhysics"
      });
      NexusPhaseLogic = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhaseLogic"
      });
      NexusPhaseAnimation = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhaseAnimation"
      });
      NexusPhasePreRender = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhasePreRender"
      });
      NexusPhaseRender = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhaseRender"
      });
      NexusPhasePostRender = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhasePostRender"
      });
      NexusPhaseCleanup = ecs_entity_init(world, &(ecs_entity_desc_t){
          .name = "NexusPhaseCleanup"
      });

      /* Set up pipeline with custom phases */
      ecs_entity_t pipeline = ecs_pipeline_init(world, &(ecs_pipeline_desc_t){
          .query.terms = {
              { .id = EcsSystem }, /* First term must be EcsSystem */
              { .id = EcsPhase, .oper = EcsOptional }
          }
      });

      /* Set up phases in pipeline */
      ecs_add_id(world, NexusPhaseInit, EcsPhase);
      ecs_add_id(world, NexusPhaseInput, EcsPhase);
      ecs_add_id(world, NexusPhasePhysics, EcsPhase);
      ecs_add_id(world, NexusPhaseLogic, EcsPhase);
      ecs_add_id(world, NexusPhaseAnimation, EcsPhase);
      ecs_add_id(world, NexusPhasePreRender, EcsPhase);
      ecs_add_id(world, NexusPhaseRender, EcsPhase);
      ecs_add_id(world, NexusPhasePostRender, EcsPhase);
      ecs_add_id(world, NexusPhaseCleanup, EcsPhase);

      /* Set the pipeline order */
      ecs_add_pair(world, NexusPhaseInput, EcsDependsOn, NexusPhaseInit);
      ecs_add_pair(world, NexusPhasePhysics, EcsDependsOn, NexusPhaseInput);
      ecs_add_pair(world, NexusPhaseLogic, EcsDependsOn, NexusPhasePhysics);
      ecs_add_pair(world, NexusPhaseAnimation, EcsDependsOn, NexusPhaseLogic);
      ecs_add_pair(world, NexusPhasePreRender, EcsDependsOn, NexusPhaseAnimation);
      ecs_add_pair(world, NexusPhaseRender, EcsDependsOn, NexusPhasePreRender);
      ecs_add_pair(world, NexusPhasePostRender, EcsDependsOn, NexusPhaseRender);
      ecs_add_pair(world, NexusPhaseCleanup, EcsDependsOn, NexusPhasePostRender);

      /* Set default pipeline */
      ecs_set_pipeline(world, pipeline);

      /* Register actual systems to their respective phases using system interface */
      /* Transform system - PreRender phase */
      ecs_entity_t transform_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusTransformSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusPositionComponent) },
              { .id = ecs_id(NexusRotationComponent) },
              { .id = ecs_id(NexusScaleComponent) },
              { .id = ecs_id(NexusTransformComponent) }
          },
          .callback = nexus_transform_system
      });
      if (!transform_system) {
          fprintf(stderr, "Failed to create transform system\n");
      }
      ecs_add_id(world, transform_system, NexusPhasePreRender);

      /* Hierarchy system - PreRender phase */
      ecs_entity_t hierarchy_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusHierarchySystem" }),
          .query.terms = {
              { .id = ecs_id(NexusTransformComponent) }
          },
          .callback = nexus_hierarchy_system
      });
      if (!hierarchy_system) {
          fprintf(stderr, "Failed to create hierarchy system\n");
      }
      ecs_add_id(world, hierarchy_system, NexusPhasePreRender);

      /* Camera system - PreRender phase */
      ecs_entity_t camera_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusCameraSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusCameraComponent) },
              { .id = ecs_id(NexusTransformComponent) }
          },
          .callback = nexus_camera_system
      });
      if (!camera_system) {
          fprintf(stderr, "Failed to create camera system\n");
      }
      ecs_add_id(world, camera_system, NexusPhasePreRender);

      /* Light system - PreRender phase */
      ecs_entity_t light_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusLightSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusLightComponent) },
              { .id = ecs_id(NexusTransformComponent) }
          },
          .callback = nexus_light_system
      });
      if (!light_system) {
          fprintf(stderr, "Failed to create light system\n");
      }
      ecs_add_id(world, light_system, NexusPhasePreRender);

      /* Renderer system - Render phase */
      ecs_entity_t renderer_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusRendererSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusRenderableComponent) },
              { .id = ecs_id(NexusTransformComponent) }
          },
          .callback = nexus_renderer_system
      });
      if (!renderer_system) {
          fprintf(stderr, "Failed to create renderer system\n");
      }
      ecs_add_id(world, renderer_system, NexusPhaseRender);

      /* Physics system - Physics phase */
      ecs_entity_t physics_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusPhysicsSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusRigidBodyComponent), .oper = EcsOptional },
              { .id = ecs_id(NexusPositionComponent) },
              { .id = ecs_id(NexusRotationComponent), .oper = EcsOptional }
          },
          .callback = nexus_physics_system
      });
      if (!physics_system) {
          fprintf(stderr, "Failed to create physics system\n");
      }
      ecs_add_id(world, physics_system, NexusPhasePhysics);

      /* Animation system - Animation phase */
      ecs_entity_t animation_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusAnimationSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusTransformComponent) }
          },
          .callback = nexus_animation_system
      });
      if (!animation_system) {
          fprintf(stderr, "Failed to create animation system\n");
      }
      ecs_add_id(world, animation_system, NexusPhaseAnimation);

      /* Audio system - Logic phase */
      ecs_entity_t audio_system = ecs_system_init(world, &(ecs_system_desc_t){
          .entity = ecs_entity(world, { .name = "NexusAudioSystem" }),
          .query.terms = {
              { .id = ecs_id(NexusAudioSourceComponent) },
              { .id = ecs_id(NexusTransformComponent), .oper = EcsOptional }
          },
          .callback = nexus_audio_system
      });
      if (!audio_system) {
          fprintf(stderr, "Failed to create audio system\n");
      }
      ecs_add_id(world, audio_system, NexusPhaseLogic);

      printf("Successfully registered all ECS systems\n");
      return true;
  }

/**
 * Transform system - updates transform matrices based on position, rotation, and scale
 */
void nexus_transform_system(ecs_iter_t* it) {
    /* Get component arrays */
    NexusPositionComponent* positions = ecs_field(it, NexusPositionComponent, 1);
    NexusRotationComponent* rotations = ecs_field(it, NexusRotationComponent, 2);
    NexusScaleComponent* scales = ecs_field(it, NexusScaleComponent, 3);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 4);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Transform system processing %d entities\n", it->count);
    }

    /* Update each entity */
    for (int i = 0; i < it->count; i++) {
        /* Only update if transform is dirty */
        if (transforms[i].dirty) {
            /* Build model matrix from position, rotation, and scale */

            /* Start with identity matrix */
            glm_mat4_identity(transforms[i].local);

            /* Apply translation */
            glm_translate(transforms[i].local, positions[i].value);

            /* Apply rotation from quaternion */
            mat4 rotation_matrix;
            glm_quat_mat4(rotations[i].quaternion, rotation_matrix);
            glm_mat4_mul(transforms[i].local, rotation_matrix, transforms[i].local);

            /* Apply scale */
            glm_scale(transforms[i].local, scales[i].value);

            /* Copy local to world (hierarchy system will update this if needed) */
            glm_mat4_copy(transforms[i].local, transforms[i].world);

            /* Clear dirty flag */
            transforms[i].dirty = false;
        }
    }
}

/**
 * Hierarchy system - updates world transforms based on parent-child relationships
 */
void nexus_hierarchy_system(ecs_iter_t* it) {
    /* Get transform components */
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 1);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Hierarchy system processing %d entities\n", it->count);
    }

    /* For a proper hierarchy system, we would have parent-child relationships,
     * but for now we'll simply ensure world transforms are updated */
    for (int i = 0; i < it->count; i++) {
        /* In a real implementation, we would iterate through the hierarchy
         * and multiply parent world transforms with child local transforms */

        /* For now, just ensure world is a copy of local if no hierarchy exists */
        if (transforms[i].dirty) {
            glm_mat4_copy(transforms[i].local, transforms[i].world);
            transforms[i].dirty = false;
        }
    }
}

/**
 * Physics system - updates physics simulation
 */
void nexus_physics_system(ecs_iter_t* it) {
    /* Get component arrays */
    NexusRigidBodyComponent* bodies = ecs_field(it, NexusRigidBodyComponent, 1);
    NexusPositionComponent* positions = ecs_field(it, NexusPositionComponent, 2);
    NexusRotationComponent* rotations = ecs_field(it, NexusRotationComponent, 3);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Physics system processing %d entities\n", it->count);
    }

    /* In a real implementation, this would integrate with the physics engine */
    /* For now, just print info about each rigid body */
    for (int i = 0; i < it->count; i++) {
        /* Check if this entity has a rigid body component */
        if (bodies) {
            /* Apply physics calculations (this would be more complex in reality) */
            if (!bodies[i].sleeping && !bodies[i].kinematic) {
                /* Basic gravity effect - move objects downward a bit */
                positions[i].value[1] -= 9.8f * it->delta_time;
            }
        }
    }
}

/**
 * Renderer system - handles rendering of all entities with render components
 */
void nexus_renderer_system(ecs_iter_t* it) {
    /* Get component arrays */
    NexusRenderableComponent* renderables = ecs_field(it, NexusRenderableComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Renderer system processing %d entities\n", it->count);
    }

    /* Get global renderer instance */
    struct NexusRenderer* renderer = nexus_engine_get_renderer();
    if (!renderer) {
        return;
    }

    /* Process each entity */
    for (int i = 0; i < it->count; i++) {
        /* Only render visible objects */
        if (renderables[i].visible && renderables[i].mesh && renderables[i].material) {
            /* Render the mesh with its material and world transform */
            nexus_renderer_render_mesh(
                renderer,
                renderables[i].mesh,
                renderables[i].material->shader,
                (float*)transforms[i].world
            );
        }
    }
}

/**
 * Light system - updates light parameters for rendering
 */
void nexus_light_system(ecs_iter_t* it) {
    /* Get component arrays */
    NexusLightComponent* lights = ecs_field(it, NexusLightComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Light system processing %d entities\n", it->count);
    }

    /* In a real implementation, this would update light position, direction,
     * and prepare shadow maps for rendering */
    for (int i = 0; i < it->count; i++) {
        /* Different handling based on light type */
        switch (lights[i].type) {
            case NEXUS_LIGHT_DIRECTIONAL:
                /* Update directional light - extract direction from transform */
                /* This would be passed to the renderer for shadow mapping and lighting */
                break;

            case NEXUS_LIGHT_POINT:
                /* Update point light position from transform */
                break;

            case NEXUS_LIGHT_SPOT:
                /* Update spot light position and direction from transform */
                break;
        }
    }
}

/**
 * Camera system - updates camera matrices and parameters
 */
void nexus_camera_system(ecs_iter_t* it) {
    /* Get component arrays */
    NexusCameraComponent* cameras = ecs_field(it, NexusCameraComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Camera system processing %d entities\n", it->count);
    }

    /* Reference to the main renderer */
    NexusRenderer* renderer = nexus_engine_get_renderer();

    /* Process each camera */
    for (int i = 0; i < it->count; i++) {
        /* Only process active cameras */
        if (cameras[i].active && cameras[i].camera) {
            /* Extract position and orientation from transform */
            vec3 position, forward, up;

            /* Convert world transform to position */
            position[0] = transforms[i].world[3][0];
            position[1] = transforms[i].world[3][1];
            position[2] = transforms[i].world[3][2];

            /* Extract forward direction (-Z axis in our coordinate system) */
            forward[0] = -transforms[i].world[2][0];
            forward[1] = -transforms[i].world[2][1];
            forward[2] = -transforms[i].world[2][2];

            /* Extract up direction (Y axis) */
            up[0] = transforms[i].world[1][0];
            up[1] = transforms[i].world[1][1];
            up[2] = transforms[i].world[1][2];

            /* Update camera position and orientation */
            nexus_camera_set_position(cameras[i].camera, position[0], position[1], position[2]);

            /* Calculate target point: position + forward */
            vec3 target;
            glm_vec3_add(position, forward, target);
            nexus_camera_look_at(cameras[i].camera, target[0], target[1], target[2]);

            /* Update camera matrices */
            nexus_camera_update(cameras[i].camera);

            /* If this is the primary camera, set it as the renderer's main camera */
            if (cameras[i].primary && renderer) {
                nexus_renderer_set_camera(renderer, cameras[i].camera);
            }
        }
    }
}

/**
 * Audio system - updates audio sources and listeners
 */
void nexus_audio_system(ecs_iter_t* it) {
    /* Get component arrays */
    NexusAudioSourceComponent* sources = ecs_field(it, NexusAudioSourceComponent, 1);
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 2);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Audio system processing %d entities\n", it->count);
    }

    /* Get global audio subsystem */
    struct NexusAudio* audio = nexus_engine_get_audio();
    if (!audio) {
        return;
    }

    /* Process each audio source */
    for (int i = 0; i < it->count; i++) {
        /* Handle automatic playback */
        if (sources[i].auto_play && !sources[i].playing) {
            /* Start playback */
            sources[i].playing = true;
            /* This would call into the audio subsystem to start playback */
        }

        /* Update 3D position for spatialized sounds */
        if (sources[i].spatial_blend > 0.0f && transforms) {
            /* Extract position from transform */
            vec3 position;
            position[0] = transforms[i].world[3][0];
            position[1] = transforms[i].world[3][1];
            position[2] = transforms[i].world[3][2];

            /* Pass to audio system - would call into audio subsystem */
        }
    }
}

/**
 * Animation system - updates animation states
 */
void nexus_animation_system(ecs_iter_t* it) {
    /* Get transform components */
    NexusTransformComponent* transforms = ecs_field(it, NexusTransformComponent, 1);

    /* Static counter to limit debug output frequency */
    static int debug_counter = 0;
    if (debug_counter++ % 300 == 0) {
        printf("Animation system processing %d entities\n", it->count);
    }

    /* In a real implementation, this would update skeletal animations,
     * blend animations, and apply results to transforms */
    for (int i = 0; i < it->count; i++) {
        /* Animation processing would happen here */
        /* For now, the system is a placeholder */
    }
}
