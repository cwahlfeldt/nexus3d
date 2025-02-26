/**
 * Nexus3D Material System Implementation
 * Handles material properties and shader parameters
 */

#include "nexus3d/renderer/material.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a material
 */
NexusMaterial* nexus_material_create(SDL_GPUDevice* device, const char* name) {
    /* Check for null parameters */
    if (device == NULL) {
        fprintf(stderr, "GPU device cannot be NULL when creating material!\n");
        return NULL;
    }
    
    /* Allocate material structure */
    NexusMaterial* material = (NexusMaterial*)malloc(sizeof(NexusMaterial));
    if (material == NULL) {
        fprintf(stderr, "Failed to allocate memory for material!\n");
        return NULL;
    }
    
    /* Initialize material structure */
    memset(material, 0, sizeof(NexusMaterial));
    
    /* Store GPU device reference */
    material->device = device;
    
    /* Set material name */
    if (name != NULL) {
        strncpy(material->name, name, sizeof(material->name) - 1);
        material->name[sizeof(material->name) - 1] = '\0';
    } else {
        strcpy(material->name, "Unnamed Material");
    }
    
    /* Set default properties */
    material->base_color[0] = 1.0f;
    material->base_color[1] = 1.0f;
    material->base_color[2] = 1.0f;
    material->base_color[3] = 1.0f;
    
    material->metallic = 0.0f;
    material->roughness = 0.5f;
    material->ao = 1.0f;
    
    material->emissive_factor[0] = 0.0f;
    material->emissive_factor[1] = 0.0f;
    material->emissive_factor[2] = 0.0f;
    
    material->blend_mode = NEXUS_BLEND_MODE_OPAQUE;
    material->two_sided = false;
    material->wireframe = false;
    material->cast_shadows = true;
    material->receive_shadows = true;
    
    return material;
}

/**
 * Destroy a material
 */
void nexus_material_destroy(NexusMaterial* material) {
    if (material == NULL) {
        return;
    }
    
    /* Note: We don't destroy the shader or textures here as they might be
     * shared between multiple materials. The resource management system should
     * handle their lifecycle. */
    
    /* Free material structure */
    free(material);
}

/**
 * Set material shader
 */
void nexus_material_set_shader(NexusMaterial* material, NexusShader* shader) {
    if (material == NULL) {
        return;
    }
    
    material->shader = shader;
}

/**
 * Set base color
 */
void nexus_material_set_base_color(NexusMaterial* material, float r, float g, float b, float a) {
    if (material == NULL) {
        return;
    }
    
    material->base_color[0] = r;
    material->base_color[1] = g;
    material->base_color[2] = b;
    material->base_color[3] = a;
}

/**
 * Set metallic factor
 */
void nexus_material_set_metallic(NexusMaterial* material, float metallic) {
    if (material == NULL) {
        return;
    }
    
    material->metallic = metallic;
}

/**
 * Set roughness factor
 */
void nexus_material_set_roughness(NexusMaterial* material, float roughness) {
    if (material == NULL) {
        return;
    }
    
    material->roughness = roughness;
}

/**
 * Set ambient occlusion factor
 */
void nexus_material_set_ao(NexusMaterial* material, float ao) {
    if (material == NULL) {
        return;
    }
    
    material->ao = ao;
}

/**
 * Set emissive factor
 */
void nexus_material_set_emissive(NexusMaterial* material, float r, float g, float b) {
    if (material == NULL) {
        return;
    }
    
    material->emissive_factor[0] = r;
    material->emissive_factor[1] = g;
    material->emissive_factor[2] = b;
}

/**
 * Set albedo texture
 */
void nexus_material_set_albedo_map(NexusMaterial* material, NexusTexture* texture) {
    if (material == NULL) {
        return;
    }
    
    material->albedo_map = texture;
}

/**
 * Set normal map texture
 */
void nexus_material_set_normal_map(NexusMaterial* material, NexusTexture* texture) {
    if (material == NULL) {
        return;
    }
    
    material->normal_map = texture;
}

/**
 * Set metallic texture
 */
void nexus_material_set_metallic_map(NexusMaterial* material, NexusTexture* texture) {
    if (material == NULL) {
        return;
    }
    
    material->metallic_map = texture;
}

