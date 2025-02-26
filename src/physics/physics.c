/**
 * Nexus3D Physics System Implementation
 * Basic physics implementation using ECS
 */

#include "nexus3d/physics/physics.h"
#include "nexus3d/ecs/components.h"
#include "nexus3d/math/math_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Default physics configuration values */
#define NEXUS_PHYSICS_DEFAULT_GRAVITY_X 0.0f
#define NEXUS_PHYSICS_DEFAULT_GRAVITY_Y -9.81f
#define NEXUS_PHYSICS_DEFAULT_GRAVITY_Z 0.0f
#define NEXUS_PHYSICS_DEFAULT_TIMESTEP (1.0f / 60.0f)
#define NEXUS_PHYSICS_DEFAULT_MAX_SUBSTEPS 10

/* Helper for collision detection */
typedef struct {
    bool collided;            /* Whether collision occurred */
    vec3 contact_point;       /* Contact point in world space */
    vec3 contact_normal;      /* Contact normal pointing from first to second object */
    float penetration_depth;  /* Penetration depth */
} NexusCollisionInfo;

/* Forward declarations for internal functions */
static void nexus_physics_movement_system(ecs_iter_t *it);
static void nexus_physics_gravity_system(ecs_iter_t *it);
static void nexus_physics_integration_system(ecs_iter_t *it);
static void nexus_physics_collision_system(ecs_iter_t *it);
static void nexus_physics_constraint_system(ecs_iter_t *it);
static void nexus_physics_transform_update_system(ecs_iter_t *it);

/* Collision detection helpers */
static bool detect_sphere_sphere_collision(
    const vec3 pos_a, float radius_a,
    const vec3 pos_b, float radius_b,
    NexusCollisionInfo* collision_info);

static bool detect_box_box_collision(
    const vec3 pos_a, const vec3 half_extents_a, const mat4 rotation_a,
    const vec3 pos_b, const vec3 half_extents_b, const mat4 rotation_b,
    NexusCollisionInfo* collision_info);

static bool detect_sphere_box_collision(
    const vec3 sphere_pos, float sphere_radius,
    const vec3 box_pos, const vec3 box_half_extents, const mat4 box_rotation,
    NexusCollisionInfo* collision_info);

/**
 * Create a physics system
 */
NexusPhysics* nexus_physics_create(ecs_world_t* world) {
    if (world == NULL) {
        fprintf(stderr, "Cannot create physics system: NULL ECS world provided!\n");
        return NULL;
    }

    /* Allocate physics system structure */
    NexusPhysics* physics = (NexusPhysics*)malloc(sizeof(NexusPhysics));
    if (physics == NULL) {
        fprintf(stderr, "Failed to allocate memory for physics system!\n");
        return NULL;
    }

    /* Initialize physics system */
    memset(physics, 0, sizeof(NexusPhysics));

    /* Store ECS world reference */
    physics->world = world;

    /* Set default configuration */
    physics->config.gravity[0] = NEXUS_PHYSICS_DEFAULT_GRAVITY_X;
    physics->config.gravity[1] = NEXUS_PHYSICS_DEFAULT_GRAVITY_Y;
    physics->config.gravity[2] = NEXUS_PHYSICS_DEFAULT_GRAVITY_Z;
    physics->config.fixed_timestep = NEXUS_PHYSICS_DEFAULT_TIMESTEP;
    physics->config.max_substeps = NEXUS_PHYSICS_DEFAULT_MAX_SUBSTEPS;
    physics->config.debug_draw = false;

    /* Initialize accumulator */
    physics->accumulated_time = 0.0f;
    physics->paused = false;

    /* Register ECS systems */

    /* Create simplified ECS systems */
    /* Movement system only for now */
    ecs_entity_t movement_sys = ecs_new(world);
    
    ecs_system_desc_t movement_desc = {0};
    movement_desc.entity = movement_sys;
    movement_desc.callback = nexus_physics_movement_system;
    ecs_system_init(world, &movement_desc);

    /* Transform system */
    ecs_entity_t transform_sys = ecs_new(world);

    ecs_system_desc_t transform_desc = {0};
    transform_desc.entity = transform_sys;
    transform_desc.callback = nexus_physics_transform_update_system;
    ecs_system_init(world, &transform_desc);

    /* The rest of the physics systems will be implemented later */

    printf("Physics system created successfully.\n");

    return physics;
}

