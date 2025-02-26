/**
 * Nexus3D Engine Implementation
 * Central engine system implementation
 */

#include "nexus3d/core/config.h"
#include "nexus3d/nexus3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <SDL3/SDL_init.h>
#include <unistd.h> /* For putenv */

/* Global engine instance */
NexusEngine* g_engine = NULL;

/**
 * Helper function to convert from NexusGraphicsConfig to NexusRendererConfig
 */
static NexusRendererConfig convert_graphics_to_renderer_config(const NexusGraphicsConfig* graphics) {
    NexusRendererConfig renderer_config = {
        .enable_shadows = graphics->enable_shadows,
        .enable_msaa = graphics->enable_msaa,
        .msaa_samples = graphics->msaa_samples,
        .enable_vsync = graphics->enable_vsync,
        .enable_hdr = graphics->enable_hdr,
        .composition_mode = SDL_GPU_SWAPCHAINCOMPOSITION_SDR,
        .present_mode = graphics->enable_vsync ? SDL_GPU_PRESENTMODE_VSYNC : SDL_GPU_PRESENTMODE_MAILBOX
    };

    return renderer_config;
}

/**
 * Initialize the engine
 */
 bool nexus_engine_init(void) {
     /* Prevent double initialization */
     if (g_engine != NULL) {
         printf("Engine already initialized!\n");
         return false;
     }

     /* Allocate engine structure */
     g_engine = (NexusEngine*)malloc(sizeof(NexusEngine));
     if (g_engine == NULL) {
         printf("Failed to allocate memory for engine!\n");
         return false;
     }

     /* Initialize engine structure */
     memset(g_engine, 0, sizeof(NexusEngine));
     g_engine->running = false;
     g_engine->time_scale = 1.0;

     /* Create configuration */
     g_engine->config = nexus_config_create();
     if (g_engine->config == NULL) {
         printf("Failed to create engine configuration!\n");
         free(g_engine);
         g_engine = NULL;
         return false;
     }

     /* Set default configuration */
     nexus_config_set_defaults(g_engine->config);

     /* Set a dummy display variable - helps with headless environments */
     /* Avoid using environment variables */

     /* Initialize SDL */
     /* First try with all subsystems */
     if (!SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO | SDL_INIT_GAMEPAD | SDL_INIT_HAPTIC) != 0) {
         printf("Warning: Full SDL initialization failed: %s\n", SDL_GetError());
         printf("Trying with minimal subsystems...\n");

         /* Try again with just VIDEO */
         if (!SDL_Init(SDL_INIT_VIDEO) != 0) {
             printf("Failed to initialize SDL minimal mode: %s\n", SDL_GetError());
             nexus_config_destroy(g_engine->config);
             free(g_engine);
             g_engine = NULL;
             return false;
         }

         printf("SDL initialized in minimal mode\n");
     }

     /* Create main window */
     g_engine->window = nexus_window_create(&g_engine->config);
     if (g_engine->window == NULL) {
         /* Attempt to continue without a window in headless environments */
         printf("Warning: Failed to create window. Running in headless mode.\n");
         /*
          * At this point we could create a dummy window struct if needed
          * For now, we'll try to proceed without a window
          */
     }

     /* Initialize ECS */
     g_engine->world = ecs_init();
     if (g_engine->world == NULL) {
         printf("Failed to initialize ECS!\n");
         nexus_window_destroy(g_engine->window);
         SDL_Quit();
         nexus_config_destroy(g_engine->config);
         free(g_engine);
         g_engine = NULL;
         return false;
     }

     /* Initialize renderer - only if we have a window */
     if (g_engine->window != NULL) {
         NexusRendererConfig renderer_config = convert_graphics_to_renderer_config(&g_engine->config);
         g_engine->renderer = nexus_renderer_create(g_engine->window, &renderer_config);
         if (g_engine->renderer == NULL) {
             printf("Warning: Failed to create renderer. Visual output will be disabled.\n");
             /* Continue execution without a renderer */
         }
     } else {
         printf("Running without a renderer (headless mode).\n");
         g_engine->renderer = NULL;
     }

     /* Initialize input system */
     g_engine->input = nexus_input_create();
     if (g_engine->input == NULL) {
         printf("Failed to create input system!\n");
         nexus_renderer_destroy(g_engine->renderer);
         ecs_fini(g_engine->world);
         nexus_window_destroy(g_engine->window);
         SDL_Quit();
         nexus_config_destroy(g_engine->config);
         free(g_engine);
         g_engine = NULL;
         return false;
     }

     /* Initialize physics system */
     g_engine->physics = nexus_physics_create(g_engine->world);
     if (g_engine->physics == NULL) {
         printf("Failed to create physics system!\n");
         nexus_input_destroy(g_engine->input);
         nexus_renderer_destroy(g_engine->renderer);
         ecs_fini(g_engine->world);
         nexus_window_destroy(g_engine->window);
         SDL_Quit();
         nexus_config_destroy(g_engine->config);
         free(g_engine);
         g_engine = NULL;
         return false;
     }

     /* Initialize audio system */
     g_engine->audio = nexus_audio_create(&g_engine->config);
     if (g_engine->audio == NULL) {
         printf("Failed to create audio system!\n");
         nexus_physics_destroy(g_engine->physics);
         nexus_input_destroy(g_engine->input);
         nexus_renderer_destroy(g_engine->renderer);
         ecs_fini(g_engine->world);
         nexus_window_destroy(g_engine->window);
         SDL_Quit();
         nexus_config_destroy(g_engine->config);
         free(g_engine);
         g_engine = NULL;
         return false;
     }

     /* Register ECS components */
     nexus_ecs_register_components(g_engine->world);

     /* Register ECS systems */
     if (!nexus_ecs_register_systems(g_engine->world)) {
         printf("Warning: Failed to register ECS systems. Some functionality may be limited.\n");
         /* Continue execution without systems */
     } else {
         printf("Successfully registered all ECS components and systems\n");
     }

     /* Engine is now running */
     g_engine->running = true;

     printf("Nexus3D Engine initialized successfully!\n");

     return true;
 }

