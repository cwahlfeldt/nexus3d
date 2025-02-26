/**
 * Nexus3D Math Utilities Implementation
 * Provides convenient math functions, building on cglm
 */

#include "nexus3d/math/math_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <float.h>

/*
 * Note: Most of the math utility functions are implemented as inline functions
 * in the header file for improved performance. This file contains any non-inline
 * implementations that are too complex or would benefit from not being inlined.
 */

/* Additional ray-triangle intersection function (more complex, so not inlined) */
bool nexus_ray_triangle_intersect(const NexusRay* ray, 
                                  const vec3 v0, const vec3 v1, const vec3 v2, 
                                  float* out_t, float* out_u, float* out_v) {
    /* Implementation using Möller–Trumbore algorithm */
    vec3 edge1, edge2, h, s, q;
    float a, f, u, v;
    
    glm_vec3_sub(v1, v0, edge1);
    glm_vec3_sub(v2, v0, edge2);
    glm_vec3_cross(ray->direction, edge2, h);
    
    a = glm_vec3_dot(edge1, h);
    
    /* Check if ray is parallel to triangle */
    if (a > -NEXUS_EPSILON && a < NEXUS_EPSILON) {
        return false;
    }
    
    f = 1.0f / a;
    glm_vec3_sub(ray->origin, v0, s);
    u = f * glm_vec3_dot(s, h);
    
    /* Check if intersection is outside the triangle */
    if (u < 0.0f || u > 1.0f) {
        return false;
    }
    
    glm_vec3_cross(s, edge1, q);
    v = f * glm_vec3_dot(ray->direction, q);
    
    /* Check if intersection is outside the triangle */
    if (v < 0.0f || u + v > 1.0f) {
        return false;
    }
    
    /* Calculate t */
    float t = f * glm_vec3_dot(edge2, q);
    
    /* Check if intersection is behind the ray */
    if (t < NEXUS_EPSILON) {
        return false;
    }
    
    /* Store results */
    if (out_t != NULL) *out_t = t;
    if (out_u != NULL) *out_u = u;
    if (out_v != NULL) *out_v = v;
    
    return true;
}

/* Ray-OBB (Oriented Bounding Box) intersection */
bool nexus_ray_obb_intersect(const NexusRay* ray, 
                            const vec3 center, const vec3 half_size, const mat4 orientation, 
                            float* out_t) {
    /* Transform ray to OBB's local space */
    vec3 local_origin, local_dir;
    mat4 inv_orientation;
    
    /* Calculate inverse orientation */
    glm_mat4_inv(orientation, inv_orientation);
    
    /* Calculate ray in local space */
    vec4 origin4 = {ray->origin[0], ray->origin[1], ray->origin[2], 1.0f};
    vec4 direction4 = {ray->direction[0], ray->direction[1], ray->direction[2], 0.0f};
    
    vec4 local_origin4, local_dir4;
    glm_mat4_mulv(inv_orientation, origin4, local_origin4);
    glm_mat4_mulv(inv_orientation, direction4, local_dir4);
    
    /* Copy back to vec3 */
    glm_vec3(local_origin4, local_origin);
    glm_vec3(local_dir4, local_dir);
    
    /* Normalize local direction */
    glm_vec3_normalize(local_dir);
    
    /* Now we can use AABB intersection in local space */
    vec3 min_point, max_point;
    glm_vec3_sub(center, half_size, min_point);
    glm_vec3_add(center, half_size, max_point);
    
    /* Create a local ray */
    NexusRay local_ray;
    glm_vec3_copy(local_origin, local_ray.origin);
    glm_vec3_copy(local_dir, local_ray.direction);
    
    /* Perform AABB intersection */
    return nexus_ray_aabb_intersect(&local_ray, min_point, max_point, out_t);
}

/* Spherical Linear Interpolation (SLERP) between quaternions */
void nexus_quat_slerp(const versor q1, const versor q2, float t, versor result) {
    /* Use cglm's implementation */
    glm_quat_slerp(q1, q2, t, result);
}

