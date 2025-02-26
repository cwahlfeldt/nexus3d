/**
 * Nexus3D Texture System
 * Handles texture loading, creation, and management
 */

#ifndef NEXUS3D_TEXTURE_H
#define NEXUS3D_TEXTURE_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/**
 * Texture type enumeration
 */
typedef enum {
    NEXUS_TEXTURE_TYPE_2D,          /* 2D texture */
    NEXUS_TEXTURE_TYPE_CUBE,        /* Cube map texture */
    NEXUS_TEXTURE_TYPE_3D,          /* 3D volume texture */
    NEXUS_TEXTURE_TYPE_2D_ARRAY     /* 2D texture array */
} NexusTextureType;

/**
 * Texture filter mode enumeration
 */
typedef enum {
    NEXUS_TEXTURE_FILTER_NEAREST,   /* Nearest neighbor filtering */
    NEXUS_TEXTURE_FILTER_LINEAR,    /* Linear filtering */
    NEXUS_TEXTURE_FILTER_BILINEAR,  /* Bilinear filtering */
    NEXUS_TEXTURE_FILTER_TRILINEAR  /* Trilinear filtering (with mipmaps) */
} NexusTextureFilter;

/**
 * Texture wrap mode enumeration
 */
typedef enum {
    NEXUS_TEXTURE_WRAP_REPEAT,      /* Repeat texture */
    NEXUS_TEXTURE_WRAP_CLAMP,       /* Clamp to edge */
    NEXUS_TEXTURE_WRAP_MIRROR       /* Mirror repeat */
} NexusTextureWrap;

/**
 * Texture format enumeration
 */
typedef enum {
    NEXUS_TEXTURE_FORMAT_R8,        /* Single channel, 8-bit */
    NEXUS_TEXTURE_FORMAT_R8G8,      /* Two channels, 8-bit each */
    NEXUS_TEXTURE_FORMAT_R8G8B8,    /* Three channels, 8-bit each */
    NEXUS_TEXTURE_FORMAT_R8G8B8A8,  /* Four channels, 8-bit each */
    NEXUS_TEXTURE_FORMAT_R16F,      /* Single channel, 16-bit float */
    NEXUS_TEXTURE_FORMAT_R16G16F,   /* Two channels, 16-bit float each */
    NEXUS_TEXTURE_FORMAT_R16G16B16F, /* Three channels, 16-bit float each */
    NEXUS_TEXTURE_FORMAT_R16G16B16A16F, /* Four channels, 16-bit float each */
    NEXUS_TEXTURE_FORMAT_R32F,      /* Single channel, 32-bit float */
    NEXUS_TEXTURE_FORMAT_R32G32F,   /* Two channels, 32-bit float each */
    NEXUS_TEXTURE_FORMAT_R32G32B32F, /* Three channels, 32-bit float each */
    NEXUS_TEXTURE_FORMAT_R32G32B32A32F, /* Four channels, 32-bit float each */
    NEXUS_TEXTURE_FORMAT_DEPTH16,   /* Depth, 16-bit */
    NEXUS_TEXTURE_FORMAT_DEPTH24,   /* Depth, 24-bit */
    NEXUS_TEXTURE_FORMAT_DEPTH32F,  /* Depth, 32-bit float */
    NEXUS_TEXTURE_FORMAT_DEPTH24_STENCIL8 /* Depth/stencil, 24-bit depth, 8-bit stencil */
} NexusTextureFormat;

/**
 * Texture structure
 */
typedef struct NexusTexture {
    SDL_GPUTexture* gpu_texture;    /* GPU texture handle */
    SDL_GPUSampler* sampler;        /* Texture sampler */
    SDL_GPUDevice* device;          /* GPU device reference */
    
    /* Properties */
    NexusTextureType type;          /* Texture type */
    NexusTextureFormat format;      /* Texture format */
    uint32_t width;                 /* Texture width */
    uint32_t height;                /* Texture height */
    uint32_t depth;                 /* Texture depth (for 3D textures) */
    uint32_t mip_levels;            /* Number of mipmap levels */
    
    /* Sampler properties */
    NexusTextureFilter min_filter;  /* Minification filter */
    NexusTextureFilter mag_filter;  /* Magnification filter */
    NexusTextureWrap wrap_s;        /* Wrap mode for S coordinate */
    NexusTextureWrap wrap_t;        /* Wrap mode for T coordinate */
    NexusTextureWrap wrap_r;        /* Wrap mode for R coordinate */
    
    char name[64];                  /* Texture name */
} NexusTexture;

/* Texture functions */
NexusTexture* nexus_texture_create(SDL_GPUDevice* device, const char* name, NexusTextureType type, 
                                  uint32_t width, uint32_t height, uint32_t depth, 
                                  NexusTextureFormat format, uint32_t mip_levels);
void nexus_texture_destroy(NexusTexture* texture);
bool nexus_texture_set_data(NexusTexture* texture, const void* data, size_t size);
bool nexus_texture_generate_mipmaps(NexusTexture* texture);
void nexus_texture_set_filter(NexusTexture* texture, NexusTextureFilter min_filter, NexusTextureFilter mag_filter);
void nexus_texture_set_wrap(NexusTexture* texture, NexusTextureWrap wrap_s, NexusTextureWrap wrap_t, NexusTextureWrap wrap_r);
void nexus_texture_bind(NexusTexture* texture, SDL_GPURenderPass* render_pass, uint32_t binding);

/* Texture loading functions */
NexusTexture* nexus_texture_load_from_file(SDL_GPUDevice* device, const char* filename, bool generate_mipmaps);
NexusTexture* nexus_texture_load_cubemap_from_files(SDL_GPUDevice* device, const char* filenames[6], bool generate_mipmaps);

/* Utility texture creation functions */
NexusTexture* nexus_texture_create_solid_color(SDL_GPUDevice* device, float r, float g, float b, float a);
NexusTexture* nexus_texture_create_checkerboard(SDL_GPUDevice* device, uint32_t width, uint32_t height, 
                                               uint32_t check_size, float r1, float g1, float b1, 
                                               float r2, float g2, float b2);
NexusTexture* nexus_texture_create_render_target(SDL_GPUDevice* device, uint32_t width, uint32_t height, 
                                                NexusTextureFormat format);

#endif /* NEXUS3D_TEXTURE_H */