/**
 * Shutdown the engine
 */
void nexus_engine_shutdown(void) {
    /* Check if engine is initialized */
    if (g_engine == NULL) {
        printf("Engine not initialized!\n");
        return;
    }

    /* Stop engine */
    g_engine->running = false;

    /* Destroy audio system */
    if (g_engine->audio != NULL) {
        nexus_audio_destroy(g_engine->audio);
        g_engine->audio = NULL;
    }

    /* Destroy physics system */
    if (g_engine->physics != NULL) {
        nexus_physics_destroy(g_engine->physics);
        g_engine->physics = NULL;
    }

    /* Destroy input system */
    if (g_engine->input != NULL) {
        nexus_input_destroy(g_engine->input);
        g_engine->input = NULL;
    }

    /* Destroy renderer */
    if (g_engine->renderer != NULL) {
        nexus_renderer_destroy(g_engine->renderer);
        g_engine->renderer = NULL;
    }

    /* Destroy ECS */
    if (g_engine->world != NULL) {
        ecs_fini(g_engine->world);
        g_engine->world = NULL;
    }

    /* Destroy window */
    if (g_engine->window != NULL) {
        nexus_window_destroy(g_engine->window);
        g_engine->window = NULL;
    }

    /* Destroy configuration */
    if (g_engine->config != NULL) {
        nexus_config_destroy(g_engine->config);
        g_engine->config = NULL;
    }

    /* Shutdown SDL */
    SDL_Quit();

    /* Free engine structure */
    free(g_engine);
    g_engine = NULL;

    printf("Nexus3D Engine shutdown successfully!\n");
}

/**
 * Update the engine
 */
