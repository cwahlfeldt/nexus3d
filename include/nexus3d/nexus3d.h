/**
 * Nexus3D Game Engine
 * A modern 3D game engine built on SDL3, SDL3 GPU API, cglm and flecs
 */

#ifndef NEXUS3D_H
#define NEXUS3D_H

#include <stdbool.h>

/* Core system includes (in order of dependency) */
#include "nexus3d/core/window.h"
#include "nexus3d/core/config.h"
#include "nexus3d/renderer/camera.h"
#include "nexus3d/renderer/mesh.h"
#include "nexus3d/renderer/shader.h"
#include "nexus3d/renderer/texture.h"
#include "nexus3d/renderer/renderer.h"
#include "nexus3d/input/input.h"
#include "nexus3d/audio/audio.h"
#include "nexus3d/physics/physics.h"
#include "nexus3d/core/engine.h"
#include "nexus3d/core/time.h"

/* Additional renderer includes */
#include "nexus3d/renderer/camera.h"
#include "nexus3d/renderer/mesh.h"
#include "nexus3d/renderer/material.h"
#include "nexus3d/renderer/shader.h"
#include "nexus3d/renderer/texture.h"

/* ECS includes */
#include "nexus3d/ecs/components.h"
#include "nexus3d/ecs/systems.h"

/* Input includes */
#include "nexus3d/input/input.h"

/* Math includes */
#include "nexus3d/math/math_utils.h"

/* Physics includes */
#include "nexus3d/physics/physics.h"

/* Audio includes */
#include "nexus3d/audio/audio.h"

/* Utils */
#include "nexus3d/utils/logger.h"

/* Version info */
#define NEXUS3D_VERSION_MAJOR 0
#define NEXUS3D_VERSION_MINOR 1
#define NEXUS3D_VERSION_PATCH 0

#ifdef __cplusplus
extern "C" {
#endif

/* Main engine init/shutdown functions */
bool nexus3d_init(void);
void nexus3d_shutdown(void);

/* Engine version info */
const char* nexus3d_get_version(void);

#ifdef __cplusplus
}
#endif

#endif /* NEXUS3D_H */