/**
 * Destroy a physics system
 */
void nexus_physics_destroy(NexusPhysics* physics) {
    if (physics == NULL) {
        return;
    }

    /* Free physics system structure */
    free(physics);

    printf("Physics system destroyed.\n");
}

/**
 * Update the physics system
 */
void nexus_physics_update(NexusPhysics* physics, float dt) {
    if (physics == NULL || physics->world == NULL || physics->paused) {
        return;
    }

    /* Update the physics world using fixed timestep */
    physics->accumulated_time += dt;
    physics->iteration_count = 0;

    /* Process fixed timestep updates */
    while (physics->accumulated_time >= physics->config.fixed_timestep &&
           physics->iteration_count < physics->config.max_substeps) {

        /* Set delta time for this iteration */
        ecs_set_time_scale(physics->world, 1.0);
        ecs_set_target_fps(physics->world, 1.0 / physics->config.fixed_timestep);

        /* Progress the ECS world for one physics timestep */
        ecs_progress(physics->world, 0);

        /* Update accumulator */
        physics->accumulated_time -= physics->config.fixed_timestep;
        physics->iteration_count++;
    }

    /* Handle remaining time if we've hit max substeps */
    if (physics->accumulated_time > 0 && physics->iteration_count >= physics->config.max_substeps) {
        /* Perform one more update with the remaining time */
        float remaining_dt = physics->accumulated_time;

        /* Set delta time for this iteration */
        ecs_set_time_scale(physics->world, 1.0);
        ecs_set_target_fps(physics->world, 1.0 / remaining_dt);

        /* Progress the ECS world for the remaining time */
        ecs_progress(physics->world, 0);

        /* Reset accumulator */
        physics->accumulated_time = 0;
    }
}

/**
 * Set gravity for the physics system
 */
void nexus_physics_set_gravity(NexusPhysics* physics, float x, float y, float z) {
    if (physics == NULL) {
        return;
    }

    physics->config.gravity[0] = x;
    physics->config.gravity[1] = y;
    physics->config.gravity[2] = z;
}

/**
 * Get gravity from the physics system
 */
void nexus_physics_get_gravity(const NexusPhysics* physics, vec3 gravity) {
    if (physics == NULL || gravity == NULL) {
        return;
    }

    gravity[0] = physics->config.gravity[0];
    gravity[1] = physics->config.gravity[1];
    gravity[2] = physics->config.gravity[2];
}

/**
 * Set fixed timestep for the physics system
 */
void nexus_physics_set_timestep(NexusPhysics* physics, float timestep) {
    if (physics == NULL || timestep <= 0.0f) {
        return;
    }

    physics->config.fixed_timestep = timestep;
}

/**
 * Get fixed timestep from the physics system
 */
float nexus_physics_get_timestep(const NexusPhysics* physics) {
    if (physics == NULL) {
        return NEXUS_PHYSICS_DEFAULT_TIMESTEP;
    }

    return physics->config.fixed_timestep;
}

/**
 * Pause or resume the physics system
 */
void nexus_physics_pause(NexusPhysics* physics, bool paused) {
    if (physics == NULL) {
        return;
    }

    physics->paused = paused;
}

/**
 * Check if the physics system is paused
 */
bool nexus_physics_is_paused(const NexusPhysics* physics) {
    if (physics == NULL) {
        return true;
    }

    return physics->paused;
}

/**
 * Enable or disable debug drawing
 */
void nexus_physics_set_debug_draw(NexusPhysics* physics, bool enabled) {
    if (physics == NULL) {
        return;
    }

    physics->config.debug_draw = enabled;
}

/**
 * Check if debug drawing is enabled
 */
bool nexus_physics_get_debug_draw(const NexusPhysics* physics) {
    if (physics == NULL) {
        return false;
    }

    return physics->config.debug_draw;
}

