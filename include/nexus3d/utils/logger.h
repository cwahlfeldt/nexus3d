/**
 * Nexus3D Logger System
 * Handles logging and debugging output
 */

#ifndef NEXUS3D_LOGGER_H
#define NEXUS3D_LOGGER_H

#include <stdbool.h>
#include <stdarg.h>

/**
 * Log levels enumeration
 */
typedef enum {
    NEXUS_LOG_LEVEL_TRACE,    /* Verbose tracing information */
    NEXUS_LOG_LEVEL_DEBUG,    /* Debug information */
    NEXUS_LOG_LEVEL_INFO,     /* General information */
    NEXUS_LOG_LEVEL_WARNING,  /* Warnings */
    NEXUS_LOG_LEVEL_ERROR,    /* Errors */
    NEXUS_LOG_LEVEL_FATAL     /* Fatal errors */
} NexusLogLevel;

/**
 * Log categories enumeration
 */
typedef enum {
    NEXUS_LOG_CATEGORY_GENERAL,    /* General engine messages */
    NEXUS_LOG_CATEGORY_RENDER,     /* Rendering system messages */
    NEXUS_LOG_CATEGORY_AUDIO,      /* Audio system messages */
    NEXUS_LOG_CATEGORY_PHYSICS,    /* Physics system messages */
    NEXUS_LOG_CATEGORY_INPUT,      /* Input system messages */
    NEXUS_LOG_CATEGORY_ECS,        /* ECS system messages */
    NEXUS_LOG_CATEGORY_RESOURCE,   /* Resource management messages */
    NEXUS_LOG_CATEGORY_FILE,       /* File I/O messages */
    NEXUS_LOG_CATEGORY_SCRIPT,     /* Scripting system messages */
    NEXUS_LOG_CATEGORY_NETWORK,    /* Network system messages */
    NEXUS_LOG_CATEGORY_CUSTOM      /* Custom categories */
} NexusLogCategory;

/**
 * Logger callback type
 */
typedef void (*NexusLoggerCallback)(NexusLogLevel level, NexusLogCategory category, 
                                   const char* message, void* user_data);

/**
 * Logger structure
 */
typedef struct NexusLogger {
    NexusLogLevel min_level;              /* Minimum log level to output */
    bool enabled_categories[16];          /* Enabled categories */
    bool console_output;                  /* Output to console flag */
    bool file_output;                     /* Output to file flag */
    char log_file_path[256];              /* Log file path */
    void* file_handle;                    /* Log file handle */
    bool color_output;                    /* Use colored output in console */
    NexusLoggerCallback callback;         /* Custom logging callback */
    void* callback_user_data;             /* Custom callback user data */
} NexusLogger;

/* Logger functions */
NexusLogger* nexus_logger_create(void);
void nexus_logger_destroy(NexusLogger* logger);
void nexus_logger_set_level(NexusLogger* logger, NexusLogLevel level);
NexusLogLevel nexus_logger_get_level(const NexusLogger* logger);
void nexus_logger_enable_category(NexusLogger* logger, NexusLogCategory category);
void nexus_logger_disable_category(NexusLogger* logger, NexusLogCategory category);
bool nexus_logger_is_category_enabled(const NexusLogger* logger, NexusLogCategory category);
void nexus_logger_enable_console_output(NexusLogger* logger, bool enable);
void nexus_logger_enable_file_output(NexusLogger* logger, bool enable, const char* file_path);
void nexus_logger_enable_color_output(NexusLogger* logger, bool enable);
void nexus_logger_set_callback(NexusLogger* logger, NexusLoggerCallback callback, void* user_data);

/* Logging functions */
void nexus_log(NexusLogger* logger, NexusLogLevel level, NexusLogCategory category, const char* fmt, ...);
void nexus_log_trace(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...);
void nexus_log_debug(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...);
void nexus_log_info(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...);
void nexus_log_warning(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...);
void nexus_log_error(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...);
void nexus_log_fatal(NexusLogger* logger, NexusLogCategory category, const char* fmt, ...);

/* Convenience macros */
#ifdef NEXUS_DEBUG
    #define NEXUS_LOG_TRACE(logger, category, ...) nexus_log_trace(logger, category, __VA_ARGS__)
    #define NEXUS_LOG_DEBUG(logger, category, ...) nexus_log_debug(logger, category, __VA_ARGS__)
#else
    #define NEXUS_LOG_TRACE(logger, category, ...) ((void)0)
    #define NEXUS_LOG_DEBUG(logger, category, ...) ((void)0)
#endif

#define NEXUS_LOG_INFO(logger, category, ...) nexus_log_info(logger, category, __VA_ARGS__)
#define NEXUS_LOG_WARNING(logger, category, ...) nexus_log_warning(logger, category, __VA_ARGS__)
#define NEXUS_LOG_ERROR(logger, category, ...) nexus_log_error(logger, category, __VA_ARGS__)
#define NEXUS_LOG_FATAL(logger, category, ...) nexus_log_fatal(logger, category, __VA_ARGS__)

#endif /* NEXUS3D_LOGGER_H */
