/**
 * Nexus3D Math Utilities
 * Provides convenient math functions, building on cglm
 */

#ifndef NEXUS3D_MATH_UTILS_H
#define NEXUS3D_MATH_UTILS_H

#include <cglm/cglm.h>
#include <stdbool.h>
#include <math.h>

/* Constants */
#define NEXUS_PI 3.14159265358979323846f
#define NEXUS_TWO_PI (2.0f * NEXUS_PI)
#define NEXUS_HALF_PI (NEXUS_PI / 2.0f)
#define NEXUS_QUARTER_PI (NEXUS_PI / 4.0f)
#define NEXUS_DEG_TO_RAD (NEXUS_PI / 180.0f)
#define NEXUS_RAD_TO_DEG (180.0f / NEXUS_PI)
#define NEXUS_EPSILON 0.000001f

/**
 * Vector3 utilities
 */

/* Create a vector3 */
static inline void nexus_vec3_set(vec3 v, float x, float y, float z) {
    v[0] = x;
    v[1] = y;
    v[2] = z;
}

/* Copy a vector3 */
static inline void nexus_vec3_copy(const vec3 src, vec3 dst) {
    dst[0] = src[0];
    dst[1] = src[1];
    dst[2] = src[2];
}

/* Create a zero vector3 */
static inline void nexus_vec3_zero(vec3 v) {
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
}

/* Create a vector3 with all components set to the same value */
static inline void nexus_vec3_set_all(vec3 v, float value) {
    v[0] = value;
    v[1] = value;
    v[2] = value;
}

/* Create unit vectors */
static inline void nexus_vec3_unit_x(vec3 v) {
    v[0] = 1.0f;
    v[1] = 0.0f;
    v[2] = 0.0f;
}

static inline void nexus_vec3_unit_y(vec3 v) {
    v[0] = 0.0f;
    v[1] = 1.0f;
    v[2] = 0.0f;
}

static inline void nexus_vec3_unit_z(vec3 v) {
    v[0] = 0.0f;
    v[1] = 0.0f;
    v[2] = 1.0f;
}

/* Vector3 equality test with epsilon */
static inline bool nexus_vec3_equals(const vec3 a, const vec3 b, float epsilon) {
    return (fabsf(a[0] - b[0]) <= epsilon) &&
           (fabsf(a[1] - b[1]) <= epsilon) &&
           (fabsf(a[2] - b[2]) <= epsilon);
}

/**
 * Quaternion utilities
 */

/* Create a quaternion from axis and angle */
static inline void nexus_quat_from_axis_angle(versor q, float x, float y, float z, float angle_radians) {
    vec3 axis = {x, y, z};
    glm_vec3_normalize(axis);
    glm_quatv(q, angle_radians, axis);
}

/* Create a quaternion from Euler angles (in degrees) */
static inline void nexus_quat_from_euler_degrees(versor q, float pitch, float yaw, float roll) {
    glm_euler_xyz((vec3){pitch * NEXUS_DEG_TO_RAD, yaw * NEXUS_DEG_TO_RAD, roll * NEXUS_DEG_TO_RAD}, q);
}

/* Convert a quaternion to Euler angles (in degrees) */
static inline void nexus_quat_to_euler_degrees(const versor q, vec3 euler) {
    float temp[3];
    glm_euler_angles(q, temp);
    euler[0] = temp[0] * NEXUS_RAD_TO_DEG; // Pitch
    euler[1] = temp[1] * NEXUS_RAD_TO_DEG; // Yaw
    euler[2] = temp[2] * NEXUS_RAD_TO_DEG; // Roll
}

/**
 * Matrix utilities
 */

/* Create a translation matrix */
static inline void nexus_mat4_translation(mat4 m, float x, float y, float z) {
    glm_mat4_identity(m);
    m[3][0] = x;
    m[3][1] = y;
    m[3][2] = z;
}

/* Create a scaling matrix */
static inline void nexus_mat4_scaling(mat4 m, float x, float y, float z) {
    glm_mat4_identity(m);
    m[0][0] = x;
    m[1][1] = y;
    m[2][2] = z;
}

/* Create a rotation matrix from Euler angles (in degrees) */
static inline void nexus_mat4_rotation_from_euler_degrees(mat4 m, float pitch, float yaw, float roll) {
    glm_mat4_identity(m);
    versor q;
    nexus_quat_from_euler_degrees(q, pitch, yaw, roll);
    glm_quat_rotate(m, q, m);
}

/* Create a transform matrix from position, rotation (Euler angles in degrees), and scale */
static inline void nexus_mat4_transform(mat4 m, const vec3 position, const vec3 rotation, const vec3 scale) {
    glm_mat4_identity(m);
    
    // Scale
    glm_scale(m, scale);
    
    // Rotation
    versor q;
    nexus_quat_from_euler_degrees(q, rotation[0], rotation[1], rotation[2]);
    mat4 rot_mat;
    glm_quat_mat4(q, rot_mat);
    glm_mul_rot(m, rot_mat, m);
    
    // Translation
    m[3][0] = position[0];
    m[3][1] = position[1];
    m[3][2] = position[2];
}

