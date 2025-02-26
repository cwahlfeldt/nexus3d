/**
 * Nexus3D Mesh System Implementation
 * Handles 3D mesh data storage and rendering
 */

#include "nexus3d/renderer/mesh.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/**
 * Create a mesh
 */
NexusMesh* nexus_mesh_create(SDL_GPUDevice* device) {
    /* Check for null parameters */
    if (device == NULL) {
        fprintf(stderr, "GPU device cannot be NULL when creating mesh!");
        return NULL;
    }

    /* Allocate mesh structure */
    NexusMesh* mesh = (NexusMesh*)malloc(sizeof(NexusMesh));
    if (mesh == NULL) {
        fprintf(stderr, "Failed to allocate memory for mesh!");
        return NULL;
    }

    /* Initialize mesh structure */
    memset(mesh, 0, sizeof(NexusMesh));

    /* Store GPU device reference */
    mesh->device = device;

    /* Create transfer buffer for uploading vertex and index data */
    SDL_GPUTransferBufferCreateInfo transfer_info = {
        .usage = SDL_GPU_TRANSFERBUFFERUSAGE_UPLOAD,
        .size = 1024 * 1024  /* 1MB initial size */
    };

    mesh->transfer_buffer = SDL_CreateGPUTransferBuffer(device, &transfer_info);
    if (mesh->transfer_buffer == NULL) {
        fprintf(stderr, "Failed to create transfer buffer: %s\
", SDL_GetError());
        free(mesh);
        return NULL;
    }

    return mesh;
}

/**
 * Destroy a mesh
 */
void nexus_mesh_destroy(NexusMesh* mesh) {
    if (mesh == NULL) {
        return;
    }

    /* Release vertex buffer */
    if (mesh->vertex_buffer != NULL) {
        SDL_ReleaseGPUBuffer(mesh->device, mesh->vertex_buffer);
        mesh->vertex_buffer = NULL;
    }

    /* Release index buffer */
    if (mesh->index_buffer != NULL) {
        SDL_ReleaseGPUBuffer(mesh->device, mesh->index_buffer);
        mesh->index_buffer = NULL;
    }

    /* Release transfer buffer */
    if (mesh->transfer_buffer != NULL) {
        SDL_ReleaseGPUTransferBuffer(mesh->device, mesh->transfer_buffer);
        mesh->transfer_buffer = NULL;
    }

    /* Free mesh structure */
    free(mesh);
}

/**
 * Set vertex data for a mesh
 */