/* Compute a frustum from view-projection matrix */
void nexus_frustum_from_viewproj(const mat4 view_proj, vec4 planes[6]) {
    /* Extract planes from view-projection matrix */
    /* Left plane */
    planes[0][0] = view_proj[0][3] + view_proj[0][0];
    planes[0][1] = view_proj[1][3] + view_proj[1][0];
    planes[0][2] = view_proj[2][3] + view_proj[2][0];
    planes[0][3] = view_proj[3][3] + view_proj[3][0];
    
    /* Right plane */
    planes[1][0] = view_proj[0][3] - view_proj[0][0];
    planes[1][1] = view_proj[1][3] - view_proj[1][0];
    planes[1][2] = view_proj[2][3] - view_proj[2][0];
    planes[1][3] = view_proj[3][3] - view_proj[3][0];
    
    /* Bottom plane */
    planes[2][0] = view_proj[0][3] + view_proj[0][1];
    planes[2][1] = view_proj[1][3] + view_proj[1][1];
    planes[2][2] = view_proj[2][3] + view_proj[2][1];
    planes[2][3] = view_proj[3][3] + view_proj[3][1];
    
    /* Top plane */
    planes[3][0] = view_proj[0][3] - view_proj[0][1];
    planes[3][1] = view_proj[1][3] - view_proj[1][1];
    planes[3][2] = view_proj[2][3] - view_proj[2][1];
    planes[3][3] = view_proj[3][3] - view_proj[3][1];
    
    /* Near plane */
    planes[4][0] = view_proj[0][3] + view_proj[0][2];
    planes[4][1] = view_proj[1][3] + view_proj[1][2];
    planes[4][2] = view_proj[2][3] + view_proj[2][2];
    planes[4][3] = view_proj[3][3] + view_proj[3][2];
    
    /* Far plane */
    planes[5][0] = view_proj[0][3] - view_proj[0][2];
    planes[5][1] = view_proj[1][3] - view_proj[1][2];
    planes[5][2] = view_proj[2][3] - view_proj[2][2];
    planes[5][3] = view_proj[3][3] - view_proj[3][2];
    
    /* Normalize all planes */
    for (int i = 0; i < 6; i++) {
        float length = sqrtf(planes[i][0] * planes[i][0] + 
                           planes[i][1] * planes[i][1] + 
                           planes[i][2] * planes[i][2]);
        planes[i][0] /= length;
        planes[i][1] /= length;
        planes[i][2] /= length;
        planes[i][3] /= length;
    }
}

/* Check if a sphere is inside or intersecting a frustum */
bool nexus_frustum_contains_sphere(const vec4 planes[6], const vec3 center, float radius) {
    for (int i = 0; i < 6; i++) {
        float distance = planes[i][0] * center[0] + 
                        planes[i][1] * center[1] + 
                        planes[i][2] * center[2] + 
                        planes[i][3];
        
        if (distance < -radius) {
            return false; /* Sphere is completely outside the frustum */
        }
    }
    
    return true; /* Sphere is at least partially inside the frustum */
}

/* Check if an AABB is inside or intersecting a frustum */
bool nexus_frustum_contains_aabb(const vec4 planes[6], const vec3 min_point, const vec3 max_point) {
    /* For each plane, check if the box is completely outside */
    for (int i = 0; i < 6; i++) {
        /* Find the positive vertex (P-vertex) */
        vec3 p_vertex;
        
        p_vertex[0] = (planes[i][0] > 0.0f) ? max_point[0] : min_point[0];
        p_vertex[1] = (planes[i][1] > 0.0f) ? max_point[1] : min_point[1];
        p_vertex[2] = (planes[i][2] > 0.0f) ? max_point[2] : min_point[2];
        
        /* Find the negative vertex (N-vertex) */
        vec3 n_vertex;
        
        n_vertex[0] = (planes[i][0] > 0.0f) ? min_point[0] : max_point[0];
        n_vertex[1] = (planes[i][1] > 0.0f) ? min_point[1] : max_point[1];
        n_vertex[2] = (planes[i][2] > 0.0f) ? min_point[2] : max_point[2];
        
        /* If the negative vertex is outside, the box is completely outside */
        float distance = planes[i][0] * n_vertex[0] + 
                        planes[i][1] * n_vertex[1] + 
                        planes[i][2] * n_vertex[2] + 
                        planes[i][3];
        
        if (distance < 0.0f) {
            return false;
        }
    }
    
    return true; /* AABB is at least partially inside the frustum */
}