/* Extract position from transform matrix */
static inline void nexus_mat4_get_position(const mat4 m, vec3 position) {
    position[0] = m[3][0];
    position[1] = m[3][1];
    position[2] = m[3][2];
}

/* Extract rotation (as quaternion) from transform matrix */
static inline void nexus_mat4_get_rotation(const mat4 m, versor rotation) {
    mat3 rot_mat;
    glm_mat4_pick3(m, rot_mat);
    glm_mat3_quat(rot_mat, rotation);
}

/* Extract scale from transform matrix */
static inline void nexus_mat4_get_scale(const mat4 m, vec3 scale) {
    scale[0] = glm_vec3_norm((vec3){m[0][0], m[0][1], m[0][2]});
    scale[1] = glm_vec3_norm((vec3){m[1][0], m[1][1], m[1][2]});
    scale[2] = glm_vec3_norm((vec3){m[2][0], m[2][1], m[2][2]});
}

/**
 * Angle utilities
 */

/* Clamp angle to [0, 360) range */
static inline float nexus_angle_normalize_360(float degrees) {
    degrees = fmodf(degrees, 360.0f);
    if (degrees < 0.0f)
        degrees += 360.0f;
    return degrees;
}

/* Clamp angle to [-180, 180) range */
static inline float nexus_angle_normalize_180(float degrees) {
    degrees = fmodf(degrees + 180.0f, 360.0f);
    if (degrees < 0.0f)
        degrees += 360.0f;
    return degrees - 180.0f;
}

/* Linear interpolation */
static inline float nexus_lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

/* Smoothstep interpolation */
static inline float nexus_smoothstep(float a, float b, float t) {
    t = t * t * (3.0f - 2.0f * t);
    return a + (b - a) * t;
}

/**
 * Utility for Ray casting and intersection
 */

/* Ray structure */
typedef struct {
    vec3 origin;    /* Ray origin */
    vec3 direction; /* Ray direction (normalized) */
} NexusRay;

/* Create a ray */
static inline void nexus_ray_set(NexusRay* ray, float ox, float oy, float oz, float dx, float dy, float dz) {
    ray->origin[0] = ox;
    ray->origin[1] = oy;
    ray->origin[2] = oz;
    ray->direction[0] = dx;
    ray->direction[1] = dy;
    ray->direction[2] = dz;
    glm_vec3_normalize(ray->direction);
}

/* Ray-plane intersection */
static inline bool nexus_ray_plane_intersect(const NexusRay* ray, const vec3 plane_normal, float plane_distance, float* out_t) {
    float denom = glm_vec3_dot(ray->direction, plane_normal);
    
    if (fabsf(denom) < NEXUS_EPSILON) {
        return false; // Ray is parallel to the plane
    }
    
    *out_t = -(glm_vec3_dot(ray->origin, plane_normal) + plane_distance) / denom;
    return *out_t >= 0.0f; // Intersection is in the positive direction of the ray
}

/* Ray-sphere intersection */
static inline bool nexus_ray_sphere_intersect(const NexusRay* ray, const vec3 center, float radius, float* out_t) {
    vec3 oc;
    glm_vec3_sub(ray->origin, center, oc);
    
    float a = glm_vec3_dot(ray->direction, ray->direction);
    float b = 2.0f * glm_vec3_dot(oc, ray->direction);
    float c = glm_vec3_dot(oc, oc) - radius * radius;
    
    float discriminant = b * b - 4.0f * a * c;
    
    if (discriminant < 0.0f) {
        return false; // No intersection
    }
    
    discriminant = sqrtf(discriminant);
    
    // Find the nearest intersection in the positive direction
    float t1 = (-b - discriminant) / (2.0f * a);
    float t2 = (-b + discriminant) / (2.0f * a);
    
    if (t1 >= 0.0f) {
        *out_t = t1;
        return true;
    }
    
    if (t2 >= 0.0f) {
        *out_t = t2;
        return true;
    }
    
    return false; // Both intersections are behind the ray
}

/* Ray-AABB intersection */
static inline bool nexus_ray_aabb_intersect(const NexusRay* ray, const vec3 min, const vec3 max, float* out_t) {
    float tmin = 0.0f;
    float tmax = FLT_MAX;
    
    for (int i = 0; i < 3; i++) {
        if (fabsf(ray->direction[i]) < NEXUS_EPSILON) {
            // Ray is parallel to slab, check if origin is inside slab
            if (ray->origin[i] < min[i] || ray->origin[i] > max[i]) {
                return false;
            }
        } else {
            // Compute intersection with slab
            float invD = 1.0f / ray->direction[i];
            float t1 = (min[i] - ray->origin[i]) * invD;
            float t2 = (max[i] - ray->origin[i]) * invD;
            
            // Ensure t1 <= t2
            if (t1 > t2) {
                float temp = t1;
                t1 = t2;
                t2 = temp;
            }
            
            tmin = t1 > tmin ? t1 : tmin;
            tmax = t2 < tmax ? t2 : tmax;
            
            if (tmin > tmax) {
                return false;
            }
        }
    }
    
    *out_t = tmin;
    return tmin >= 0.0f; // Intersection is in positive direction of ray
}

#endif /* NEXUS3D_MATH_UTILS_H */
