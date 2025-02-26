/**
 * Nexus3D Configuration Implementation
 * Handles engine configuration settings
 */

#include "nexus3d/core/config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * Create a configuration
 */
NexusConfig* nexus_config_create(void) {
    /* Allocate configuration structure */
    NexusConfig* config = (NexusConfig*)malloc(sizeof(NexusConfig));
    if (config == NULL) {
        printf("Failed to allocate memory for configuration!\n");
        return NULL;
    }

    /* Initialize with default values */
    nexus_config_set_defaults(config);
    
    return config;
}

/**
 * Destroy a configuration
 */
void nexus_config_destroy(NexusConfig* config) {
    if (config == NULL) {
        return;
    }
    
    /* Free configuration structure */
    free(config);
}

/**
 * Set default configuration values
 */
void nexus_config_set_defaults(NexusConfig* config) {
    if (config == NULL) {
        return;
    }
    
    /* Clear configuration structure */
    memset(config, 0, sizeof(NexusConfig));
    
    /* Window configuration */
    config->window.title = "Nexus3D";
    config->window.width = 1280;
    config->window.height = 720;
    config->window.fullscreen = false;
    config->window.resizable = true;
    config->window.vsync = true;
    config->window.display_index = 0;
    
    /* Graphics configuration */
    config->graphics.enable_shadows = true;
    config->graphics.enable_msaa = true;
    config->graphics.msaa_samples = 4;
    config->graphics.enable_vsync = true;
    config->graphics.max_fps = 0; /* Unlimited */
    config->graphics.enable_hdr = false;
    
    /* Audio configuration */
    config->audio.enable_audio = true;
    config->audio.max_channels = 32;
    config->audio.master_volume = 1.0f;
    
    /* Physics configuration */
    config->physics.enable_physics = true;
    config->physics.fixed_timestep = 1.0f / 60.0f; /* 60 Hz */
    config->physics.max_substeps = 5;
    config->physics.gravity = 9.81f;
    
    /* Input configuration */
    config->input.enable_gamepad = true;
    config->input.enable_keyboard = true;
    config->input.enable_mouse = true;
    
    /* Debug configuration */
    config->debug.enable_debug_logging = false;
    config->debug.enable_physics_debug = false;
    config->debug.enable_profiling = false;
}

/**
 * Load configuration from file
 * This is a basic implementation that can be extended with a proper file format
 */
bool nexus_config_load_from_file(NexusConfig* config, const char* filepath) {
    if (config == NULL || filepath == NULL) {
        return false;
    }
    
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        printf("Failed to open configuration file: %s\n", filepath);
        return false;
    }
    
    /* Set default values first */
    nexus_config_set_defaults(config);
    
    /* Buffer for reading lines */
    char line[256];
    char key[128];
    char value[128];
    
    /* Read configuration lines */
    while (fgets(line, sizeof(line), file)) {
        /* Skip comments and empty lines */
        if (line[0] == '#' || line[0] == '\n') {
            continue;
        }
        
        /* Parse key-value pairs */
        if (sscanf(line, "%127[^=]=%127s", key, value) == 2) {
            /* Trim whitespace */
            char* k = key;
            while (*k == ' ' || *k == '\t') k++;
            char* v = value;
            while (*v == ' ' || *v == '\t') v++;
            
            /* Process key-value pairs */
            if (strcmp(k, "window.width") == 0) {
                config->window.width = atoi(v);
            } else if (strcmp(k, "window.height") == 0) {
                config->window.height = atoi(v);
            } else if (strcmp(k, "window.fullscreen") == 0) {
                config->window.fullscreen = (strcmp(v, "true") == 0 || strcmp(v, "1") == 0);
            } else if (strcmp(k, "window.vsync") == 0) {
                config->window.vsync = (strcmp(v, "true") == 0 || strcmp(v, "1") == 0);
            } else if (strcmp(k, "graphics.enable_shadows") == 0) {
                config->graphics.enable_shadows = (strcmp(v, "true") == 0 || strcmp(v, "1") == 0);
            }
            /* Add more configuration options as needed */
        }
    }
    
    fclose(file);
    return true;
}

