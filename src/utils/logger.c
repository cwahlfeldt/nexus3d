/**
 * Nexus3D Logger Implementation
 * Handles logging and debugging output
 */

#include "nexus3d/utils/logger.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>

/* ANSI color codes for colored output */
#define NEXUS_COLOR_RESET   "\033[0m"
#define NEXUS_COLOR_TRACE   "\033[90m"  /* Bright Black (Gray) */
#define NEXUS_COLOR_DEBUG   "\033[36m"  /* Cyan */
#define NEXUS_COLOR_INFO    "\033[32m"  /* Green */
#define NEXUS_COLOR_WARNING "\033[33m"  /* Yellow */
#define NEXUS_COLOR_ERROR   "\033[31m"  /* Red */
#define NEXUS_COLOR_FATAL   "\033[35m"  /* Magenta */

/* Level strings */
static const char* level_strings[] = {
    "TRACE", "DEBUG", "INFO", "WARNING", "ERROR", "FATAL"
};

/* Category strings */
static const char* category_strings[] = {
    "GENERAL", "RENDER", "AUDIO", "PHYSICS", "INPUT", "ECS", 
    "RESOURCE", "FILE", "SCRIPT", "NETWORK", "CUSTOM"
};

/* Get time string */
static void get_time_string(char* buffer, size_t buffer_size) {
    time_t raw_time;
    struct tm* time_info;
    
    time(&raw_time);
    time_info = localtime(&raw_time);
    
    strftime(buffer, buffer_size, "%Y-%m-%d %H:%M:%S", time_info);
}

/**
 * Create a logger
 */
NexusLogger* nexus_logger_create(void) {
    /* Allocate logger structure */
    NexusLogger* logger = (NexusLogger*)malloc(sizeof(NexusLogger));
    if (logger == NULL) {
        fprintf(stderr, "Failed to allocate memory for logger!\n");
        return NULL;
    }

    /* Initialize logger structure */
    memset(logger, 0, sizeof(NexusLogger));
    
    /* Set default values */
    logger->min_level = NEXUS_LOG_LEVEL_INFO;
    logger->console_output = true;
    logger->file_output = false;
    logger->color_output = true;
    logger->callback = NULL;
    logger->callback_user_data = NULL;
    
    /* Enable all categories by default */
    for (int i = 0; i < 16; i++) {
        logger->enabled_categories[i] = true;
    }
    
    return logger;
}

/**
 * Destroy a logger
 */
void nexus_logger_destroy(NexusLogger* logger) {
    if (logger == NULL) {
        return;
    }
    
    /* Close log file if open */
    if (logger->file_output && logger->file_handle != NULL) {
        fclose((FILE*)logger->file_handle);
        logger->file_handle = NULL;
    }
    
    /* Free logger structure */
    free(logger);
}

/**
 * Set minimum log level
 */
void nexus_logger_set_level(NexusLogger* logger, NexusLogLevel level) {
    if (logger == NULL) {
        return;
    }
    
    logger->min_level = level;
}

/**
 * Get minimum log level
 */
NexusLogLevel nexus_logger_get_level(const NexusLogger* logger) {
    if (logger == NULL) {
        return NEXUS_LOG_LEVEL_INFO;
    }
    
    return logger->min_level;
}

/**
 * Enable a log category
 */
void nexus_logger_enable_category(NexusLogger* logger, NexusLogCategory category) {
    if (logger == NULL || category < 0 || category >= 16) {
        return;
    }
    
    logger->enabled_categories[category] = true;
}

/**
 * Disable a log category
 */
void nexus_logger_disable_category(NexusLogger* logger, NexusLogCategory category) {
    if (logger == NULL || category < 0 || category >= 16) {
        return;
    }
    
    logger->enabled_categories[category] = false;
}

/**
 * Check if a log category is enabled
 */
bool nexus_logger_is_category_enabled(const NexusLogger* logger, NexusLogCategory category) {
    if (logger == NULL || category < 0 || category >= 16) {
        return false;
    }
    
    return logger->enabled_categories[category];
}

/**
 * Enable console output
 */
void nexus_logger_enable_console_output(NexusLogger* logger, bool enable) {
    if (logger == NULL) {
        return;
    }
    
    logger->console_output = enable;
}

/**
 * Enable file output
 */
void nexus_logger_enable_file_output(NexusLogger* logger, bool enable, const char* file_path) {
    if (logger == NULL) {
        return;
    }
    
    /* Close previous file if open */
    if (logger->file_output && logger->file_handle != NULL) {
        fclose((FILE*)logger->file_handle);
        logger->file_handle = NULL;
    }
    
    logger->file_output = enable;
    
    if (enable && file_path != NULL) {
        /* Copy file path */
        strncpy(logger->log_file_path, file_path, sizeof(logger->log_file_path) - 1);
        logger->log_file_path[sizeof(logger->log_file_path) - 1] = '\0';
        
        /* Open log file */
        logger->file_handle = fopen(file_path, "a");
        if (logger->file_handle == NULL) {
            fprintf(stderr, "Failed to open log file: %s\n", file_path);
            logger->file_output = false;
        }
    }
}