/**
 * Create a box collision shape
 */
NexusCollisionShape* nexus_collision_shape_create_box(float width, float height, float depth) {
    if (width <= 0.0f || height <= 0.0f || depth <= 0.0f) {
        fprintf(stderr, "Invalid box dimensions for collision shape!\n");
        return NULL;
    }

    /* Allocate collision shape structure */
    NexusCollisionShape* shape = (NexusCollisionShape*)malloc(sizeof(NexusCollisionShape));
    if (shape == NULL) {
        fprintf(stderr, "Failed to allocate memory for box collision shape!\n");
        return NULL;
    }

    /* Initialize box shape */
    shape->type = NEXUS_COLLISION_SHAPE_BOX;
    shape->data.box.half_extents[0] = width * 0.5f;
    shape->data.box.half_extents[1] = height * 0.5f;
    shape->data.box.half_extents[2] = depth * 0.5f;

    return shape;
}

/**
 * Create a sphere collision shape
 */
NexusCollisionShape* nexus_collision_shape_create_sphere(float radius) {
    if (radius <= 0.0f) {
        fprintf(stderr, "Invalid radius for sphere collision shape!\n");
        return NULL;
    }

    /* Allocate collision shape structure */
    NexusCollisionShape* shape = (NexusCollisionShape*)malloc(sizeof(NexusCollisionShape));
    if (shape == NULL) {
        fprintf(stderr, "Failed to allocate memory for sphere collision shape!\n");
        return NULL;
    }

    /* Initialize sphere shape */
    shape->type = NEXUS_COLLISION_SHAPE_SPHERE;
    shape->data.sphere.radius = radius;

    return shape;
}

/**
 * Collision detection helper: Sphere-Sphere collision
 */
static bool detect_sphere_sphere_collision(
    const vec3 pos_a, float radius_a,
    const vec3 pos_b, float radius_b,
    NexusCollisionInfo* collision_info) {

    /* Calculate distance between centers */
    vec3 direction;
    glm_vec3_sub(pos_b, pos_a, direction);
    float distance_sq = glm_vec3_norm2(direction);
    float radius_sum = radius_a + radius_b;

    /* Check for collision */
    if (distance_sq >= radius_sum * radius_sum) {
        return false; /* No collision */
    }

    /* Calculate collision info */
    float distance = sqrtf(distance_sq);

    /* Set collision info */
    if (collision_info != NULL) {
        collision_info->collided = true;

        /* Normalize direction */
        if (distance > 0.0001f) {
            glm_vec3_scale(direction, 1.0f / distance, collision_info->contact_normal);
        } else {
            /* Objects are at same position, use default normal */
            collision_info->contact_normal[0] = 0.0f;
            collision_info->contact_normal[1] = 1.0f;
            collision_info->contact_normal[2] = 0.0f;
        }

        /* Calculate contact point */
        vec3 offset;
        glm_vec3_scale(collision_info->contact_normal, radius_a, offset);
        glm_vec3_add(pos_a, offset, collision_info->contact_point);

        /* Calculate penetration depth */
        collision_info->penetration_depth = radius_sum - distance;
    }

    return true; /* Collision detected */
}

/**
 * Movement system implementation
 */
static void nexus_physics_movement_system(ecs_iter_t *it) {
    NexusPositionComponent *positions = ecs_field(it, NexusPositionComponent, 1);
    NexusVelocityComponent *velocities = ecs_field(it, NexusVelocityComponent, 2);

    /* Apply velocities to positions */
    for (int i = 0; i < it->count; i++) {
        /* Calculate position change based on velocity and delta time */
        vec3 delta;
        glm_vec3_scale(velocities[i].linear, it->delta_time, delta);

        /* Update position */
        glm_vec3_add(positions[i].value, delta, positions[i].value);
    }
}

/**
 * Transform update system implementation
 */
