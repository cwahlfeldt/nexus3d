/**
 * Nexus3D Mesh System
 * Handles 3D mesh data storage and rendering
 */

#ifndef NEXUS3D_MESH_H
#define NEXUS3D_MESH_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * Vertex structure
 */
typedef struct {
    float position[3];  /* x, y, z */
    float normal[3];    /* nx, ny, nz */
    float texcoord[2];  /* u, v */
    float color[4];     /* r, g, b, a */
} NexusVertex;

/**
 * Mesh structure
 */
typedef struct NexusMesh {
    SDL_GPUBuffer* vertex_buffer;      /* Vertex buffer */
    SDL_GPUBuffer* index_buffer;       /* Index buffer */
    uint32_t vertex_count;             /* Number of vertices */
    uint32_t index_count;              /* Number of indices */
    SDL_GPUDevice* device;             /* GPU device reference */
    SDL_GPUTransferBuffer* transfer_buffer; /* Transfer buffer for uploads */
    bool has_indices;                  /* Whether the mesh has indices */
} NexusMesh;

/* Mesh functions */
NexusMesh* nexus_mesh_create(SDL_GPUDevice* device);
void nexus_mesh_destroy(NexusMesh* mesh);
bool nexus_mesh_set_vertices(NexusMesh* mesh, const NexusVertex* vertices, uint32_t vertex_count);
bool nexus_mesh_set_indices(NexusMesh* mesh, const uint32_t* indices, uint32_t index_count);
void nexus_mesh_draw(NexusMesh* mesh, SDL_GPURenderPass* render_pass);

/* Primitive creation functions */
NexusMesh* nexus_mesh_create_plane(SDL_GPUDevice* device, float width, float height, uint32_t width_segments, uint32_t height_segments);
NexusMesh* nexus_mesh_create_cube(SDL_GPUDevice* device, float size);
NexusMesh* nexus_mesh_create_sphere(SDL_GPUDevice* device, float radius, uint32_t rings, uint32_t sectors);
NexusMesh* nexus_mesh_create_cylinder(SDL_GPUDevice* device, float radius, float height, uint32_t segments);
NexusMesh* nexus_mesh_create_cone(SDL_GPUDevice* device, float radius, float height, uint32_t segments);
NexusMesh* nexus_mesh_create_torus(SDL_GPUDevice* device, float radius, float tube_radius, uint32_t radial_segments, uint32_t tubular_segments);

/* Mesh loading functions */
NexusMesh* nexus_mesh_load_obj(SDL_GPUDevice* device, const char* filename);

#endif /* NEXUS3D_MESH_H */
