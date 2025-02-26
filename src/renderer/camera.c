/**
 * Nexus3D Camera System Implementation
 * Handles camera management and transformation matrices
 */

#include "nexus3d/renderer/camera.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * Create a camera
 */
NexusCamera* nexus_camera_create(void) {
    /* Allocate camera structure */
    NexusCamera* camera = (NexusCamera*)malloc(sizeof(NexusCamera));
    if (camera == NULL) {
        printf("Failed to allocate memory for camera!\n");
        return NULL;
    }

    /* Initialize camera structure */
    memset(camera, 0, sizeof(NexusCamera));
    
    /* Set default values */
    camera->projection_type = NEXUS_CAMERA_PERSPECTIVE;
    
    /* Default position (0, 0, 5) looking at (0, 0, 0) */
    camera->position[0] = 0.0f;
    camera->position[1] = 0.0f;
    camera->position[2] = 5.0f;
    
    camera->target[0] = 0.0f;
    camera->target[1] = 0.0f;
    camera->target[2] = 0.0f;
    
    /* Default up vector (0, 1, 0) */
    camera->up[0] = 0.0f;
    camera->up[1] = 1.0f;
    camera->up[2] = 0.0f;
    
    /* Default projection parameters */
    camera->fov = 45.0f;
    camera->aspect_ratio = 16.0f / 9.0f;
    camera->near_plane = 0.1f;
    camera->far_plane = 1000.0f;
    
    /* Default orthographic parameters */
    camera->ortho_width = 10.0f;
    camera->ortho_height = 5.625f;  /* 16:9 aspect ratio */
    
    /* Set identity matrices */
    glm_mat4_identity(camera->view);
    glm_mat4_identity(camera->projection);
    glm_mat4_identity(camera->view_projection);
    
    /* Calculate initial matrices */
    camera->view_dirty = true;
    camera->projection_dirty = true;
    
    /* Calculate initial forward and right vectors */
    camera->forward[0] = 0.0f;
    camera->forward[1] = 0.0f;
    camera->forward[2] = -1.0f;
    
    camera->right[0] = 1.0f;
    camera->right[1] = 0.0f;
    camera->right[2] = 0.0f;
    
    /* Update camera matrices */
    nexus_camera_update(camera);
    
    return camera;
}

/**
 * Destroy a camera
 */
void nexus_camera_destroy(NexusCamera* camera) {
    if (camera == NULL) {
        return;
    }
    
    /* Free camera structure */
    free(camera);
}

/**
 * Set perspective projection
 */
void nexus_camera_set_perspective(NexusCamera* camera, float fov, float aspect_ratio, float near_plane, float far_plane) {
    if (camera == NULL) {
        return;
    }
    
    camera->projection_type = NEXUS_CAMERA_PERSPECTIVE;
    camera->fov = fov;
    camera->aspect_ratio = aspect_ratio;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;
    camera->projection_dirty = true;
}

/**
 * Set orthographic projection
 */
void nexus_camera_set_orthographic(NexusCamera* camera, float width, float height, float near_plane, float far_plane) {
    if (camera == NULL) {
        return;
    }
    
    camera->projection_type = NEXUS_CAMERA_ORTHOGRAPHIC;
    camera->ortho_width = width;
    camera->ortho_height = height;
    camera->near_plane = near_plane;
    camera->far_plane = far_plane;
    camera->projection_dirty = true;
}

/**
 * Set camera position
 */
void nexus_camera_set_position(NexusCamera* camera, float x, float y, float z) {
    if (camera == NULL) {
        return;
    }
    
    camera->position[0] = x;
    camera->position[1] = y;
    camera->position[2] = z;
    camera->view_dirty = true;
}

/**
 * Set camera target
 */
void nexus_camera_set_target(NexusCamera* camera, float x, float y, float z) {
    if (camera == NULL) {
        return;
    }
    
    camera->target[0] = x;
    camera->target[1] = y;
    camera->target[2] = z;
    
    /* Calculate forward vector */
    vec3 dir;
    glm_vec3_sub(camera->target, camera->position, dir);
    glm_vec3_normalize(dir);
    glm_vec3_copy(dir, camera->forward);
    
    /* Calculate right vector */
    glm_vec3_cross(camera->forward, camera->up, camera->right);
    glm_vec3_normalize(camera->right);
    
    /* Recalculate up vector to ensure orthogonality */
    glm_vec3_cross(camera->right, camera->forward, camera->up);
    glm_vec3_normalize(camera->up);
    
    camera->view_dirty = true;
}

/**
 * Set camera up vector
 */
void nexus_camera_set_up(NexusCamera* camera, float x, float y, float z) {
    if (camera == NULL) {
        return;
    }
    
    camera->up[0] = x;
    camera->up[1] = y;
    camera->up[2] = z;
    glm_vec3_normalize(camera->up);
    
    /* Recalculate right vector */
    glm_vec3_cross(camera->forward, camera->up, camera->right);
    glm_vec3_normalize(camera->right);
    
    camera->view_dirty = true;
}

