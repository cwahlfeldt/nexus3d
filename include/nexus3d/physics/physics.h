/**
 * Nexus3D Physics System
 * Basic physics implementation using ECS
 */

#ifndef NEXUS3D_PHYSICS_H
#define NEXUS3D_PHYSICS_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include <flecs.h>

/**
 * Collision shape type enumeration
 */
typedef enum {
    NEXUS_COLLISION_SHAPE_BOX,
    NEXUS_COLLISION_SHAPE_SPHERE,
    NEXUS_COLLISION_SHAPE_CAPSULE,
    NEXUS_COLLISION_SHAPE_CYLINDER,
    NEXUS_COLLISION_SHAPE_CONE,
    NEXUS_COLLISION_SHAPE_CONVEX_HULL,
    NEXUS_COLLISION_SHAPE_MESH
} NexusCollisionShapeType;

/**
 * Collision shape structure
 */
typedef struct {
    NexusCollisionShapeType type;
    union {
        struct { vec3 half_extents; } box;
        struct { float radius; } sphere;
        struct { float radius; float height; } capsule;
        struct { float radius; float height; } cylinder;
        struct { float radius; float height; } cone;
        struct { void* vertices; int vertex_count; } convex_hull;
        struct { void* mesh_data; } mesh;
    } data;
} NexusCollisionShape;

/**
 * Physics material structure
 */
typedef struct {
    float friction;       /* Friction coefficient [0,1] */
    float restitution;    /* Restitution/bounciness coefficient [0,1] */
    float density;        /* Density kg/m^3 */
} NexusPhysicsMaterial;

/**
 * Physics system configuration
 */
typedef struct {
    vec3 gravity;          /* Gravity vector */
    float fixed_timestep;  /* Fixed timestep for physics simulation */
    int max_substeps;      /* Maximum number of physics substeps per frame */
    bool debug_draw;       /* Debug drawing flag */
} NexusPhysicsConfig;

/**
 * Main physics system structure
 */
typedef struct NexusPhysics {
    NexusPhysicsConfig config;    /* Physics configuration */
    float accumulated_time;        /* Accumulated simulation time */
    ecs_world_t* world;            /* ECS world reference */
    bool paused;                   /* Physics paused flag */
    int iteration_count;           /* Iteration count in current frame */
} NexusPhysics;

/* Physics system functions */
NexusPhysics* nexus_physics_create(ecs_world_t* world);
void nexus_physics_destroy(NexusPhysics* physics);
void nexus_physics_update(NexusPhysics* physics, float dt);
void nexus_physics_set_gravity(NexusPhysics* physics, float x, float y, float z);
void nexus_physics_get_gravity(const NexusPhysics* physics, vec3 gravity);
void nexus_physics_set_timestep(NexusPhysics* physics, float timestep);
float nexus_physics_get_timestep(const NexusPhysics* physics);
void nexus_physics_pause(NexusPhysics* physics, bool paused);
bool nexus_physics_is_paused(const NexusPhysics* physics);
void nexus_physics_set_debug_draw(NexusPhysics* physics, bool enabled);
bool nexus_physics_get_debug_draw(const NexusPhysics* physics);

/* Collision shape functions */
NexusCollisionShape* nexus_collision_shape_create_box(float width, float height, float depth);
NexusCollisionShape* nexus_collision_shape_create_sphere(float radius);
NexusCollisionShape* nexus_collision_shape_create_capsule(float radius, float height);
NexusCollisionShape* nexus_collision_shape_create_cylinder(float radius, float height);
NexusCollisionShape* nexus_collision_shape_create_cone(float radius, float height);
NexusCollisionShape* nexus_collision_shape_create_convex_hull(const float* vertices, int vertex_count);
void nexus_collision_shape_destroy(NexusCollisionShape* shape);

/* Physics material functions */
NexusPhysicsMaterial* nexus_physics_material_create(void);
void nexus_physics_material_destroy(NexusPhysicsMaterial* material);
void nexus_physics_material_set_friction(NexusPhysicsMaterial* material, float friction);
void nexus_physics_material_set_restitution(NexusPhysicsMaterial* material, float restitution);
void nexus_physics_material_set_density(NexusPhysicsMaterial* material, float density);

/* ECS component registration */
void nexus_physics_register_components(ecs_world_t* world);

/* Raycast functions */
bool nexus_physics_raycast(NexusPhysics* physics, const vec3 origin, const vec3 direction, float max_distance,
                         ecs_entity_t* hit_entity, vec3 hit_point, vec3 hit_normal, float* hit_distance);

#endif /* NEXUS3D_PHYSICS_H */