bool nexus_mesh_set_vertices(NexusMesh* mesh, const NexusVertex* vertices, uint32_t vertex_count) {
    if (mesh == NULL || vertices == NULL || vertex_count == 0) {
        return false;
    }

    /* Calculate vertex data size */
    uint32_t vertex_data_size = vertex_count * sizeof(NexusVertex);

    /* Map the transfer buffer */
    void* transfer_data = SDL_MapGPUTransferBuffer(mesh->device, mesh->transfer_buffer, true);
    if (transfer_data == NULL) {
        fprintf(stderr, "Failed to map transfer buffer: %s\
", SDL_GetError());
        return false;
    }

    /* Copy vertex data to transfer buffer */
    memcpy(transfer_data, vertices, vertex_data_size);

    /* Unmap the transfer buffer */
    SDL_UnmapGPUTransferBuffer(mesh->device, mesh->transfer_buffer);

    /* Create vertex buffer */
    SDL_GPUBufferCreateInfo buffer_info = {
        .size = vertex_data_size,
        .usage = SDL_GPU_BUFFERUSAGE_VERTEX
    };

    SDL_GPUBuffer* vertex_buffer = SDL_CreateGPUBuffer(mesh->device, &buffer_info);
    if (vertex_buffer == NULL) {
        fprintf(stderr, "Failed to create vertex buffer: %s\
", SDL_GetError());
        return false;
    }

    /* Begin a copy pass to upload data */
    SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(mesh->device);
    if (cmd_buffer == NULL) {
        fprintf(stderr, "Failed to acquire command buffer: %s\
", SDL_GetError());
        SDL_ReleaseGPUBuffer(mesh->device, vertex_buffer);
        return false;
    }

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd_buffer);
    if (copy_pass == NULL) {
        fprintf(stderr, "Failed to begin copy pass: %s\
", SDL_GetError());
        SDL_CancelGPUCommandBuffer(cmd_buffer);
        SDL_ReleaseGPUBuffer(mesh->device, vertex_buffer);
        return false;
    }

    /* Set up buffer upload */
    SDL_GPUTransferBufferLocation src_location = {
        .transfer_buffer = mesh->transfer_buffer,
        .offset = 0
    };

    SDL_GPUBufferRegion dst_region = {
        .buffer = vertex_buffer,
        .offset = 0,
        .size = vertex_data_size
    };

    /* Upload vertex data */
    SDL_UploadToGPUBuffer(copy_pass, &src_location, &dst_region, true);

    /* End copy pass */
    SDL_EndGPUCopyPass(copy_pass);

    /* Submit command buffer */
    SDL_SubmitGPUCommandBuffer(cmd_buffer);

    /* Release old vertex buffer if exists */
    if (mesh->vertex_buffer != NULL) {
        SDL_ReleaseGPUBuffer(mesh->device, mesh->vertex_buffer);
    }

    /* Store the new vertex buffer */
    mesh->vertex_buffer = vertex_buffer;
    mesh->vertex_count = vertex_count;

    return true;
}

/**
 * Set index data for a mesh
 */
bool nexus_mesh_set_indices(NexusMesh* mesh, const uint32_t* indices, uint32_t index_count) {
    if (mesh == NULL || indices == NULL || index_count == 0) {
        return false;
    }

    /* Calculate index data size */
    uint32_t index_data_size = index_count * sizeof(uint32_t);

    /* Map the transfer buffer */
    void* transfer_data = SDL_MapGPUTransferBuffer(mesh->device, mesh->transfer_buffer, true);
    if (transfer_data == NULL) {
        fprintf(stderr, "Failed to map transfer buffer: %s\
", SDL_GetError());
        return false;
    }

    /* Copy index data to transfer buffer */
    memcpy(transfer_data, indices, index_data_size);

    /* Unmap the transfer buffer */
    SDL_UnmapGPUTransferBuffer(mesh->device, mesh->transfer_buffer);

    /* Create index buffer */
    SDL_GPUBufferCreateInfo buffer_info = {
        .size = index_data_size,
        .usage = SDL_GPU_BUFFERUSAGE_INDEX
    };

    SDL_GPUBuffer* index_buffer = SDL_CreateGPUBuffer(mesh->device, &buffer_info);
    if (index_buffer == NULL) {
        fprintf(stderr, "Failed to create index buffer: %s\
", SDL_GetError());
        return false;
    }

    /* Begin a copy pass to upload data */
    SDL_GPUCommandBuffer* cmd_buffer = SDL_AcquireGPUCommandBuffer(mesh->device);
    if (cmd_buffer == NULL) {
        fprintf(stderr, "Failed to acquire command buffer: %s\
", SDL_GetError());
        SDL_ReleaseGPUBuffer(mesh->device, index_buffer);
        return false;
    }

    SDL_GPUCopyPass* copy_pass = SDL_BeginGPUCopyPass(cmd_buffer);
    if (copy_pass == NULL) {
        fprintf(stderr, "Failed to begin copy pass: %s\
", SDL_GetError());
        SDL_CancelGPUCommandBuffer(cmd_buffer);
        SDL_ReleaseGPUBuffer(mesh->device, index_buffer);
        return false;
    }

    /* Set up buffer upload */
    SDL_GPUTransferBufferLocation src_location = {
        .transfer_buffer = mesh->transfer_buffer,
        .offset = 0
    };

    SDL_GPUBufferRegion dst_region = {
        .buffer = index_buffer,
        .offset = 0,
        .size = index_data_size
    };

    /* Upload index data */
    SDL_UploadToGPUBuffer(copy_pass, &src_location, &dst_region, true);

    /* End copy pass */
    SDL_EndGPUCopyPass(copy_pass);

    /* Submit command buffer */
    SDL_SubmitGPUCommandBuffer(cmd_buffer);

    /* Release old index buffer if exists */
    if (mesh->index_buffer != NULL) {
        SDL_ReleaseGPUBuffer(mesh->device, mesh->index_buffer);
    }

    /* Store the new index buffer */
    mesh->index_buffer = index_buffer;
    mesh->index_count = index_count;
    mesh->has_indices = true;

    return true;
}