void nexus_engine_update(void) {
    /* Check if engine is initialized and running */
    if (g_engine == NULL || !g_engine->running) {
        return;
    }

    /* Start frame timing */
    uint64_t frame_start_time = SDL_GetTicks();

    /* Process window events - only if we have a window */
    if (g_engine->window != NULL) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            /* Process input events */
            nexus_input_process_event(g_engine->input, &event);

            /* Handle quit event */
            if (event.type == SDL_EVENT_QUIT) {
                g_engine->running = false;
                break;
            }

            /* Handle window events */
            if (event.type == SDL_EVENT_WINDOW_RESIZED && g_engine->renderer != NULL) {
                /* Resize renderer */
                nexus_renderer_resize(g_engine->renderer, event.window.data1, event.window.data2);
            }
        }

        /* Check if window should be closed */
        if (nexus_window_should_close(g_engine->window)) {
            g_engine->running = false;
            return;
        }
    }



    /* Update input system */
    nexus_input_update(g_engine->input);
    /* Update ECS world - this processes all registered systems */
    ecs_progress(g_engine->world, g_engine->delta_time * g_engine->time_scale);
    // printf("FUCK\n");

    /*
     * Note: Most systems are now handled by the ECS, but we still need to manually update
     * these subsystems for functionality that isn't yet integrated into the ECS architecture
     */
    nexus_physics_update(g_engine->physics, g_engine->delta_time * g_engine->time_scale);
    nexus_audio_update(g_engine->audio, g_engine->delta_time * g_engine->time_scale);

    /* Render frame - only if we have a renderer */
    // printf("rendering running...\n");
    if (g_engine->renderer != NULL) {
        if (nexus_renderer_begin_frame(g_engine->renderer)) {
            /* Rendering happens here, but is mostly handled by ECS systems */

            nexus_renderer_end_frame(g_engine->renderer);
        }
    }

    /* Update window - only if we have a window */
    if (g_engine->window != NULL) {
        nexus_window_update(g_engine->window);
    }

    /* Update frame counter */
    g_engine->frame_count++;

    /* Calculate frame time */
    uint64_t frame_end_time = SDL_GetTicks();
    double frame_time_ms = (double)(frame_end_time - frame_start_time);

    /* Update delta time for next frame (convert to seconds) */
    g_engine->delta_time = frame_time_ms / 1000.0;

    /* Update renderer statistics */
    if (g_engine->renderer != NULL) {
        nexus_renderer_set_frame_time(g_engine->renderer, frame_time_ms);
    }

    /* Calculate and update performance metrics */
    g_engine->avg_frame_time = (g_engine->avg_frame_time * 0.95) + (frame_time_ms * 0.05);
    g_engine->fps = 1000.0 / g_engine->avg_frame_time;
}

/**
 * Check if the engine is running
 */
bool nexus_engine_is_running(void) {
    if (g_engine == NULL) {
        return false;
    }
    return g_engine->running;
}

/**
 * Request engine exit
 */
void nexus_engine_request_exit(void) {
    if (g_engine != NULL) {
        g_engine->running = false;
    }
}

/**
 * Get the delta time
 */
double nexus_engine_get_delta_time(void) {
    if (g_engine == NULL) {
        return 0.0;
    }
    return g_engine->delta_time;
}

/**
 * Set the time scale
 */
void nexus_engine_set_time_scale(double scale) {
    if (g_engine == NULL) {
        return;
    }
    g_engine->time_scale = scale;
}

/**
 * Get the time scale
 */
double nexus_engine_get_time_scale(void) {
    if (g_engine == NULL) {
        return 1.0;
    }
    return g_engine->time_scale;
}

/**
 * Get the frame count
 */
uint64_t nexus_engine_get_frame_count(void) {
    if (g_engine == NULL) {
        return 0;
    }
    return g_engine->frame_count;
}

/**
 * Get the ECS world
 */
ecs_world_t* nexus_engine_get_world(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->world;
}

/**
 * Get the renderer
 */
struct NexusRenderer* nexus_engine_get_renderer(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->renderer;
}

/**
 * Get the window
 */
struct NexusWindow* nexus_engine_get_window(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->window;
}

/**
 * Get the input system
 */
struct NexusInput* nexus_engine_get_input(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->input;
}

/**
 * Get the physics system
 */
struct NexusPhysics* nexus_engine_get_physics(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->physics;
}

/**
 * Get the audio system
 */
struct NexusAudio* nexus_engine_get_audio(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->audio;
}

/**
 * Get the configuration
 */
struct NexusConfig* nexus_engine_get_config(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->config;
}

/**
 * Get the current frames per second
 */
double nexus_engine_get_fps(void) {
    if (g_engine == NULL) {
        return 0.0;
    }
    return g_engine->fps;
}

/**
 * Get the average frame time in milliseconds
 */
double nexus_engine_get_avg_frame_time(void) {
    if (g_engine == NULL) {
        return 0.0;
    }
    return g_engine->avg_frame_time;
}
