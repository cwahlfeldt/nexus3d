/**
 * Nexus3D ECS Components
 * Component definitions for the Entity Component System
 */

#ifndef NEXUS3D_COMPONENTS_H
#define NEXUS3D_COMPONENTS_H

#include <cglm/cglm.h>
#include <flecs.h>
#include "nexus3d/renderer/mesh.h"
#include "nexus3d/renderer/material.h"
#include "nexus3d/renderer/camera.h"

/**
 * Position component
 */
typedef struct {
    vec3 value;          /* Position in world space (x, y, z) */
} NexusPositionComponent;

/**
 * Rotation component
 */
typedef struct {
    vec3 euler;          /* Euler angles in degrees (pitch, yaw, roll) */
    versor quaternion;   /* Rotation as quaternion */
} NexusRotationComponent;

/**
 * Scale component
 */
typedef struct {
    vec3 value;          /* Scale in each axis (x, y, z) */
} NexusScaleComponent;

/**
 * Transform component
 */
typedef struct {
    mat4 local;          /* Local transformation matrix */
    mat4 world;          /* World transformation matrix */
    bool dirty;          /* Flag indicating if matrices need recalculation */
} NexusTransformComponent;

/**
 * Renderable component
 */
typedef struct {
    NexusMesh* mesh;     /* Mesh to render */
    NexusMaterial* material;  /* Material to use for rendering */
    bool visible;        /* Visibility flag */
    bool cast_shadows;   /* Whether the entity casts shadows */
    bool receive_shadows; /* Whether the entity receives shadows */
} NexusRenderableComponent;

/**
 * Camera component
 */
typedef struct {
    NexusCamera* camera; /* Camera data */
    bool primary;        /* Whether this is the primary camera */
    bool active;         /* Whether the camera is active */
} NexusCameraComponent;

/**
 * Light type enumeration
 */
typedef enum {
    NEXUS_LIGHT_DIRECTIONAL,  /* Directional light */
    NEXUS_LIGHT_POINT,        /* Point light */
    NEXUS_LIGHT_SPOT          /* Spot light */
} NexusLightType;

/**
 * Light component
 */
typedef struct {
    NexusLightType type;      /* Light type */
    vec3 color;               /* Light color */
    float intensity;          /* Light intensity */
    float range;              /* Light range (point and spot lights) */
    float spot_angle;         /* Spotlight angle in degrees (spot lights) */
    float spot_softness;      /* Spotlight edge softness (spot lights) */
    bool cast_shadows;        /* Whether the light casts shadows */
    int shadow_resolution;    /* Shadow map resolution */
} NexusLightComponent;

/**
 * Velocity component
 */
typedef struct {
    vec3 linear;              /* Linear velocity */
    vec3 angular;             /* Angular velocity */
} NexusVelocityComponent;

/**
 * Physics rigid body component
 */
typedef struct {
    float mass;               /* Mass in kg */
    float restitution;        /* Bounciness factor */
    float friction;           /* Friction coefficient */
    bool kinematic;           /* Whether the body is kinematic */
    bool trigger;             /* Whether the body is a trigger */
    bool sleeping;            /* Whether the body is sleeping */
} NexusRigidBodyComponent;

/**
 * Audio source component
 */
typedef struct {
    char sound_id[64];        /* Sound identifier */
    float volume;             /* Volume (0.0 - 1.0) */
    float pitch;              /* Pitch multiplier */
    float spatial_blend;      /* 2D/3D sound blend (0.0 = 2D, 1.0 = 3D) */
    float min_distance;       /* Distance where attenuation starts */
    float max_distance;       /* Distance where attenuation ends */
    bool loop;                /* Whether the sound should loop */
    bool playing;             /* Whether the sound is playing */
    bool auto_play;           /* Whether the sound should auto-play */
} NexusAudioSourceComponent;

/**
 * Tag components (empty structures)
 */
typedef struct { char _unused; } NexusStaticTag;      /* Static object that doesn't move */
typedef struct { char _unused; } NexusDynamicTag;     /* Dynamic object that moves */
typedef struct { char _unused; } NexusMainCameraTag;  /* Tag for the main camera */

/* ECS component registration */
void nexus_ecs_register_components(ecs_world_t* world);

#endif /* NEXUS3D_COMPONENTS_H */