/**
 * Set roughness texture
 */
void nexus_material_set_roughness_map(NexusMaterial* material, NexusTexture* texture) {
    if (material == NULL) {
        return;
    }
    
    material->roughness_map = texture;
}

/**
 * Set ambient occlusion texture
 */
void nexus_material_set_ao_map(NexusMaterial* material, NexusTexture* texture) {
    if (material == NULL) {
        return;
    }
    
    material->ao_map = texture;
}

/**
 * Set emissive texture
 */
void nexus_material_set_emissive_map(NexusMaterial* material, NexusTexture* texture) {
    if (material == NULL) {
        return;
    }
    
    material->emissive_map = texture;
}

/**
 * Set blend mode
 */
void nexus_material_set_blend_mode(NexusMaterial* material, NexusBlendMode blend_mode) {
    if (material == NULL) {
        return;
    }
    
    material->blend_mode = blend_mode;
}

/**
 * Set two-sided rendering
 */
void nexus_material_set_two_sided(NexusMaterial* material, bool two_sided) {
    if (material == NULL) {
        return;
    }
    
    material->two_sided = two_sided;
}

/**
 * Set wireframe rendering
 */
void nexus_material_set_wireframe(NexusMaterial* material, bool wireframe) {
    if (material == NULL) {
        return;
    }
    
    material->wireframe = wireframe;
}

/**
 * Set shadow casting
 */
void nexus_material_set_cast_shadows(NexusMaterial* material, bool cast_shadows) {
    if (material == NULL) {
        return;
    }
    
    material->cast_shadows = cast_shadows;
}

/**
 * Set shadow receiving
 */
void nexus_material_set_receive_shadows(NexusMaterial* material, bool receive_shadows) {
    if (material == NULL) {
        return;
    }
    
    material->receive_shadows = receive_shadows;
}

/**
 * Apply material to a render pass
 */
void nexus_material_apply(NexusMaterial* material, SDL_GPURenderPass* render_pass) {
    if (material == NULL || render_pass == NULL || material->shader == NULL) {
        return;
    }
    
    /* Bind shader pipeline */
    nexus_shader_bind(material->shader, render_pass);
    
    /* Set material properties as shader uniforms */
    nexus_shader_set_uniform_float4(material->shader, "u_baseColor", 
                                   material->base_color[0], 
                                   material->base_color[1], 
                                   material->base_color[2], 
                                   material->base_color[3]);
    
    nexus_shader_set_uniform_float(material->shader, "u_metallic", material->metallic);
    nexus_shader_set_uniform_float(material->shader, "u_roughness", material->roughness);
    nexus_shader_set_uniform_float(material->shader, "u_ao", material->ao);
    
    nexus_shader_set_uniform_float3(material->shader, "u_emissive", 
                                   material->emissive_factor[0], 
                                   material->emissive_factor[1], 
                                   material->emissive_factor[2]);
    
    /* Set texture map presence flags */
    nexus_shader_set_uniform_int(material->shader, "u_hasAlbedoMap", 
                                material->albedo_map != NULL ? 1 : 0);
    nexus_shader_set_uniform_int(material->shader, "u_hasNormalMap", 
                                material->normal_map != NULL ? 1 : 0);
    nexus_shader_set_uniform_int(material->shader, "u_hasMetallicMap", 
                                material->metallic_map != NULL ? 1 : 0);
    nexus_shader_set_uniform_int(material->shader, "u_hasRoughnessMap", 
                                material->roughness_map != NULL ? 1 : 0);
    nexus_shader_set_uniform_int(material->shader, "u_hasAOMap", 
                                material->ao_map != NULL ? 1 : 0);
    nexus_shader_set_uniform_int(material->shader, "u_hasEmissiveMap", 
                                material->emissive_map != NULL ? 1 : 0);
    
    /* Bind textures if available */
    /* Note: This will be implemented once the texture system is complete */
    
    /* Apply blend mode */
    /* Note: This will be expanded based on SDL3 GPU API blend state setup */
    
    /* Apply other render states */
    /* Note: The shader's pipeline should have culling, etc. configured already */
}
