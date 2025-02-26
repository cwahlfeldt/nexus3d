/**
 * Nexus3D Renderer Implementation
 * GPU-accelerated rendering system using SDL3 GPU API
 */

#include "nexus3d/renderer/renderer.h"
#include "nexus3d/utils/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Current SDL GPU driver message */
#ifdef NEXUS_DEBUG
#define NEXUS_LOG_GPU_DRIVER(renderer) \
    if (renderer && renderer->gpu_device) { \
        printf("Using GPU driver: %s\n", SDL_GetGPUDeviceDriver(renderer->gpu_device)); \
    }
#else
#define NEXUS_LOG_GPU_DRIVER(renderer)
#endif

/* Initialize GPU device */
static SDL_GPUDevice* nexus_renderer_init_gpu(NexusWindow* window,
                                           const NexusRendererConfig* config,
                                           NexusRendererCaps* caps) {
    /* Check for null parameters */
    if (window == NULL || config == NULL || caps == NULL) {
        fprintf(stderr, "Invalid parameters for renderer initialization!\n");
        return NULL;
    }

    /* Create GPU device with support for all shader formats */
    SDL_GPUDevice* device = SDL_CreateGPUDevice(
        SDL_GPU_SHADERFORMAT_SPIRV | SDL_GPU_SHADERFORMAT_DXBC |
        SDL_GPU_SHADERFORMAT_MSL,
        true, NULL
    );

    if (device == NULL) {
        fprintf(stderr, "Failed to create GPU device: %s\n", SDL_GetError());
        return NULL;
    }

    /* Get driver information */
    const char* driver = SDL_GetGPUDeviceDriver(device);
    printf("GPU driver: %s\n", driver);

    /* Get capabilities information and fill caps structure */
    SDL_PropertiesID deviceProps = SDL_GetGlobalProperties();

    /* Store driver information */
    strncpy(caps->gpu_vendor, "Unknown", sizeof(caps->gpu_vendor) - 1);
    strncpy(caps->gpu_renderer, driver, sizeof(caps->gpu_renderer) - 1);
    strncpy(caps->gpu_version, "1.0", sizeof(caps->gpu_version) - 1);

    /* Get supported formats and capabilities */
    caps->supports_msaa = true;
    caps->max_msaa_samples = 8;
    // We'll assume compute is supported for now
    caps->supports_compute = true;
    caps->supports_hdr = true;
    caps->max_texture_size = 4096;
    caps->max_texture_array_layers = 256;

    /* Claim the window for the GPU device */
    if (!SDL_ClaimWindowForGPUDevice(device, window->sdl_window)) {
        fprintf(stderr, "Failed to claim window for GPU device: %s\n", SDL_GetError());
        SDL_DestroyGPUDevice(device);
        return NULL;
    }

    /* Set swapchain parameters */
    SDL_GPUSwapchainComposition composition = config->composition_mode;
    SDL_GPUPresentMode present_mode = config->enable_vsync ?
        SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX;

    /* Check if the requested modes are supported */
    if (!SDL_WindowSupportsGPUSwapchainComposition(device, window->sdl_window, composition)) {
        fprintf(stderr, "Warning: Requested swapchain composition mode not supported, using default\n");
        composition = SDL_GPU_SWAPCHAINCOMPOSITION_SDR;
    }

    if (!SDL_WindowSupportsGPUPresentMode(device, window->sdl_window, present_mode)) {
        fprintf(stderr, "Warning: Requested present mode not supported, using FIFO\n");
        present_mode = SDL_GPU_PRESENTMODE_VSYNC;
    }

    /* Apply swapchain parameters */
    if (!SDL_SetGPUSwapchainParameters(device, window->sdl_window, composition, present_mode)) {
        fprintf(stderr, "Failed to set swapchain parameters: %s\n", SDL_GetError());
        SDL_ReleaseWindowFromGPUDevice(device, window->sdl_window);
        SDL_DestroyGPUDevice(device);
        return NULL;
    }

    /* Set maximum frames in flight to 2 (double buffering) */
    SDL_SetGPUAllowedFramesInFlight(device, 2);

    return device;
}

/**
 * Create a renderer
 */
