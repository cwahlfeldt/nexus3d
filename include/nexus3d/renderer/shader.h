/**
 * Nexus3D Shader System
 * Handles shader compilation, loading, and management
 */

#ifndef NEXUS3D_SHADER_H
#define NEXUS3D_SHADER_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * Shader type enumeration
 */
typedef enum {
    NEXUS_SHADER_TYPE_VERTEX,
    NEXUS_SHADER_TYPE_FRAGMENT,
    NEXUS_SHADER_TYPE_COMPUTE
} NexusShaderType;

/**
 * Shader language enumeration
 */
typedef enum {
    NEXUS_SHADER_LANGUAGE_GLSL,    /* OpenGL Shading Language */
    NEXUS_SHADER_LANGUAGE_HLSL,    /* High-Level Shading Language (DirectX) */
    NEXUS_SHADER_LANGUAGE_MSL,     /* Metal Shading Language */
    NEXUS_SHADER_LANGUAGE_SPIRV,   /* Standard Portable Intermediate Representation V */
} NexusShaderLanguage;

/**
 * Shader structure
 */
typedef struct NexusShader {
    SDL_GPUShader* vertex_shader;      /* Vertex shader */
    SDL_GPUShader* fragment_shader;    /* Fragment shader */
    SDL_GPUGraphicsPipeline* pipeline; /* Graphics pipeline */
    SDL_GPUDevice* device;             /* GPU device reference */
    char name[64];                     /* Shader name */
} NexusShader;

/* Shader functions */
NexusShader* nexus_shader_create(SDL_GPUDevice* device, const char* name);
void nexus_shader_destroy(NexusShader* shader);
bool nexus_shader_load_from_source(NexusShader* shader, NexusShaderType type, NexusShaderLanguage language, const char* source);
bool nexus_shader_load_from_file(NexusShader* shader, NexusShaderType type, NexusShaderLanguage language, const char* filename);
bool nexus_shader_compile(NexusShader* shader);
void nexus_shader_bind(NexusShader* shader, SDL_GPURenderPass* render_pass);
void nexus_shader_set_uniform_float(NexusShader* shader, const char* name, float value);
void nexus_shader_set_uniform_float2(NexusShader* shader, const char* name, float x, float y);
void nexus_shader_set_uniform_float3(NexusShader* shader, const char* name, float x, float y, float z);
void nexus_shader_set_uniform_float4(NexusShader* shader, const char* name, float x, float y, float z, float w);
void nexus_shader_set_uniform_int(NexusShader* shader, const char* name, int value);
void nexus_shader_set_uniform_matrix4(NexusShader* shader, const char* name, const float* matrix);

#endif /* NEXUS3D_SHADER_H */