/**
 * Draw a mesh in a render pass
 * @return The number of triangles drawn
 */
uint32_t nexus_mesh_draw(NexusMesh* mesh, SDL_GPURenderPass* render_pass) {
    if (mesh == NULL || render_pass == NULL || mesh->vertex_buffer == NULL) {
        return 0;
    }

    /* Bind vertex buffer */
    SDL_GPUBufferBinding vertex_binding = {
        .buffer = mesh->vertex_buffer,
        .offset = 0
    };

    SDL_BindGPUVertexBuffers(render_pass, 0, &vertex_binding, 1);

    uint32_t triangle_count = 0;

    /* Draw the mesh */
    if (mesh->has_indices && mesh->index_buffer != NULL) {
        /* Bind index buffer */
        SDL_GPUBufferBinding index_binding = {
            .buffer = mesh->index_buffer,
            .offset = 0
        };

        SDL_BindGPUIndexBuffer(render_pass, &index_binding, SDL_GPU_INDEXELEMENTSIZE_32BIT);

        /* Draw indexed primitives */
        SDL_DrawGPUIndexedPrimitives(render_pass, mesh->index_count, 1, 0, 0, 0);
        
        /* Calculate triangle count (each 3 indices = 1 triangle) */
        triangle_count = mesh->index_count / 3;
    } else {
        /* Draw non-indexed primitives */
        SDL_DrawGPUPrimitives(render_pass, mesh->vertex_count, 1, 0, 0);
        
        /* Calculate triangle count (each 3 vertices = 1 triangle) */
        triangle_count = mesh->vertex_count / 3;
    }
    
    return triangle_count;
}

/**
 * Create a plane mesh
 */