/* Catmull-Rom spline interpolation */
void nexus_catmull_rom(const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3, float t, vec3 result) {
    float t2 = t * t;
    float t3 = t2 * t;
    
    /* Catmull-Rom matrix coefficients */
    float b0 = -0.5f * t3 + t2 - 0.5f * t;
    float b1 = 1.5f * t3 - 2.5f * t2 + 1.0f;
    float b2 = -1.5f * t3 + 2.0f * t2 + 0.5f * t;
    float b3 = 0.5f * t3 - 0.5f * t2;
    
    /* Interpolate each component */
    for (int i = 0; i < 3; i++) {
        result[i] = b0 * p0[i] + b1 * p1[i] + b2 * p2[i] + b3 * p3[i];
    }
}

/* Cubic Bézier curve interpolation */
void nexus_bezier_cubic(const vec3 p0, const vec3 p1, const vec3 p2, const vec3 p3, float t, vec3 result) {
    float t2 = t * t;
    float t3 = t2 * t;
    float mt = 1.0f - t;
    float mt2 = mt * mt;
    float mt3 = mt2 * mt;
    
    /* Bernstein polynomials */
    float b0 = mt3;
    float b1 = 3.0f * mt2 * t;
    float b2 = 3.0f * mt * t2;
    float b3 = t3;
    
    /* Interpolate each component */
    for (int i = 0; i < 3; i++) {
        result[i] = b0 * p0[i] + b1 * p1[i] + b2 * p2[i] + b3 * p3[i];
    }
}

/* Perlin noise generation (1D) */
float nexus_perlin_1d(float x) {
    /* Simple 1D Perlin noise implementation */
    int x0 = (int)floorf(x);
    int x1 = x0 + 1;
    float dx = x - (float)x0;
    
    /* Smooth interpolation */
    float s = dx * dx * (3.0f - 2.0f * dx);
    
    /* Generate random gradients */
    float g0 = ((x0 * 1664525 + 1013904223) & 0x7FFFFFFF) / (float)0x40000000 - 1.0f;
    float g1 = ((x1 * 1664525 + 1013904223) & 0x7FFFFFFF) / (float)0x40000000 - 1.0f;
    
    /* Calculate dot products */
    float n0 = g0 * (dx - 0.0f);
    float n1 = g1 * (dx - 1.0f);
    
    /* Interpolate */
    return 0.5f + 0.5f * (n0 + s * (n1 - n0));
}

/* Bilinear interpolation for a 2D grid */
float nexus_bilinear_interpolate(float v00, float v10, float v01, float v11, float u, float v) {
    /* First interpolate along x-axis */
    float v0 = nexus_lerp(v00, v10, u);
    float v1 = nexus_lerp(v01, v11, u);
    
    /* Then interpolate along y-axis */
    return nexus_lerp(v0, v1, v);
}

/* Hermite interpolation */
float nexus_hermite(float y0, float y1, float y2, float y3, float mu) {
    float mu2 = mu * mu;
    float mu3 = mu2 * mu;
    
    float m0, m1;
    
    /* Calculate tangents */
    m0 = (y2 - y0) * 0.5f;
    m1 = (y3 - y1) * 0.5f;
    
    /* Calculate coefficients */
    float a0 = 2.0f * mu3 - 3.0f * mu2 + 1.0f;
    float a1 = mu3 - 2.0f * mu2 + mu;
    float a2 = mu3 - mu2;
    float a3 = -2.0f * mu3 + 3.0f * mu2;
    
    /* Calculate interpolated value */
    return a0 * y1 + a1 * m0 + a2 * m1 + a3 * y2;
}

