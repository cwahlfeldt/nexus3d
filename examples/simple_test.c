/**
 * Nexus3D Simple Test Example
 * Demonstrates basic engine initialization and shutdown
 */

#include "nexus3d/nexus3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <SDL3/SDL.h>  /* For SDL_Delay */

int main(int argc, char *argv[]) {
    /* Initialize the engine */
    printf("Attempting to initialize Nexus3D...\n");
    if (!nexus3d_init()) {
        fprintf(stderr, "Failed to initialize Nexus3D Engine!\n");
        return EXIT_FAILURE;
    }

    /* Print version information */
    printf("Nexus3D Engine v%s - Simple Test\n", nexus3d_get_version());

    /* Main game loop - just run for a few frames */
    printf("Running main loop...\n");
    int frames = 10;
    while (nexus_engine_is_running() && frames-- > 0) {
        /* Update engine */
        nexus_engine_update();

        /* Print frame count */
        printf("Frame: %lu\n", nexus_engine_get_frame_count());
        
        /* Add a small delay to avoid busy waiting */
        SDL_Delay(100); /* 100ms */
    }

    /* Shutdown engine */
    printf("Shutting down engine...\n");
    nexus3d_shutdown();
    printf("Engine shutdown complete\n");

    return EXIT_SUCCESS;
}