/**
 * Enable color output
 */
void nexus_logger_enable_color_output(NexusLogger* logger, bool enable) {
    if (logger == NULL) {
        return;
    }
    
    logger->color_output = enable;
}

/**
 * Set logger callback
 */
void nexus_logger_set_callback(NexusLogger* logger, NexusLoggerCallback callback, void* user_data) {
    if (logger == NULL) {
        return;
    }
    
    logger->callback = callback;
    logger->callback_user_data = user_data;
}

/**
 * Log a message
 */
void nexus_log(NexusLogger* logger, NexusLogLevel level, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (level < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Get time string */
    char time_str[64];
    get_time_string(time_str, sizeof(time_str));
    
    /* Call callback if set */
    if (logger->callback != NULL) {
        logger->callback(level, category, message, logger->callback_user_data);
    }
    
    /* Color codes for different log levels */
    const char* color = NEXUS_COLOR_RESET;
    if (logger->color_output) {
        switch (level) {
            case NEXUS_LOG_LEVEL_TRACE:   color = NEXUS_COLOR_TRACE;   break;
            case NEXUS_LOG_LEVEL_DEBUG:   color = NEXUS_COLOR_DEBUG;   break;
            case NEXUS_LOG_LEVEL_INFO:    color = NEXUS_COLOR_INFO;    break;
            case NEXUS_LOG_LEVEL_WARNING: color = NEXUS_COLOR_WARNING; break;
            case NEXUS_LOG_LEVEL_ERROR:   color = NEXUS_COLOR_ERROR;   break;
            case NEXUS_LOG_LEVEL_FATAL:   color = NEXUS_COLOR_FATAL;   break;
        }
    }
    
    /* Write to console */
    if (logger->console_output) {
        FILE* output = (level >= NEXUS_LOG_LEVEL_ERROR) ? stderr : stdout;
        if (logger->color_output) {
            fprintf(output, "%s[%s] [%s] [%s]%s %s\n",
                   color, time_str, level_strings[level], category_strings[category],
                   NEXUS_COLOR_RESET, message);
        } else {
            fprintf(output, "[%s] [%s] [%s] %s\n",
                   time_str, level_strings[level], category_strings[category],
                   message);
        }
    }
    
    /* Write to file */
    if (logger->file_output && logger->file_handle != NULL) {
        fprintf((FILE*)logger->file_handle, "[%s] [%s] [%s] %s\n",
               time_str, level_strings[level], category_strings[category],
               message);
        fflush((FILE*)logger->file_handle);
    }
}

/**
 * Log a trace message
 */
void nexus_log_trace(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (NEXUS_LOG_LEVEL_TRACE < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Log the message */
    nexus_log(logger, NEXUS_LOG_LEVEL_TRACE, category, "%s", message);
}

/**
 * Log a debug message
 */
void nexus_log_debug(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (NEXUS_LOG_LEVEL_DEBUG < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Log the message */
    nexus_log(logger, NEXUS_LOG_LEVEL_DEBUG, category, "%s", message);
}

/**
 * Log an info message
 */
void nexus_log_info(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (NEXUS_LOG_LEVEL_INFO < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Log the message */
    nexus_log(logger, NEXUS_LOG_LEVEL_INFO, category, "%s", message);
}

/**
 * Log a warning message
 */
void nexus_log_warning(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (NEXUS_LOG_LEVEL_WARNING < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Log the message */
    nexus_log(logger, NEXUS_LOG_LEVEL_WARNING, category, "%s", message);
}

/**
 * Log an error message
 */
void nexus_log_error(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (NEXUS_LOG_LEVEL_ERROR < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Log the message */
    nexus_log(logger, NEXUS_LOG_LEVEL_ERROR, category, "%s", message);
}

/**
 * Log a fatal message
 */
void nexus_log_fatal(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...) {
    if (logger == NULL || fmt == NULL) {
        return;
    }
    
    /* Check level and category */
    if (NEXUS_LOG_LEVEL_FATAL < logger->min_level || !logger->enabled_categories[category]) {
        return;
    }
    
    /* Format message */
    char message[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(message, sizeof(message), fmt, args);
    va_end(args);
    
    /* Log the message */
    nexus_log(logger, NEXUS_LOG_LEVEL_FATAL, category, "%s", message);
}