/* Calculate barycentric coordinates for a point in a triangle */
void nexus_barycentric_coords(const vec3 p, const vec3 a, const vec3 b, const vec3 c, vec3 coords) {
    vec3 v0, v1, v2;
    glm_vec3_sub(b, a, v0);
    glm_vec3_sub(c, a, v1);
    glm_vec3_sub(p, a, v2);
    
    float d00 = glm_vec3_dot(v0, v0);
    float d01 = glm_vec3_dot(v0, v1);
    float d11 = glm_vec3_dot(v1, v1);
    float d20 = glm_vec3_dot(v2, v0);
    float d21 = glm_vec3_dot(v2, v1);
    
    float denom = d00 * d11 - d01 * d01;
    
    coords[1] = (d11 * d20 - d01 * d21) / denom;  /* Beta */
    coords[2] = (d00 * d21 - d01 * d20) / denom;  /* Gamma */
    coords[0] = 1.0f - coords[1] - coords[2];    /* Alpha */
}

/* Calculate the distance from a point to a line segment */
float nexus_point_segment_distance(const vec3 point, const vec3 segment_start, const vec3 segment_end) {
    vec3 line_dir, point_to_start, projection;
    float line_length_sq, t;
    
    /* Calculate line direction and length squared */
    glm_vec3_sub(segment_end, segment_start, line_dir);
    line_length_sq = glm_vec3_dot(line_dir, line_dir);
    
    /* Handle degenerate line segment */
    if (line_length_sq < NEXUS_EPSILON) {
        return glm_vec3_distance(point, segment_start);
    }
    
    /* Calculate projection of point onto line */
    glm_vec3_sub(point, segment_start, point_to_start);
    t = glm_vec3_dot(point_to_start, line_dir) / line_length_sq;
    
    /* Clamp t to [0, 1] for line segment */
    t = (t < 0.0f) ? 0.0f : ((t > 1.0f) ? 1.0f : t);
    
    /* Calculate projection point on segment */
    projection[0] = segment_start[0] + t * line_dir[0];
    projection[1] = segment_start[1] + t * line_dir[1];
    projection[2] = segment_start[2] + t * line_dir[2];
    
    /* Return distance from point to projection */
    return glm_vec3_distance(point, projection);
}

/* Create a look-at rotation quaternion */
void nexus_quat_look_at(const vec3 direction, const vec3 up, versor result) {
    /* Create a coordinate system from direction and up */
    vec3 forward, right, local_up;
    
    /* Forward is the negative of direction (camera looks down -Z) */
    glm_vec3_negate_to(direction, forward);
    glm_vec3_normalize(forward);
    
    /* Right is the cross product of up and forward */
    glm_vec3_cross(up, forward, right);
    glm_vec3_normalize(right);
    
    /* Recalculate up as the cross product of forward and right */
    glm_vec3_cross(forward, right, local_up);
    
    /* Create rotation matrix from the coordinate system */
    mat3 rotation_matrix;
    glm_vec3_copy(right, rotation_matrix[0]);
    glm_vec3_copy(local_up, rotation_matrix[1]);
    glm_vec3_copy(forward, rotation_matrix[2]);
    
    /* Convert rotation matrix to quaternion */
    glm_mat3_quat(rotation_matrix, result);
}

/* Compute bounce vector (reflection) */
void nexus_reflect(const vec3 incident, const vec3 normal, vec3 result) {
    float dot = glm_vec3_dot(incident, normal);
    
    /* Calculate reflection: I - 2.0 * (N·I) * N */
    for (int i = 0; i < 3; i++) {
        result[i] = incident[i] - 2.0f * dot * normal[i];
    }
}

/* Compute refraction vector using Snell's law */
bool nexus_refract(const vec3 incident, const vec3 normal, float ior_ratio, vec3 result) {
    float dot = glm_vec3_dot(incident, normal);
    float k = 1.0f - ior_ratio * ior_ratio * (1.0f - dot * dot);
    
    if (k < 0.0f) {
        /* Total internal reflection, no refraction */
        return false;
    }
    
    /* Calculate refraction: ior_ratio * I - (ior_ratio * (N·I) + sqrt(k)) * N */
    for (int i = 0; i < 3; i++) {
        result[i] = ior_ratio * incident[i] - (ior_ratio * dot + sqrtf(k)) * normal[i];
    }
    
    return true;
}
