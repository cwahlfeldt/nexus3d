/**
 * Nexus3D Engine Main Implementation
 * Entry point implementation for the engine
 */

#include "nexus3d/nexus3d.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Initialize the engine
 */
bool nexus3d_init(void) {
    return nexus_engine_init();
}

/**
 * Shutdown the engine
 */
void nexus3d_shutdown(void) {
    nexus_engine_shutdown();
}

/**
 * Get the engine version
 */
const char* nexus3d_get_version(void) {
    static char version[32];
    sprintf(version, "%d.%d.%d", 
        NEXUS3D_VERSION_MAJOR, 
        NEXUS3D_VERSION_MINOR, 
        NEXUS3D_VERSION_PATCH);
    return version;
}

/**
 * Main game loop example
 * This is not part of the API but can be used as a reference
 */
int nexus3d_run_example(void) {
    /* Initialize engine */
    if (!nexus3d_init()) {
        fprintf(stderr, "Failed to initialize Nexus3D Engine!\n");
        return EXIT_FAILURE;
    }
    
    /* Print version information */
    printf("Nexus3D Engine v%s\n", nexus3d_get_version());
    
    /* Main game loop */
    while (nexus_engine_is_running()) {
        /* Update engine */
        nexus_engine_update();
        
        /* You can add additional logic here if needed */
    }
    
    /* Shutdown engine */
    nexus3d_shutdown();
    
    return EXIT_SUCCESS;
}
