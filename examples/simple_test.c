/**
 * Nexus3D Simple Test Example
 * Demonstrates basic engine initialization and shutdown
 */

#include "nexus3d/nexus3d.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
    /* Initialize the engine */
    if (!nexus3d_init()) {
        fprintf(stderr, "Failed to initialize Nexus3D Engine!\n");
        return EXIT_FAILURE;
    }

    /* Print version information */
    printf("Nexus3D Engine v%s - Simple Test\n", nexus3d_get_version());

    /* Main game loop - just run for a few frames */
    int frames = 10;
    while (nexus_engine_is_running() && frames-- > 0) {
        /* Update engine */
        nexus_engine_update();

        /* Print frame count */
        // printf("Frame: %lu\n", nexus_engine_get_frame_count());
    }

    /* Shutdown engine */
    nexus3d_shutdown();

    return EXIT_SUCCESS;
}