NexusMesh* nexus_mesh_create_plane(SDL_GPUDevice* device, float width, float height, uint32_t width_segments, uint32_t height_segments) {
    if (device == NULL || width <= 0.0f || height <= 0.0f ||
        width_segments < 1 || height_segments < 1) {
        return NULL;
    }

    /* Ensure minimum segments */
    width_segments = width_segments < 1 ? 1 : width_segments;
    height_segments = height_segments < 1 ? 1 : height_segments;

    /* Calculate vertex count */
    uint32_t vertex_count = (width_segments + 1) * (height_segments + 1);

    /* Calculate index count (2 triangles per grid cell) */
    uint32_t index_count = width_segments * height_segments * 6;

    /* Allocate vertices */
    NexusVertex* vertices = (NexusVertex*)malloc(vertex_count * sizeof(NexusVertex));
    if (vertices == NULL) {
        fprintf(stderr, "Failed to allocate memory for plane vertices!");
        return NULL;
    }

    /* Allocate indices */
    uint32_t* indices = (uint32_t*)malloc(index_count * sizeof(uint32_t));
    if (indices == NULL) {
        fprintf(stderr, "Failed to allocate memory for plane indices!");
        free(vertices);
        return NULL;
    }

    /* Create the mesh */
    NexusMesh* mesh = nexus_mesh_create(device);
    if (mesh == NULL) {
        fprintf(stderr, "Failed to create plane mesh!");
        free(vertices);
        free(indices);
        return NULL;
    }

    /* Calculate dimensions */
    float half_width = width * 0.5f;
    float half_height = height * 0.5f;
    float segment_width = width / width_segments;
    float segment_height = height / height_segments;

    /* Generate vertices */
    uint32_t vertex_index = 0;
    for (uint32_t y = 0; y <= height_segments; y++) {
        float v = (float)y / height_segments;
        float pos_y = (v - 0.5f) * height;

        for (uint32_t x = 0; x <= width_segments; x++) {
            float u = (float)x / width_segments;
            float pos_x = (u - 0.5f) * width;

            /* Set position */
            vertices[vertex_index].position[0] = pos_x;
            vertices[vertex_index].position[1] = 0.0f;
            vertices[vertex_index].position[2] = pos_y;

            /* Set normal (pointing up) */
            vertices[vertex_index].normal[0] = 0.0f;
            vertices[vertex_index].normal[1] = 1.0f;
            vertices[vertex_index].normal[2] = 0.0f;

            /* Set texture coordinates */
            vertices[vertex_index].texcoord[0] = u;
            vertices[vertex_index].texcoord[1] = v;

            /* Set color (white) */
            vertices[vertex_index].color[0] = 1.0f;
            vertices[vertex_index].color[1] = 1.0f;
            vertices[vertex_index].color[2] = 1.0f;
            vertices[vertex_index].color[3] = 1.0f;

            vertex_index++;
        }
    }

    /* Generate indices */
    uint32_t index_index = 0;
    uint32_t width_vertex_count = width_segments + 1;

    for (uint32_t y = 0; y < height_segments; y++) {
        for (uint32_t x = 0; x < width_segments; x++) {
            uint32_t a = y * width_vertex_count + x;
            uint32_t b = a + 1;
            uint32_t c = a + width_vertex_count;
            uint32_t d = c + 1;

            /* First triangle */
            indices[index_index++] = a;
            indices[index_index++] = b;
            indices[index_index++] = c;

            /* Second triangle */
            indices[index_index++] = c;
            indices[index_index++] = b;
            indices[index_index++] = d;
        }
    }

    /* Set vertices and indices */
    if (!nexus_mesh_set_vertices(mesh, vertices, vertex_count)) {
        fprintf(stderr, "Failed to set plane vertices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    if (!nexus_mesh_set_indices(mesh, indices, index_count)) {
        fprintf(stderr, "Failed to set plane indices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    /* Free temporary data */
    free(vertices);
    free(indices);

    return mesh;
}

/**
 * Create a cube mesh
 */
NexusMesh* nexus_mesh_create_cube(SDL_GPUDevice* device, float size) {
    if (device == NULL || size <= 0.0f) {
        return NULL;
    }

    /* Calculate half size */
    float half_size = size * 0.5f;

    /* Cube has 24 vertices (4 per face, 6 faces) */
    uint32_t vertex_count = 24;

    /* Cube has 36 indices (6 per face, 6 faces) */
    uint32_t index_count = 36;

    /* Allocate vertices */
    NexusVertex* vertices = (NexusVertex*)malloc(vertex_count * sizeof(NexusVertex));
    if (vertices == NULL) {
        fprintf(stderr, "Failed to allocate memory for cube vertices!");
        return NULL;
    }

    /* Allocate indices */
    uint32_t* indices = (uint32_t*)malloc(index_count * sizeof(uint32_t));
    if (indices == NULL) {
        fprintf(stderr, "Failed to allocate memory for cube indices!");
        free(vertices);
        return NULL;
    }

    /* Create the mesh */
    NexusMesh* mesh = nexus_mesh_create(device);
    if (mesh == NULL) {
        fprintf(stderr, "Failed to create cube mesh!");
        free(vertices);
        free(indices);
        return NULL;
    }

    /* Generate vertices for each face */
    uint32_t vertex_index = 0;

    /* Positions for each face (using counter-clockwise winding) */
    float face_positions[6][4][3] = {
        /* Front face (z+) */
        {
            { -half_size, -half_size, half_size },
            { half_size, -half_size, half_size },
            { half_size, half_size, half_size },
            { -half_size, half_size, half_size }
        },
        /* Back face (z-) */
        {
            { half_size, -half_size, -half_size },
            { -half_size, -half_size, -half_size },
            { -half_size, half_size, -half_size },
            { half_size, half_size, -half_size }
        },
        /* Top face (y+) */
        {
            { -half_size, half_size, half_size },
            { half_size, half_size, half_size },
            { half_size, half_size, -half_size },
            { -half_size, half_size, -half_size }
        },
        /* Bottom face (y-) */
        {
            { -half_size, -half_size, -half_size },
            { half_size, -half_size, -half_size },
            { half_size, -half_size, half_size },
            { -half_size, -half_size, half_size }
        },
        /* Right face (x+) */
        {
            { half_size, -half_size, half_size },
            { half_size, -half_size, -half_size },
            { half_size, half_size, -half_size },
            { half_size, half_size, half_size }
        },
        /* Left face (x-) */
        {
            { -half_size, -half_size, -half_size },
            { -half_size, -half_size, half_size },
            { -half_size, half_size, half_size },
            { -half_size, half_size, -half_size }
        }
    };

    /* Normals for each face */
    float face_normals[6][3] = {
        { 0.0f, 0.0f, 1.0f },   /* Front face (z+) */
        { 0.0f, 0.0f, -1.0f },  /* Back face (z-) */
        { 0.0f, 1.0f, 0.0f },   /* Top face (y+) */
        { 0.0f, -1.0f, 0.0f },  /* Bottom face (y-) */
        { 1.0f, 0.0f, 0.0f },   /* Right face (x+) */
        { -1.0f, 0.0f, 0.0f }   /* Left face (x-) */
    };

    /* Generate vertices */
    for (uint32_t face = 0; face < 6; face++) {
        for (uint32_t v = 0; v < 4; v++) {
            /* Set position */
            vertices[vertex_index].position[0] = face_positions[face][v][0];
            vertices[vertex_index].position[1] = face_positions[face][v][1];
            vertices[vertex_index].position[2] = face_positions[face][v][2];

            /* Set normal */
            vertices[vertex_index].normal[0] = face_normals[face][0];
            vertices[vertex_index].normal[1] = face_normals[face][1];
            vertices[vertex_index].normal[2] = face_normals[face][2];

            /* Set texture coordinates */
            vertices[vertex_index].texcoord[0] = (v == 1 || v == 2) ? 1.0f : 0.0f;
            vertices[vertex_index].texcoord[1] = (v == 2 || v == 3) ? 1.0f : 0.0f;

            /* Set color (white) */
            vertices[vertex_index].color[0] = 1.0f;
            vertices[vertex_index].color[1] = 1.0f;
            vertices[vertex_index].color[2] = 1.0f;
            vertices[vertex_index].color[3] = 1.0f;

            vertex_index++;
        }
    }

    /* Generate indices */
    uint32_t index_index = 0;
    for (uint32_t face = 0; face < 6; face++) {
        uint32_t base_index = face * 4;

        /* First triangle */
        indices[index_index++] = base_index;
        indices[index_index++] = base_index + 1;
        indices[index_index++] = base_index + 2;

        /* Second triangle */
        indices[index_index++] = base_index;
        indices[index_index++] = base_index + 2;
        indices[index_index++] = base_index + 3;
    }

    /* Set vertices and indices */
    if (!nexus_mesh_set_vertices(mesh, vertices, vertex_count)) {
        fprintf(stderr, "Failed to set cube vertices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    if (!nexus_mesh_set_indices(mesh, indices, index_count)) {
        fprintf(stderr, "Failed to set cube indices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    /* Free temporary data */
    free(vertices);
    free(indices);

    return mesh;
}

/**
 * Create a sphere mesh
 */
NexusMesh* nexus_mesh_create_sphere(SDL_GPUDevice* device, float radius, uint32_t rings, uint32_t sectors) {
    if (device == NULL || radius <= 0.0f || rings < 2 || sectors < 3) {
        return NULL;
    }

    /* Calculate vertex count */
    uint32_t vertex_count = (rings + 1) * (sectors + 1);

    /* Calculate index count */
    uint32_t index_count = rings * sectors * 6;

    /* Allocate vertices */
    NexusVertex* vertices = (NexusVertex*)malloc(vertex_count * sizeof(NexusVertex));
    if (vertices == NULL) {
        fprintf(stderr, "Failed to allocate memory for sphere vertices!");
        return NULL;
    }

    /* Allocate indices */
    uint32_t* indices = (uint32_t*)malloc(index_count * sizeof(uint32_t));
    if (indices == NULL) {
        fprintf(stderr, "Failed to allocate memory for sphere indices!");
        free(vertices);
        return NULL;
    }

    /* Create the mesh */
    NexusMesh* mesh = nexus_mesh_create(device);
    if (mesh == NULL) {
        fprintf(stderr, "Failed to create sphere mesh!");
        free(vertices);
        free(indices);
        return NULL;
    }

    /* Generate vertices */
    uint32_t vertex_index = 0;
    for (uint32_t r = 0; r <= rings; r++) {
        float ring_ratio = (float)r / rings;
        float phi = ring_ratio * M_PI;

        for (uint32_t s = 0; s <= sectors; s++) {
            float sector_ratio = (float)s / sectors;
            float theta = sector_ratio * 2.0f * M_PI;

            /* Calculate position */
            float x = sinf(phi) * cosf(theta);
            float y = cosf(phi);
            float z = sinf(phi) * sinf(theta);

            /* Set position */
            vertices[vertex_index].position[0] = x * radius;
            vertices[vertex_index].position[1] = y * radius;
            vertices[vertex_index].position[2] = z * radius;

            /* Set normal (normalized position) */
            vertices[vertex_index].normal[0] = x;
            vertices[vertex_index].normal[1] = y;
            vertices[vertex_index].normal[2] = z;

            /* Set texture coordinates */
            vertices[vertex_index].texcoord[0] = sector_ratio;
            vertices[vertex_index].texcoord[1] = ring_ratio;

            /* Set color (white) */
            vertices[vertex_index].color[0] = 1.0f;
            vertices[vertex_index].color[1] = 1.0f;
            vertices[vertex_index].color[2] = 1.0f;
            vertices[vertex_index].color[3] = 1.0f;

            vertex_index++;
        }
    }

    /* Generate indices */
    uint32_t index_index = 0;
    uint32_t sectors_plus_one = sectors + 1;

    for (uint32_t r = 0; r < rings; r++) {
        for (uint32_t s = 0; s < sectors; s++) {
            uint32_t a = r * sectors_plus_one + s;
            uint32_t b = a + 1;
            uint32_t c = a + sectors_plus_one;
            uint32_t d = c + 1;

            /* First triangle */
            indices[index_index++] = a;
            indices[index_index++] = b;
            indices[index_index++] = c;

            /* Second triangle */
            indices[index_index++] = c;
            indices[index_index++] = b;
            indices[index_index++] = d;
        }
    }

    /* Set vertices and indices */
    if (!nexus_mesh_set_vertices(mesh, vertices, vertex_count)) {
        fprintf(stderr, "Failed to set sphere vertices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    if (!nexus_mesh_set_indices(mesh, indices, index_count)) {
        fprintf(stderr, "Failed to set sphere indices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    /* Free temporary data */
    free(vertices);
    free(indices);

    return mesh;
}

/**
 * Create a cylinder mesh
 */
NexusMesh* nexus_mesh_create_cylinder(SDL_GPUDevice* device, float radius, float height, uint32_t segments) {
    if (device == NULL || radius <= 0.0f || height <= 0.0f || segments < 3) {
        return NULL;
    }

    /* Calculate vertex count */
    uint32_t vertex_count = (segments + 1) * 2 + segments * 2;

    /* Calculate index count */
    uint32_t index_count = segments * 3 * 2 + segments * 6;

    /* Allocate vertices */
    NexusVertex* vertices = (NexusVertex*)malloc(vertex_count * sizeof(NexusVertex));
    if (vertices == NULL) {
        fprintf(stderr, "Failed to allocate memory for cylinder vertices!");
        return NULL;
    }

    /* Allocate indices */
    uint32_t* indices = (uint32_t*)malloc(index_count * sizeof(uint32_t));
    if (indices == NULL) {
        fprintf(stderr, "Failed to allocate memory for cylinder indices!");
        free(vertices);
        return NULL;
    }

    /* Create the mesh */
    NexusMesh* mesh = nexus_mesh_create(device);
    if (mesh == NULL) {
        fprintf(stderr, "Failed to create cylinder mesh!");
        free(vertices);
        free(indices);
        return NULL;
    }

    /* Calculate half height */
    float half_height = height * 0.5f;

    /* Generate vertices */
    uint32_t vertex_index = 0;

    /* Top and bottom circle centers */
    vertices[vertex_index].position[0] = 0.0f;
    vertices[vertex_index].position[1] = half_height;
    vertices[vertex_index].position[2] = 0.0f;
    vertices[vertex_index].normal[0] = 0.0f;
    vertices[vertex_index].normal[1] = 1.0f;
    vertices[vertex_index].normal[2] = 0.0f;
    vertices[vertex_index].texcoord[0] = 0.5f;
    vertices[vertex_index].texcoord[1] = 0.5f;
    vertices[vertex_index].color[0] = 1.0f;
    vertices[vertex_index].color[1] = 1.0f;
    vertices[vertex_index].color[2] = 1.0f;
    vertices[vertex_index].color[3] = 1.0f;

    uint32_t top_center_index = vertex_index++;

    vertices[vertex_index].position[0] = 0.0f;
    vertices[vertex_index].position[1] = -half_height;
    vertices[vertex_index].position[2] = 0.0f;
    vertices[vertex_index].normal[0] = 0.0f;
    vertices[vertex_index].normal[1] = -1.0f;
    vertices[vertex_index].normal[2] = 0.0f;
    vertices[vertex_index].texcoord[0] = 0.5f;
    vertices[vertex_index].texcoord[1] = 0.5f;
    vertices[vertex_index].color[0] = 1.0f;
    vertices[vertex_index].color[1] = 1.0f;
    vertices[vertex_index].color[2] = 1.0f;
    vertices[vertex_index].color[3] = 1.0f;

    uint32_t bottom_center_index = vertex_index++;

    // Generate the circles for top and bottom
    for (uint32_t i = 0; i <= segments; i++) {
        float ratio = (float)i / segments;
        float angle = ratio * 2.0f * M_PI;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;

        // Top circle vertex
        vertices[vertex_index].position[0] = x;
        vertices[vertex_index].position[1] = half_height;
        vertices[vertex_index].position[2] = z;
        vertices[vertex_index].normal[0] = 0.0f;
        vertices[vertex_index].normal[1] = 1.0f;
        vertices[vertex_index].normal[2] = 0.0f;
        vertices[vertex_index].texcoord[0] = x / (2.0f * radius) + 0.5f;
        vertices[vertex_index].texcoord[1] = z / (2.0f * radius) + 0.5f;
        vertices[vertex_index].color[0] = 1.0f;
        vertices[vertex_index].color[1] = 1.0f;
        vertices[vertex_index].color[2] = 1.0f;
        vertices[vertex_index].color[3] = 1.0f;
        vertex_index++;

        // Bottom circle vertex
        vertices[vertex_index].position[0] = x;
        vertices[vertex_index].position[1] = -half_height;
        vertices[vertex_index].position[2] = z;
        vertices[vertex_index].normal[0] = 0.0f;
        vertices[vertex_index].normal[1] = -1.0f;
        vertices[vertex_index].normal[2] = 0.0f;
        vertices[vertex_index].texcoord[0] = x / (2.0f * radius) + 0.5f;
        vertices[vertex_index].texcoord[1] = z / (2.0f * radius) + 0.5f;
        vertices[vertex_index].color[0] = 1.0f;
        vertices[vertex_index].color[1] = 1.0f;
        vertices[vertex_index].color[2] = 1.0f;
        vertices[vertex_index].color[3] = 1.0f;
        vertex_index++;
    }

    // Generate the cylinder side vertices
    uint32_t start_index = vertex_index;
    for (uint32_t i = 0; i <= segments; i++) {
        float ratio = (float)i / segments;
        float angle = ratio * 2.0f * M_PI;
        float x = cosf(angle) * radius;
        float z = sinf(angle) * radius;
        float nx = x / radius;
        float nz = z / radius;

        // Top side vertex
        vertices[vertex_index].position[0] = x;
        vertices[vertex_index].position[1] = half_height;
        vertices[vertex_index].position[2] = z;
        vertices[vertex_index].normal[0] = nx;
        vertices[vertex_index].normal[1] = 0.0f;
        vertices[vertex_index].normal[2] = nz;
        vertices[vertex_index].texcoord[0] = ratio;
        vertices[vertex_index].texcoord[1] = 1.0f;
        vertices[vertex_index].color[0] = 1.0f;
        vertices[vertex_index].color[1] = 1.0f;
        vertices[vertex_index].color[2] = 1.0f;
        vertices[vertex_index].color[3] = 1.0f;
        vertex_index++;

        // Bottom side vertex
        vertices[vertex_index].position[0] = x;
        vertices[vertex_index].position[1] = -half_height;
        vertices[vertex_index].position[2] = z;
        vertices[vertex_index].normal[0] = nx;
        vertices[vertex_index].normal[1] = 0.0f;
        vertices[vertex_index].normal[2] = nz;
        vertices[vertex_index].texcoord[0] = ratio;
        vertices[vertex_index].texcoord[1] = 0.0f;
        vertices[vertex_index].color[0] = 1.0f;
        vertices[vertex_index].color[1] = 1.0f;
        vertices[vertex_index].color[2] = 1.0f;
        vertices[vertex_index].color[3] = 1.0f;
        vertex_index++;
    }

    // Generate indices
    uint32_t index_index = 0;
    
    // Top circle indices
    uint32_t top_start_index = top_center_index + 1;
    for (uint32_t i = 0; i < segments; i++) {
        indices[index_index++] = top_center_index;
        indices[index_index++] = top_start_index + i;
        indices[index_index++] = top_start_index + i + 1;
    }

    // Bottom circle indices
    uint32_t bottom_start_index = bottom_center_index + 1;
    for (uint32_t i = 0; i < segments; i++) {
        indices[index_index++] = bottom_center_index;
        indices[index_index++] = bottom_start_index + i + 1;
        indices[index_index++] = bottom_start_index + i;
    }

    // Side indices
    for (uint32_t i = 0; i < segments; i++) {
        uint32_t a = start_index + i * 2;
        uint32_t b = start_index + i * 2 + 1;
        uint32_t c = start_index + i * 2 + 2;
        uint32_t d = start_index + i * 2 + 3;

        indices[index_index++] = a;
        indices[index_index++] = b;
        indices[index_index++] = c;

        indices[index_index++] = c;
        indices[index_index++] = b;
        indices[index_index++] = d;
    }

    // Set vertices and indices
    if (!nexus_mesh_set_vertices(mesh, vertices, vertex_count)) {
        fprintf(stderr, "Failed to set cylinder vertices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    if (!nexus_mesh_set_indices(mesh, indices, index_count)) {
        fprintf(stderr, "Failed to set cylinder indices!");
        free(vertices);
        free(indices);
        nexus_mesh_destroy(mesh);
        return NULL;
    }

    // Free temporary data
    free(vertices);
    free(indices);

    return mesh;
}
