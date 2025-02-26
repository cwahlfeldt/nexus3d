/**
 * Nexus3D Simple Cube Example
 * Demonstrates creating a basic 3D scene with a spinning cube
 */

#include "nexus3d/nexus3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

/* Entity for our cube */
ecs_entity_t cube_entity;

/* Rotation speed (degrees per second) */
#define ROTATION_SPEED 45.0f

/**
 * Custom update system for the cube
 */
void cube_update_system(ecs_iter_t *it) {
    /* Get components */
    NexusRotationComponent *rotations = ecs_field(it, NexusRotationComponent, 1);
    
    /* Update rotation for each matching entity */
    for (int i = 0; i < it->count; i++) {
        /* Rotate around Y axis */
        rotations[i].euler[1] += ROTATION_SPEED * it->delta_time;
        
        /* Keep rotation in [0, 360) range */
        rotations[i].euler[1] = fmodf(rotations[i].euler[1], 360.0f);
        
        /* Update quaternion from Euler angles */
        nexus_quat_from_euler_degrees(rotations[i].quaternion, 
                                     rotations[i].euler[0], 
                                     rotations[i].euler[1], 
                                     rotations[i].euler[2]);
    }
}

/**
 * Create a simple scene
 */
bool create_scene(void) {
    /* Get the ECS world */
    ecs_world_t *world = nexus_engine_get_world();
    if (world == NULL) {
        fprintf(stderr, "Failed to get ECS world!\n");
        return false;
    }
    
    /* Get the renderer */
    NexusRenderer *renderer = nexus_engine_get_renderer();
    if (renderer == NULL) {
        fprintf(stderr, "Failed to get renderer!\n");
        return false;
    }
    
    /* Create a simple PBR shader */
    NexusShader *pbr_shader = nexus_shader_create(renderer->gpu_device, "PBR");
    if (pbr_shader == NULL) {
        fprintf(stderr, "Failed to create shader!\n");
        return false;
    }
    
    /* TODO: Load the PBR shader when we implement shader loading */
    
    /* Create a material */
    NexusMaterial *material = nexus_material_create(renderer->gpu_device, "CubeMaterial");
    if (material == NULL) {
        fprintf(stderr, "Failed to create material!\n");
        nexus_shader_destroy(pbr_shader);
        return false;
    }
    
    /* Set up material properties */
    nexus_material_set_shader(material, pbr_shader);
    nexus_material_set_base_color(material, 0.2f, 0.4f, 0.8f, 1.0f);
    nexus_material_set_metallic(material, 0.1f);
    nexus_material_set_roughness(material, 0.7f);
    
    /* Create a cube mesh */
    NexusMesh *cube_mesh = nexus_mesh_create_cube(renderer->gpu_device, 1.0f);
    if (cube_mesh == NULL) {
        fprintf(stderr, "Failed to create cube mesh!\n");
        nexus_material_destroy(material);
        nexus_shader_destroy(pbr_shader);
        return false;
    }
    
    /* Create main camera */
    NexusCamera *camera = nexus_camera_create();
    if (camera == NULL) {
        fprintf(stderr, "Failed to create camera!\n");
        nexus_mesh_destroy(cube_mesh);
        nexus_material_destroy(material);
        nexus_shader_destroy(pbr_shader);
        return false;
    }
    
    /* Set up camera */
    nexus_camera_set_perspective(camera, 45.0f, 16.0f/9.0f, 0.1f, 1000.0f);
    nexus_camera_set_position(camera, 0.0f, 0.0f, 5.0f);
    nexus_camera_look_at(camera, 0.0f, 0.0f, 0.0f);
    nexus_camera_update(camera);
    
    /* Set as main camera */
    nexus_renderer_set_camera(renderer, camera);
    
    /* Create a camera entity */
    ecs_entity_t camera_entity = ecs_new(world);
    
    /* Add camera components */
    ecs_set(world, camera_entity, NexusPositionComponent, {{0.0f, 0.0f, 5.0f}});
    ecs_set(world, camera_entity, NexusRotationComponent, {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
    ecs_set(world, camera_entity, NexusCameraComponent, {camera, true, true});
    
    /* Create a directional light */
    ecs_entity_t light_entity = ecs_new(world);
    
    /* Add light components */
    ecs_set(world, light_entity, NexusPositionComponent, {{10.0f, 10.0f, 10.0f}});
    ecs_set(world, light_entity, NexusRotationComponent, {{45.0f, 45.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
    ecs_set(world, light_entity, NexusLightComponent, {
        NEXUS_LIGHT_DIRECTIONAL,    /* Light type */
        {1.0f, 1.0f, 1.0f},         /* Color (white) */
        1.0f,                       /* Intensity */
        0.0f,                       /* Range (not used for directional) */
        0.0f,                       /* Spot angle (not used for directional) */
        0.0f,                       /* Spot softness (not used for directional) */
        true,                       /* Cast shadows */
        2048                        /* Shadow resolution */
    });
    
    /* Create the cube entity */
    cube_entity = ecs_new(world);
    
    /* Add components to the cube */
    ecs_set(world, cube_entity, NexusPositionComponent, {{0.0f, 0.0f, 0.0f}});
    ecs_set(world, cube_entity, NexusRotationComponent, {{0.0f, 0.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f}});
    ecs_set(world, cube_entity, NexusScaleComponent, {{1.0f, 1.0f, 1.0f}});
    ecs_set(world, cube_entity, NexusTransformComponent, {{0}, {0}, true});
    ecs_set(world, cube_entity, NexusRenderableComponent, {
        cube_mesh,   /* Mesh */
        material,    /* Material */
        true,        /* Visible */
        true,        /* Cast shadows */
        true         /* Receive shadows */
    });
    
    /* Create a custom system for rotating the cube */
    ecs_system(world, {
        .entity = ecs_entity(world, {.name = "CubeRotationSystem"}),
        .query.filter.terms = {
            { .id = ecs_id(NexusRotationComponent), .inout = EcsInOut }
        },
        .callback = cube_update_system,
        .interval = 0 /* Run every frame */
    });
    
    return true;
}

/**
 * Clean up the scene
 */
void destroy_scene(void) {
    /* Get the ECS world */
    ecs_world_t *world = nexus_engine_get_world();
    if (world == NULL) {
        return;
    }
    
    /* Delete entities */
    ecs_delete(world, cube_entity);
    
    /* Renderer will clean up resources */
}

/**
 * Main function
 */
int main(int argc, char *argv[]) {
    /* Initialize the engine */
    if (!nexus3d_init()) {
        fprintf(stderr, "Failed to initialize Nexus3D Engine!\n");
        return EXIT_FAILURE;
    }
    
    /* Print version information */
    printf("Nexus3D Engine v%s - Simple Cube Example\n", nexus3d_get_version());
    
    /* Create scene */
    if (!create_scene()) {
        fprintf(stderr, "Failed to create scene!\n");
        nexus3d_shutdown();
        return EXIT_FAILURE;
    }
    
    /* Main game loop */
    while (nexus_engine_is_running()) {
        /* Update engine */
        nexus_engine_update();
    }
    
    /* Destroy scene */
    destroy_scene();
    
    /* Shutdown engine */
    nexus3d_shutdown();
    
    return EXIT_SUCCESS;
}
