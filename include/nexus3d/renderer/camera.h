/**
 * Nexus3D Camera System
 * Handles camera management and transformation matrices
 */

#ifndef NEXUS3D_CAMERA_H
#define NEXUS3D_CAMERA_H

#include <stdbool.h>
#include <cglm/cglm.h>

/**
 * Camera projection type
 */
typedef enum {
    NEXUS_CAMERA_PERSPECTIVE,
    NEXUS_CAMERA_ORTHOGRAPHIC
} NexusCameraProjection;

/**
 * Camera structure
 */
typedef struct NexusCamera {
    NexusCameraProjection projection_type;  /* Projection type */
    
    /* Camera properties */
    vec3 position;                 /* Camera position */
    vec3 target;                   /* Camera target/look-at point */
    vec3 up;                       /* Camera up vector */
    vec3 right;                    /* Camera right vector */
    vec3 forward;                  /* Camera forward vector */
    
    /* Perspective projection properties */
    float fov;                     /* Field of view in degrees */
    float aspect_ratio;            /* Aspect ratio (width/height) */
    float near_plane;              /* Near clipping plane */
    float far_plane;               /* Far clipping plane */
    
    /* Orthographic projection properties */
    float ortho_width;             /* Orthographic width */
    float ortho_height;            /* Orthographic height */
    
    /* Matrices */
    mat4 view;                     /* View matrix */
    mat4 projection;               /* Projection matrix */
    mat4 view_projection;          /* Combined view-projection matrix */
    
    /* Cache */
    bool view_dirty;               /* View matrix needs recalculation */
    bool projection_dirty;         /* Projection matrix needs recalculation */
} NexusCamera;

/* Camera functions */
NexusCamera* nexus_camera_create(void);
void nexus_camera_destroy(NexusCamera* camera);
void nexus_camera_set_perspective(NexusCamera* camera, float fov, float aspect_ratio, float near_plane, float far_plane);
void nexus_camera_set_orthographic(NexusCamera* camera, float width, float height, float near_plane, float far_plane);
void nexus_camera_set_position(NexusCamera* camera, float x, float y, float z);
void nexus_camera_set_target(NexusCamera* camera, float x, float y, float z);
void nexus_camera_set_up(NexusCamera* camera, float x, float y, float z);
void nexus_camera_look_at(NexusCamera* camera, float x, float y, float z);
void nexus_camera_set_aspect_ratio(NexusCamera* camera, float aspect_ratio);
void nexus_camera_set_fov(NexusCamera* camera, float fov);
void nexus_camera_update(NexusCamera* camera);
void nexus_camera_get_view_matrix(const NexusCamera* camera, float* matrix);
void nexus_camera_get_projection_matrix(const NexusCamera* camera, float* matrix);
void nexus_camera_get_view_projection_matrix(const NexusCamera* camera, float* matrix);
void nexus_camera_get_position(const NexusCamera* camera, float* x, float* y, float* z);
void nexus_camera_get_forward(const NexusCamera* camera, float* x, float* y, float* z);
void nexus_camera_get_right(const NexusCamera* camera, float* x, float* y, float* z);
void nexus_camera_get_up(const NexusCamera* camera, float* x, float* y, float* z);

/* Camera movement functions */
void nexus_camera_move_forward(NexusCamera* camera, float distance);
void nexus_camera_move_right(NexusCamera* camera, float distance);
void nexus_camera_move_up(NexusCamera* camera, float distance);
void nexus_camera_rotate_yaw(NexusCamera* camera, float angle);
void nexus_camera_rotate_pitch(NexusCamera* camera, float angle);
void nexus_camera_rotate_roll(NexusCamera* camera, float angle);

#endif /* NEXUS3D_CAMERA_H */