/**
 * Look at a target position
 */
void nexus_camera_look_at(NexusCamera* camera, float x, float y, float z) {
    if (camera == NULL) {
        return;
    }
    
    camera->target[0] = x;
    camera->target[1] = y;
    camera->target[2] = z;
    
    camera->view_dirty = true;
}

/**
 * Set aspect ratio
 */
void nexus_camera_set_aspect_ratio(NexusCamera* camera, float aspect_ratio) {
    if (camera == NULL) {
        return;
    }
    
    camera->aspect_ratio = aspect_ratio;
    
    if (camera->projection_type == NEXUS_CAMERA_ORTHOGRAPHIC) {
        camera->ortho_width = camera->ortho_height * aspect_ratio;
    }
    
    camera->projection_dirty = true;
}

/**
 * Set field of view
 */
void nexus_camera_set_fov(NexusCamera* camera, float fov) {
    if (camera == NULL || camera->projection_type != NEXUS_CAMERA_PERSPECTIVE) {
        return;
    }
    
    camera->fov = fov;
    camera->projection_dirty = true;
}

/**
 * Update camera matrices
 */
void nexus_camera_update(NexusCamera* camera) {
    if (camera == NULL) {
        return;
    }
    
    /* Update view matrix if dirty */
    if (camera->view_dirty) {
        /* Calculate view matrix using look-at function */
        glm_lookat(camera->position, camera->target, camera->up, camera->view);
        
        /* Calculate forward, right and up vectors */
        vec3 dir;
        glm_vec3_sub(camera->target, camera->position, dir);
        glm_vec3_normalize(dir);
        glm_vec3_copy(dir, camera->forward);
        
        glm_vec3_cross(camera->forward, camera->up, camera->right);
        glm_vec3_normalize(camera->right);
        
        glm_vec3_cross(camera->right, camera->forward, camera->up);
        glm_vec3_normalize(camera->up);
        
        camera->view_dirty = false;
    }
    
    /* Update projection matrix if dirty */
    if (camera->projection_dirty) {
        if (camera->projection_type == NEXUS_CAMERA_PERSPECTIVE) {
            /* Calculate perspective projection matrix */
            glm_perspective(
                glm_rad(camera->fov),
                camera->aspect_ratio,
                camera->near_plane,
                camera->far_plane,
                camera->projection
            );
        } else {
            /* Calculate orthographic projection matrix */
            float half_width = camera->ortho_width * 0.5f;
            float half_height = camera->ortho_height * 0.5f;
            
            glm_ortho(
                -half_width, half_width,
                -half_height, half_height,
                camera->near_plane,
                camera->far_plane,
                camera->projection
            );
        }
        
        camera->projection_dirty = false;
    }
    
    /* Calculate view-projection matrix */
    glm_mat4_mul(camera->projection, camera->view, camera->view_projection);
}

/**
 * Get view matrix
 */
void nexus_camera_get_view_matrix(const NexusCamera* camera, float* matrix) {
    if (camera == NULL || matrix == NULL) {
        return;
    }
    
    memcpy(matrix, camera->view, sizeof(mat4));
}

/**
 * Get projection matrix
 */
void nexus_camera_get_projection_matrix(const NexusCamera* camera, float* matrix) {
    if (camera == NULL || matrix == NULL) {
        return;
    }
    
    memcpy(matrix, camera->projection, sizeof(mat4));
}

/**
 * Get view-projection matrix
 */
void nexus_camera_get_view_projection_matrix(const NexusCamera* camera, float* matrix) {
    if (camera == NULL || matrix == NULL) {
        return;
    }
    
    memcpy(matrix, camera->view_projection, sizeof(mat4));
}

/**
 * Get camera position
 */
void nexus_camera_get_position(const NexusCamera* camera, float* x, float* y, float* z) {
    if (camera == NULL) {
        if (x) *x = 0.0f;
        if (y) *y = 0.0f;
        if (z) *z = 0.0f;
        return;
    }
    
    if (x) *x = camera->position[0];
    if (y) *y = camera->position[1];
    if (z) *z = camera->position[2];
}

/**
 * Get camera forward vector
 */
void nexus_camera_get_forward(const NexusCamera* camera, float* x, float* y, float* z) {
    if (camera == NULL) {
        if (x) *x = 0.0f;
        if (y) *y = 0.0f;
        if (z) *z = -1.0f;
        return;
    }
    
    if (x) *x = camera->forward[0];
    if (y) *y = camera->forward[1];
    if (z) *z = camera->forward[2];
}

/**
 * Get camera right vector
 */
void nexus_camera_get_right(const NexusCamera* camera, float* x, float* y, float* z) {
    if (camera == NULL) {
        if (x) *x = 1.0f;
        if (y) *y = 0.0f;
        if (z) *z = 0.0f;
        return;
    }
    
    if (x) *x = camera->right[0];
    if (y) *y = camera->right[1];
    if (z) *z = camera->right[2];
}

/**
 * Get camera up vector
 */
