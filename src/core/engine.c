/**
 * Nexus3D Engine Implementation
 * Central engine system implementation
 */

#include "nexus3d/nexus3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Global engine instance */
NexusEngine* g_engine = NULL;

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

    /* Initialize SDL */
    if (!SDL_Init(SDL_INIT_EVERYTHING)) {
        printf("Failed to initialize SDL: %s\n", SDL_GetError());
        nexus_config_destroy(g_engine->config);
        free(g_engine);
        g_engine = NULL;
        return false;
    }

    /* Create main window */
    g_engine->window = nexus_window_create(&g_engine->config->window);
    if (g_engine->window == NULL) {
        printf("Failed to create window!\n");
        SDL_Quit();
        nexus_config_destroy(g_engine->config);
        free(g_engine);
        g_engine = NULL;
        return false;
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

    /* Initialize renderer */
    g_engine->renderer = nexus_renderer_create(g_engine->window, &g_engine->config->graphics);
    if (g_engine->renderer == NULL) {
        printf("Failed to create renderer!\n");
        ecs_fini(g_engine->world);
        nexus_window_destroy(g_engine->window);
        SDL_Quit();
        nexus_config_destroy(g_engine->config);
        free(g_engine);
        g_engine = NULL;
        return false;
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
    g_engine->audio = nexus_audio_create(&g_engine->config->audio);
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
    nexus_ecs_register_systems(g_engine->world);

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

    /* Process window events */
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
        if (event.type == SDL_EVENT_WINDOW_RESIZED) {
            /* Resize renderer */
            nexus_renderer_resize(g_engine->renderer, event.window.data1, event.window.data2);
        }
    }

    /* Check if window should be closed */
    if (nexus_window_should_close(g_engine->window)) {
        g_engine->running = false;
        return;
    }

    /* Update input system */
    nexus_input_update(g_engine->input);

    /* Update ECS */
    ecs_progress(g_engine->world, g_engine->delta_time * g_engine->time_scale);

    /* Update physics system */
    nexus_physics_update(g_engine->physics, g_engine->delta_time * g_engine->time_scale);

    /* Update audio system */
    nexus_audio_update(g_engine->audio);

    /* Render frame */
    if (nexus_renderer_begin_frame(g_engine->renderer)) {
        /* Rendering happens here, but is mostly handled by ECS systems */
        nexus_renderer_end_frame(g_engine->renderer);
    }

    /* Update window */
    nexus_window_update(g_engine->window);

    /* Update frame counter */
    g_engine->frame_count++;
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
NexusRenderer* nexus_engine_get_renderer(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->renderer;
}

/**
 * Get the window
 */
NexusWindow* nexus_engine_get_window(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->window;
}

/**
 * Get the input system
 */
NexusInput* nexus_engine_get_input(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->input;
}

/**
 * Get the physics system
 */
NexusPhysics* nexus_engine_get_physics(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->physics;
}

/**
 * Get the audio system
 */
NexusAudio* nexus_engine_get_audio(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->audio;
}

/**
 * Get the configuration
 */
NexusConfig* nexus_engine_get_config(void) {
    if (g_engine == NULL) {
        return NULL;
    }
    return g_engine->config;
}