/**
 * Save configuration to file
 * This is a basic implementation that can be extended with a proper file format
 */
bool nexus_config_save_to_file(const NexusConfig* config, const char* filepath) {
    if (config == NULL || filepath == NULL) {
        return false;
    }
    
    FILE* file = fopen(filepath, "w");
    if (file == NULL) {
        printf("Failed to open configuration file for writing: %s\n", filepath);
        return false;
    }
    
    /* Write header */
    fprintf(file, "# Nexus3D Engine Configuration\n");
    fprintf(file, "# Generated automatically\n\n");
    
    /* Write window configuration */
    fprintf(file, "# Window Configuration\n");
    fprintf(file, "window.width=%d\n", config->window.width);
    fprintf(file, "window.height=%d\n", config->window.height);
    fprintf(file, "window.fullscreen=%s\n", config->window.fullscreen ? "true" : "false");
    fprintf(file, "window.resizable=%s\n", config->window.resizable ? "true" : "false");
    fprintf(file, "window.vsync=%s\n", config->window.vsync ? "true" : "false");
    fprintf(file, "window.display_index=%d\n\n", config->window.display_index);
    
    /* Write graphics configuration */
    fprintf(file, "# Graphics Configuration\n");
    fprintf(file, "graphics.enable_shadows=%s\n", config->graphics.enable_shadows ? "true" : "false");
    fprintf(file, "graphics.enable_msaa=%s\n", config->graphics.enable_msaa ? "true" : "false");
    fprintf(file, "graphics.msaa_samples=%d\n", config->graphics.msaa_samples);
    fprintf(file, "graphics.enable_vsync=%s\n", config->graphics.enable_vsync ? "true" : "false");
    fprintf(file, "graphics.max_fps=%d\n", config->graphics.max_fps);
    fprintf(file, "graphics.enable_hdr=%s\n\n", config->graphics.enable_hdr ? "true" : "false");
    
    /* Write audio configuration */
    fprintf(file, "# Audio Configuration\n");
    fprintf(file, "audio.enable_audio=%s\n", config->audio.enable_audio ? "true" : "false");
    fprintf(file, "audio.max_channels=%d\n", config->audio.max_channels);
    fprintf(file, "audio.master_volume=%f\n\n", config->audio.master_volume);
    
    /* Write physics configuration */
    fprintf(file, "# Physics Configuration\n");
    fprintf(file, "physics.enable_physics=%s\n", config->physics.enable_physics ? "true" : "false");
    fprintf(file, "physics.fixed_timestep=%f\n", config->physics.fixed_timestep);
    fprintf(file, "physics.max_substeps=%d\n", config->physics.max_substeps);
    fprintf(file, "physics.gravity=%f\n\n", config->physics.gravity);
    
    /* Write input configuration */
    fprintf(file, "# Input Configuration\n");
    fprintf(file, "input.enable_gamepad=%s\n", config->input.enable_gamepad ? "true" : "false");
    fprintf(file, "input.enable_keyboard=%s\n", config->input.enable_keyboard ? "true" : "false");
    fprintf(file, "input.enable_mouse=%s\n\n", config->input.enable_mouse ? "true" : "false");
    
    /* Write debug configuration */
    fprintf(file, "# Debug Configuration\n");
    fprintf(file, "debug.enable_debug_logging=%s\n", config->debug.enable_debug_logging ? "true" : "false");
    fprintf(file, "debug.enable_physics_debug=%s\n", config->debug.enable_physics_debug ? "true" : "false");
    fprintf(file, "debug.enable_profiling=%s\n", config->debug.enable_profiling ? "true" : "false");
    
    fclose(file);
    return true;
}
