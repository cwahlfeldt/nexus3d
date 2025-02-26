/**
 * Nexus3D Renderer
 * GPU-accelerated rendering system using SDL3 GPU API
 */

#ifndef NEXUS3D_RENDERER_H
#define NEXUS3D_RENDERER_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include "nexus3d/core/window.h"
#include "nexus3d/renderer/camera.h"
#include "nexus3d/renderer/mesh.h"
#include "nexus3d/renderer/shader.h"

/**
 * Renderer capabilities structure
 */
typedef struct {
    bool supports_msaa;            /* Multi-sample anti-aliasing support */
    int max_msaa_samples;          /* Maximum MSAA samples */
    bool supports_compute;         /* Compute shader support */
    bool supports_hdr;             /* High dynamic range support */
    int max_texture_size;          /* Maximum texture size */
    int max_texture_array_layers;  /* Maximum texture array layers */
    char gpu_vendor[64];           /* GPU vendor string */
    char gpu_renderer[64];         /* GPU renderer string */
    char gpu_version[64];          /* GPU version string */
} NexusRendererCaps;

/**
 * Renderer configuration
 */
typedef struct {
    bool enable_shadows;           /* Enable shadow rendering */
    bool enable_msaa;              /* Enable multi-sample anti-aliasing */
    int msaa_samples;              /* MSAA sample count (2, 4, 8, etc.) */
    bool enable_vsync;             /* Enable vertical sync */
    bool enable_hdr;               /* Enable high dynamic range */
    SDL_GPUSwapchainComposition composition_mode; /* Swapchain composition mode */
    SDL_GPUPresentMode present_mode; /* Present mode */
} NexusRendererConfig;

/**
 * Main renderer structure
 */
typedef struct NexusRenderer {
    NexusRendererConfig config;    /* Renderer configuration */
    NexusRendererCaps caps;        /* Renderer capabilities */
    SDL_GPUDevice* gpu_device;     /* SDL GPU device */
    SDL_Window* window;            /* Associated window */
    SDL_GPUTexture* swapchain_texture; /* Current swapchain texture */
    uint32_t swapchain_width;      /* Swapchain width */
    uint32_t swapchain_height;     /* Swapchain height */
    SDL_GPUTextureFormat swapchain_format; /* Swapchain texture format */
    
    /* Command pools and buffers */
    SDL_GPUCommandBuffer* cmd_buffer; /* Current command buffer */
    
    /* Resources */
    NexusShader* default_shader;   /* Default shader */
    NexusCamera* main_camera;      /* Main camera */
} NexusRenderer;

/* Renderer functions */
NexusRenderer* nexus_renderer_create(NexusWindow* window, const NexusRendererConfig* config);
void nexus_renderer_destroy(NexusRenderer* renderer);
bool nexus_renderer_begin_frame(NexusRenderer* renderer);
void nexus_renderer_end_frame(NexusRenderer* renderer);
void nexus_renderer_render_mesh(NexusRenderer* renderer, NexusMesh* mesh, NexusShader* shader, const float* transform);
void nexus_renderer_set_clear_color(NexusRenderer* renderer, float r, float g, float b, float a);
void nexus_renderer_set_camera(NexusRenderer* renderer, NexusCamera* camera);
NexusCamera* nexus_renderer_get_camera(const NexusRenderer* renderer);
void nexus_renderer_resize(NexusRenderer* renderer, int width, int height);
NexusRendererCaps nexus_renderer_get_capabilities(NexusRenderer* renderer);
SDL_GPUDevice* nexus_renderer_get_gpu_device(const NexusRenderer* renderer);

#endif /* NEXUS3D_RENDERER_H */