NexusRenderer* nexus_renderer_create(NexusWindow* window, const NexusRendererConfig* config) {
    /* Check for null parameters */
    if (window == NULL) {
        fprintf(stderr, "Window cannot be NULL when creating renderer!\n");
        return NULL;
    }

    /* Use default config if NULL is provided */
    NexusRendererConfig default_config = {
        .enable_shadows = true,
        .enable_msaa = true,
        .msaa_samples = 4,
        .enable_vsync = true,
        .enable_hdr = false,
        .composition_mode = SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        .present_mode = SDL_GPU_PRESENTMODE_VSYNC
    };

    if (config == NULL) {
        config = &default_config;
    }

    /* Allocate renderer structure */
    NexusRenderer* renderer = (NexusRenderer*)malloc(sizeof(NexusRenderer));
    if (renderer == NULL) {
        fprintf(stderr, "Failed to allocate memory for renderer!\n");
        return NULL;
    }

    /* Initialize renderer structure */
    memset(renderer, 0, sizeof(NexusRenderer));

    /* Copy configuration */
    renderer->config = *config;

    /* Set default clear color (dark blue) */
    renderer->clear_color[0] = 0.1f;  /* R */
    renderer->clear_color[1] = 0.1f;  /* G */
    renderer->clear_color[2] = 0.2f;  /* B */
    renderer->clear_color[3] = 1.0f;  /* A */

    /* Reset stats */
    renderer->frame_time = 0.0;
    renderer->draw_calls = 0;
    renderer->triangle_count = 0;

    /* Store window reference */
    renderer->window = window->sdl_window;

    /* Initialize GPU device and check capabilities */
    renderer->gpu_device = nexus_renderer_init_gpu(window, config, &renderer->caps);
    if (renderer->gpu_device == NULL) {
        fprintf(stderr, "Failed to initialize GPU device!\n");
        free(renderer);
        return NULL;
    }

    /* Get swapchain format */
    renderer->swapchain_format = SDL_GetGPUSwapchainTextureFormat(
        renderer->gpu_device, window->sdl_window);

    /* Get window dimensions */
    int width, height;
    SDL_GetWindowSize(window->sdl_window, &width, &height);
    renderer->swapchain_width = (uint32_t)width;
    renderer->swapchain_height = (uint32_t)height;

    /* Create default shader (will be properly implemented in shader.c) */
    renderer->default_shader = NULL;

    /* Create main camera */
    renderer->main_camera = nexus_camera_create();
    if (renderer->main_camera == NULL) {
        fprintf(stderr, "Failed to create default camera!\n");
        SDL_ReleaseWindowFromGPUDevice(renderer->gpu_device, window->sdl_window);
        SDL_DestroyGPUDevice(renderer->gpu_device);
        free(renderer);
        return NULL;
    }

    /* Set up default camera */
    nexus_camera_set_perspective(renderer->main_camera, 45.0f,
                               (float)width / (float)height, 0.1f, 1000.0f);
    nexus_camera_set_position(renderer->main_camera, 0.0f, 0.0f, 5.0f);
    nexus_camera_look_at(renderer->main_camera, 0.0f, 0.0f, 0.0f);
    nexus_camera_update(renderer->main_camera);

    printf("Renderer created successfully!\n");
    NEXUS_LOG_GPU_DRIVER(renderer);

    return renderer;
}

/**
 * Destroy a renderer
 */
void nexus_renderer_destroy(NexusRenderer* renderer) {
    if (renderer == NULL) {
        return;
    }

    /* Destroy main camera */
    if (renderer->main_camera != NULL) {
        nexus_camera_destroy(renderer->main_camera);
        renderer->main_camera = NULL;
    }

    /* Destroy default shader */
    if (renderer->default_shader != NULL) {
        nexus_shader_destroy(renderer->default_shader);
        renderer->default_shader = NULL;
    }

    /* Release SDL window from GPU device */
    if (renderer->gpu_device != NULL && renderer->window != NULL) {
        /* Wait for all GPU operations to complete */
        SDL_WaitForGPUIdle(renderer->gpu_device);

        /* Release the window */
        SDL_ReleaseWindowFromGPUDevice(renderer->gpu_device, renderer->window);

        /* Destroy the GPU device */
        SDL_DestroyGPUDevice(renderer->gpu_device);
        renderer->gpu_device = NULL;
    }

    /* Free renderer structure */
    free(renderer);
}

/**
 * Begin rendering a frame
 */