void nexus_camera_get_up(const NexusCamera* camera, float* x, float* y, float* z) {
    if (camera == NULL) {
        if (x) *x = 0.0f;
        if (y) *y = 1.0f;
        if (z) *z = 0.0f;
        return;
    }
    
    if (x) *x = camera->up[0];
    if (y) *y = camera->up[1];
    if (z) *z = camera->up[2];
}

/**
 * Move camera forward
 */
void nexus_camera_move_forward(NexusCamera* camera, float distance) {
    if (camera == NULL) {
        return;
    }
    
    /* Move position along forward vector */
    camera->position[0] += camera->forward[0] * distance;
    camera->position[1] += camera->forward[1] * distance;
    camera->position[2] += camera->forward[2] * distance;
    
    /* Update target */
    camera->target[0] = camera->position[0] + camera->forward[0];
    camera->target[1] = camera->position[1] + camera->forward[1];
    camera->target[2] = camera->position[2] + camera->forward[2];
    
    camera->view_dirty = true;
}

/**
 * Move camera right
 */
void nexus_camera_move_right(NexusCamera* camera, float distance) {
    if (camera == NULL) {
        return;
    }
    
    /* Move position along right vector */
    camera->position[0] += camera->right[0] * distance;
    camera->position[1] += camera->right[1] * distance;
    camera->position[2] += camera->right[2] * distance;
    
    /* Update target */
    camera->target[0] = camera->position[0] + camera->forward[0];
    camera->target[1] = camera->position[1] + camera->forward[1];
    camera->target[2] = camera->position[2] + camera->forward[2];
    
    camera->view_dirty = true;
}

/**
 * Move camera up
 */
void nexus_camera_move_up(NexusCamera* camera, float distance) {
    if (camera == NULL) {
        return;
    }
    
    /* Move position along up vector */
    camera->position[0] += camera->up[0] * distance;
    camera->position[1] += camera->up[1] * distance;
    camera->position[2] += camera->up[2] * distance;
    
    /* Update target */
    camera->target[0] = camera->position[0] + camera->forward[0];
    camera->target[1] = camera->position[1] + camera->forward[1];
    camera->target[2] = camera->position[2] + camera->forward[2];
    
    camera->view_dirty = true;
}

/**
 * Rotate camera yaw (around up axis)
 */
void nexus_camera_rotate_yaw(NexusCamera* camera, float angle) {
    if (camera == NULL) {
        return;
    }
    
    /* Convert angle to radians */
    float radians = glm_rad(angle);
    
    /* Create rotation matrix */
    mat4 rotation;
    glm_mat4_identity(rotation);
    glm_rotate(rotation, radians, camera->up);
    
    /* Rotate forward vector */
    glm_mat4_mulv3(rotation, camera->forward, 1.0f, camera->forward);
    glm_vec3_normalize(camera->forward);
    
    /* Rotate right vector */
    glm_mat4_mulv3(rotation, camera->right, 1.0f, camera->right);
    glm_vec3_normalize(camera->right);
    
    /* Update target */
    camera->target[0] = camera->position[0] + camera->forward[0];
    camera->target[1] = camera->position[1] + camera->forward[1];
    camera->target[2] = camera->position[2] + camera->forward[2];
    
    camera->view_dirty = true;
}

/**
 * Rotate camera pitch (around right axis)
 */
void nexus_camera_rotate_pitch(NexusCamera* camera, float angle) {
    if (camera == NULL) {
        return;
    }
    
    /* Convert angle to radians */
    float radians = glm_rad(angle);
    
    /* Create rotation matrix */
    mat4 rotation;
    glm_mat4_identity(rotation);
    glm_rotate(rotation, radians, camera->right);
    
    /* Rotate forward vector */
    glm_mat4_mulv3(rotation, camera->forward, 1.0f, camera->forward);
    glm_vec3_normalize(camera->forward);
    
    /* Rotate up vector */
    glm_mat4_mulv3(rotation, camera->up, 1.0f, camera->up);
    glm_vec3_normalize(camera->up);
    
    /* Update target */
    camera->target[0] = camera->position[0] + camera->forward[0];
    camera->target[1] = camera->position[1] + camera->forward[1];
    camera->target[2] = camera->position[2] + camera->forward[2];
    
    camera->view_dirty = true;
}

/**
 * Rotate camera roll (around forward axis)
 */
void nexus_camera_rotate_roll(NexusCamera* camera, float angle) {
    if (camera == NULL) {
        return;
    }
    
    /* Convert angle to radians */
    float radians = glm_rad(angle);
    
    /* Create rotation matrix */
    mat4 rotation;
    glm_mat4_identity(rotation);
    glm_rotate(rotation, radians, camera->forward);
    
    /* Rotate up vector */
    glm_mat4_mulv3(rotation, camera->up, 1.0f, camera->up);
    glm_vec3_normalize(camera->up);
    
    /* Rotate right vector */
    glm_mat4_mulv3(rotation, camera->right, 1.0f, camera->right);
    glm_vec3_normalize(camera->right);
    
    camera->view_dirty = true;
}
