/**
 * Nexus3D Material System
 * Handles material properties and shader parameters
 */

#ifndef NEXUS3D_MATERIAL_H
#define NEXUS3D_MATERIAL_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "nexus3d/renderer/shader.h"
#include "nexus3d/renderer/texture.h"

/**
 * Material blend mode
 */
typedef enum {
    NEXUS_BLEND_MODE_OPAQUE,       /* No blending */
    NEXUS_BLEND_MODE_ALPHA,        /* Alpha blending */
    NEXUS_BLEND_MODE_ADDITIVE,     /* Additive blending */
    NEXUS_BLEND_MODE_MULTIPLY      /* Multiply blending */
} NexusBlendMode;

/**
 * Material structure
 */
typedef struct NexusMaterial {
    NexusShader* shader;           /* Material shader */
    
    /* Base properties */
    float base_color[4];           /* Base color (RGBA) */
    NexusTexture* albedo_map;      /* Albedo texture */
    NexusTexture* normal_map;      /* Normal map texture */
    NexusTexture* metallic_map;    /* Metallic texture */
    NexusTexture* roughness_map;   /* Roughness texture */
    NexusTexture* ao_map;          /* Ambient occlusion texture */
    
    /* PBR properties */
    float metallic;                /* Metallic factor [0,1] */
    float roughness;               /* Roughness factor [0,1] */
    float ao;                      /* Ambient occlusion factor [0,1] */
    float emissive_factor[3];      /* Emissive factor (RGB) */
    NexusTexture* emissive_map;    /* Emissive texture */
    
    /* Rendering properties */
    NexusBlendMode blend_mode;     /* Blend mode */
    bool two_sided;                /* Render both sides */
    bool wireframe;                /* Render in wireframe mode */
    bool cast_shadows;             /* Material casts shadows */
    bool receive_shadows;          /* Material receives shadows */
    
    /* References */
    SDL_GPUDevice* device;         /* GPU device reference */
    char name[64];                 /* Material name */
} NexusMaterial;

/* Material functions */
NexusMaterial* nexus_material_create(SDL_GPUDevice* device, const char* name);
void nexus_material_destroy(NexusMaterial* material);
void nexus_material_set_shader(NexusMaterial* material, NexusShader* shader);
void nexus_material_set_base_color(NexusMaterial* material, float r, float g, float b, float a);
void nexus_material_set_metallic(NexusMaterial* material, float metallic);
void nexus_material_set_roughness(NexusMaterial* material, float roughness);
void nexus_material_set_ao(NexusMaterial* material, float ao);
void nexus_material_set_emissive(NexusMaterial* material, float r, float g, float b);
void nexus_material_set_albedo_map(NexusMaterial* material, NexusTexture* texture);
void nexus_material_set_normal_map(NexusMaterial* material, NexusTexture* texture);
void nexus_material_set_metallic_map(NexusMaterial* material, NexusTexture* texture);
void nexus_material_set_roughness_map(NexusMaterial* material, NexusTexture* texture);
void nexus_material_set_ao_map(NexusMaterial* material, NexusTexture* texture);
void nexus_material_set_emissive_map(NexusMaterial* material, NexusTexture* texture);
void nexus_material_set_blend_mode(NexusMaterial* material, NexusBlendMode blend_mode);
void nexus_material_set_two_sided(NexusMaterial* material, bool two_sided);
void nexus_material_set_wireframe(NexusMaterial* material, bool wireframe);
void nexus_material_set_cast_shadows(NexusMaterial* material, bool cast_shadows);
void nexus_material_set_receive_shadows(NexusMaterial* material, bool receive_shadows);
void nexus_material_apply(NexusMaterial* material, SDL_GPURenderPass* render_pass);

#endif /* NEXUS3D_MATERIAL_H */