bool nexus_renderer_begin_frame(NexusRenderer* renderer) {
    if (renderer == NULL || renderer->gpu_device == NULL || renderer->window == NULL) {
        return false;
    }

    /* Acquire a command buffer */
    renderer->cmd_buffer = SDL_AcquireGPUCommandBuffer(renderer->gpu_device);
    if (renderer->cmd_buffer == NULL) {
        fprintf(stderr, "Failed to acquire command buffer: %s\n", SDL_GetError());
        return false;
    }

    /* Wait for and acquire the swapchain texture */
    if (!SDL_WaitAndAcquireGPUSwapchainTexture(
            renderer->cmd_buffer, renderer->window,
            &renderer->swapchain_texture,
            &renderer->swapchain_width, &renderer->swapchain_height)) {
        fprintf(stderr, "Failed to acquire swapchain texture: %s\n", SDL_GetError());
        SDL_CancelGPUCommandBuffer(renderer->cmd_buffer);
        return false;
    }

    /* Reset frame statistics */
    renderer->draw_calls = 0;
    renderer->triangle_count = 0;

    /* Begin a render pass to clear the swapchain */
    SDL_GPUColorTargetInfo color_target = {
        .texture = renderer->swapchain_texture,
        .load_op = SDL_GPU_LOADOP_CLEAR,
        .store_op = SDL_GPU_STOREOP_STORE,
        .clear_color = {
            renderer->clear_color[0],
            renderer->clear_color[1],
            renderer->clear_color[2],
            renderer->clear_color[3]
        },
        .cycle = true
    };

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
        renderer->cmd_buffer, &color_target, 1, NULL);

    if (render_pass == NULL) {
        fprintf(stderr, "Failed to begin render pass: %s\n", SDL_GetError());
        SDL_CancelGPUCommandBuffer(renderer->cmd_buffer);
        return false;
    }

    /* Set up viewport to match the swapchain dimensions */
    SDL_GPUViewport viewport = {
        .x = 0,
        .y = 0,
        .w = (float)renderer->swapchain_width,
        .h = (float)renderer->swapchain_height,
        .min_depth = 0.0f,
        .max_depth = 1.0f
    };
    SDL_SetGPUViewport(render_pass, &viewport);

    /* End the initial clear render pass */
    SDL_EndGPURenderPass(render_pass);

    return true;
}

/**
 * End rendering a frame
 */
void nexus_renderer_end_frame(NexusRenderer* renderer) {
    if (renderer == NULL || renderer->cmd_buffer == NULL) {
        return;
    }

    /* Submit the command buffer */
    if (!SDL_SubmitGPUCommandBuffer(renderer->cmd_buffer)) {
        fprintf(stderr, "Failed to submit command buffer: %s\n", SDL_GetError());
    }

    /* Clear references */
    renderer->cmd_buffer = NULL;
    renderer->swapchain_texture = NULL;
}

/**
 * Render a mesh with a shader and transform
 */
void nexus_renderer_render_mesh(NexusRenderer* renderer,
                              NexusMesh* mesh,
                              NexusShader* shader,
                              const float* transform) {
    if (renderer == NULL || mesh == NULL ||
        renderer->cmd_buffer == NULL || renderer->window == NULL) {
        return;
    }

    /* Use default shader if none provided */
    if (shader == NULL) {
        shader = renderer->default_shader;
        if (shader == NULL) {
            fprintf(stderr, "No shader available for rendering!\n");
            return;
        }
    }

    /* Begin a render pass */
    SDL_GPUColorTargetInfo color_target = {
        .texture = renderer->swapchain_texture,
        .load_op = SDL_GPU_LOADOP_LOAD,  /* We're not clearing, just drawing */
        .store_op = SDL_GPU_STOREOP_STORE,
        .clear_color = {0.0f, 0.0f, 0.0f, 0.0f},
        .cycle = false
    };

    SDL_GPURenderPass* render_pass = SDL_BeginGPURenderPass(
        renderer->cmd_buffer, &color_target, 1, NULL);

    if (render_pass == NULL) {
        fprintf(stderr, "Failed to begin mesh render pass: %s\n", SDL_GetError());
        return;
    }

    /* Set up viewport to match the swapchain dimensions */
    SDL_GPUViewport viewport = {
        .x = 0,
        .y = 0,
        .w = (float)renderer->swapchain_width,
        .h = (float)renderer->swapchain_height,
        .min_depth = 0.0f,
        .max_depth = 1.0f
    };
    SDL_SetGPUViewport(render_pass, &viewport);

    /* Bind the shader's pipeline */
    nexus_shader_bind(shader, render_pass);

    /* Set camera matrices as uniforms */
    if (renderer->main_camera != NULL) {
        mat4 view, projection, view_projection;

        /* Update camera matrices */
        nexus_camera_update(renderer->main_camera);

        /* Get matrices */
        nexus_camera_get_view_matrix(renderer->main_camera, (float*)view);
        nexus_camera_get_projection_matrix(renderer->main_camera, (float*)projection);
        nexus_camera_get_view_projection_matrix(renderer->main_camera, (float*)view_projection);

        /* Set matrices as uniforms */
        nexus_shader_set_uniform_matrix4(shader, "u_view", (float*)view);
        nexus_shader_set_uniform_matrix4(shader, "u_projection", (float*)projection);
        nexus_shader_set_uniform_matrix4(shader, "u_viewProjection", (float*)view_projection);
    }

    /* Set transform matrix if provided */
    if (transform != NULL) {
        nexus_shader_set_uniform_matrix4(shader, "u_model", transform);
    }

    /* Draw the mesh */
    uint32_t triangles = nexus_mesh_draw(mesh, render_pass);

    /* Update statistics */
    renderer->draw_calls++;
    renderer->triangle_count += triangles;

    /* End the render pass */
    SDL_EndGPURenderPass(render_pass);
}