static void nexus_physics_transform_update_system(ecs_iter_t *it) {
    NexusPositionComponent *positions = ecs_field(it, NexusPositionComponent, 1);
    NexusRotationComponent *rotations = ecs_field(it, NexusRotationComponent, 2);
    NexusTransformComponent *transforms = ecs_field(it, NexusTransformComponent, 3);

    /* Update transform matrices */
    for (int i = 0; i < it->count; i++) {
        /* Mark transform as dirty */
        transforms[i].dirty = true;

        /* Create transform matrix from position and rotation */
        /* We assume scale is handled elsewhere (or is uniform 1.0) */
        mat4 pos_mat, rot_mat;

        /* Create translation matrix */
        glm_mat4_identity(pos_mat);
        pos_mat[3][0] = positions[i].value[0];
        pos_mat[3][1] = positions[i].value[1];
        pos_mat[3][2] = positions[i].value[2];

        /* Create rotation matrix from quaternion */
        glm_quat_mat4(rotations[i].quaternion, rot_mat);

        /* Combine matrices */
        glm_mat4_mul(pos_mat, rot_mat, transforms[i].local);

        /* For simplicity, we assume world = local
         * A full implementation would need to handle parent-child relationships */
        glm_mat4_copy(transforms[i].local, transforms[i].world);
    }
}

/**
 * Perform a raycast in the physics world
 */
bool nexus_physics_raycast(NexusPhysics* physics, const vec3 origin, const vec3 direction, float max_distance,
                         ecs_entity_t* hit_entity, vec3 hit_point, vec3 hit_normal, float* hit_distance) {
    if (physics == NULL || physics->world == NULL) {
        return false;
    }

    /* Initialize output parameters */
    if (hit_entity) *hit_entity = 0;
    if (hit_point) hit_point[0] = hit_point[1] = hit_point[2] = 0.0f;
    if (hit_normal) hit_normal[0] = hit_normal[1] = hit_normal[2] = 0.0f;
    if (hit_distance) *hit_distance = max_distance;

    /* Create a ray */
    NexusRay ray;
    nexus_ray_set(&ray, origin[0], origin[1], origin[2], direction[0], direction[1], direction[2]);

    /* Variables to track closest hit */
    float closest_distance = max_distance;
    ecs_entity_t closest_entity = 0;
    vec3 closest_point = {0.0f, 0.0f, 0.0f};
    vec3 closest_normal = {0.0f, 0.0f, 0.0f};
    bool hit_found = false;

    /* Create a query for all rigid bodies with position */
    ecs_query_desc_t query_desc = {0};
    ecs_query_t *query = ecs_query_init(physics->world, &query_desc);

    /* Iterate all potential collision candidates */
    ecs_iter_t it = ecs_query_iter(physics->world, query);
    while (ecs_query_next(&it)) {
        NexusPositionComponent *positions = ecs_field(&it, NexusPositionComponent, 1);
        NexusRigidBodyComponent *bodies = ecs_field(&it, NexusRigidBodyComponent, 2);

        for (int i = 0; i < it.count; i++) {
            ecs_entity_t entity = it.entities[i];
            vec3 position;
            float radius = 1.0f;  /* Default size for simple test */

            /* Get entity position */
            glm_vec3_copy(positions[i].value, position);

            /* Perform ray-sphere intersection test */
            float t;
            if (nexus_ray_sphere_intersect(&ray, position, radius, &t)) {
                if (t < closest_distance) {
                    /* This is the closest hit so far */
                    closest_distance = t;
                    closest_entity = entity;

                    /* Calculate hit point */
                    vec3 scaled_dir;
                    glm_vec3_scale(ray.direction, t, scaled_dir);
                    glm_vec3_add(ray.origin, scaled_dir, closest_point);

                    /* Calculate hit normal */
                    glm_vec3_sub(closest_point, position, closest_normal);
                    glm_vec3_normalize(closest_normal);

                    hit_found = true;
                }
            }
        }
    }

    /* Clean up */
    ecs_query_fini(query);

    /* Set output parameters if hit was found */
    if (hit_found) {
        if (hit_entity) *hit_entity = closest_entity;
        if (hit_point) glm_vec3_copy(closest_point, hit_point);
        if (hit_normal) glm_vec3_copy(closest_normal, hit_normal);
        if (hit_distance) *hit_distance = closest_distance;
    }

    return hit_found;
}