/**
 * Set the clear color for the renderer
 */
void nexus_renderer_set_clear_color(NexusRenderer* renderer, float r, float g, float b, float a) {
    if (renderer == NULL) {
        return;
    }

    /* Store the clear color for next frame */
    renderer->clear_color[0] = r;
    renderer->clear_color[1] = g;
    renderer->clear_color[2] = b;
    renderer->clear_color[3] = a;
}

/**
 * Set the active camera for the renderer
 */
void nexus_renderer_set_camera(NexusRenderer* renderer, NexusCamera* camera) {
    if (renderer == NULL) {
        return;
    }

    /* Store the camera */
    renderer->main_camera = camera;
}

/**
 * Get the active camera from the renderer
 */
NexusCamera* nexus_renderer_get_camera(const NexusRenderer* renderer) {
    if (renderer == NULL) {
        return NULL;
    }

    return renderer->main_camera;
}

/**
 * Resize the renderer to match a new window size
 */
void nexus_renderer_resize(NexusRenderer* renderer, int width, int height) {
    if (renderer == NULL || renderer->main_camera == NULL || width <= 0 || height <= 0) {
        return;
    }

    /* Update renderer dimensions */
    renderer->swapchain_width = (uint32_t)width;
    renderer->swapchain_height = (uint32_t)height;

    /* Update main camera aspect ratio */
    float aspect_ratio = (float)width / (float)height;
    nexus_camera_set_aspect_ratio(renderer->main_camera, aspect_ratio);

    printf("Renderer resized to %dx%d (aspect: %.2f)\n", width, height, aspect_ratio);
}

/**
 * Get the capabilities of the renderer
 */
NexusRendererCaps nexus_renderer_get_capabilities(NexusRenderer* renderer) {
    NexusRendererCaps empty_caps;
    memset(&empty_caps, 0, sizeof(NexusRendererCaps));

    if (renderer == NULL) {
        return empty_caps;
    }

    return renderer->caps;
}

/**
 * Get the GPU device from the renderer
 */
SDL_GPUDevice* nexus_renderer_get_gpu_device(const NexusRenderer* renderer) {
    if (renderer == NULL) {
        return NULL;
    }

    return renderer->gpu_device;
}

/**
 * Get the number of draw calls in the current frame
 */
uint32_t nexus_renderer_get_draw_call_count(const NexusRenderer* renderer) {
    if (renderer == NULL) {
        return 0;
    }

    return renderer->draw_calls;
}

/**
 * Get the number of triangles in the current frame
 */
uint32_t nexus_renderer_get_triangle_count(const NexusRenderer* renderer) {
    if (renderer == NULL) {
        return 0;
    }

    return renderer->triangle_count;
}

/**
 * Get the time taken to render the last frame (in milliseconds)
 */
double nexus_renderer_get_frame_time(const NexusRenderer* renderer) {
    if (renderer == NULL) {
        return 0.0;
    }

    return renderer->frame_time;
}

/**
 * Set the frame time value
 * This should be called by the engine after measuring the frame time
 */
void nexus_renderer_set_frame_time(NexusRenderer* renderer, double frame_time_ms) {
    if (renderer == NULL) {
        return;
    }

    renderer->frame_time = frame_time_ms;
}
