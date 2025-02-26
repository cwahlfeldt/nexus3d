/**
 * Nexus3D Shader System Implementation
 * Handles shader compilation, loading, and management
 */

#include "nexus3d/renderer/shader.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Vertex attributes structure array */
static const SDL_GPUVertexAttribute s_default_vertex_attributes[] = {
    {
        .format = SDL_GPU_VERTEX_ELEMENT_FORMAT_FLOAT3, /* xyz position */
        .offset = 0,
        .binding = 0,
        .location = 0,
        .semantic_name = "POSITION"
    },
    {
        .format = SDL_GPU_VERTEX_ELEMENT_FORMAT_FLOAT3, /* xyz normal */
        .offset = 12, /* offset after position (3 floats × 4 bytes) */
        .binding = 0,
        .location = 1,
        .semantic_name = "NORMAL"
    },
    {
        .format = SDL_GPU_VERTEX_ELEMENT_FORMAT_FLOAT2, /* uv coordinates */
        .offset = 24, /* offset after position and normal (3+3 floats × 4 bytes) */
        .binding = 0,
        .location = 2,
        .semantic_name = "TEXCOORD"
    },
    {
        .format = SDL_GPU_VERTEX_ELEMENT_FORMAT_FLOAT4, /* rgba color */
        .offset = 32, /* offset after position, normal, and uv (3+3+2 floats × 4 bytes) */
        .binding = 0,
        .location = 3,
        .semantic_name = "COLOR"
    }
};

/* Number of vertex attributes */
static const int s_num_vertex_attributes = sizeof(s_default_vertex_attributes) / sizeof(s_default_vertex_attributes[0]);

/**
 * Create a shader
 */
NexusShader* nexus_shader_create(SDL_GPUDevice* device, const char* name) {
    /* Check for null parameters */
    if (device == NULL) {
        fprintf(stderr, "GPU device cannot be NULL when creating shader!\n");
        return NULL;
    }
    
    /* Allocate shader structure */
    NexusShader* shader = (NexusShader*)malloc(sizeof(NexusShader));
    if (shader == NULL) {
        fprintf(stderr, "Failed to allocate memory for shader!\n");
        return NULL;
    }
    
    /* Initialize shader structure */
    memset(shader, 0, sizeof(NexusShader));
    
    /* Store GPU device reference */
    shader->device = device;
    
    /* Set shader name */
    if (name != NULL) {
        strncpy(shader->name, name, sizeof(shader->name) - 1);
        shader->name[sizeof(shader->name) - 1] = '\0';
    } else {
        strcpy(shader->name, "Unnamed Shader");
    }
    
    return shader;
}

/**
 * Destroy a shader
 */
void nexus_shader_destroy(NexusShader* shader) {
    if (shader == NULL) {
        return;
    }
    
    /* Release the shaders */
    if (shader->vertex_shader != NULL) {
        SDL_ReleaseGPUShader(shader->device, shader->vertex_shader);
        shader->vertex_shader = NULL;
    }
    
    if (shader->fragment_shader != NULL) {
        SDL_ReleaseGPUShader(shader->device, shader->fragment_shader);
        shader->fragment_shader = NULL;
    }
    
    /* Release the pipeline */
    if (shader->pipeline != NULL) {
        SDL_ReleaseGPUGraphicsPipeline(shader->device, shader->pipeline);
        shader->pipeline = NULL;
    }
    
    /* Free shader structure */
    free(shader);
}

/**
 * Load shader from source
 */
bool nexus_shader_load_from_source(NexusShader* shader, NexusShaderType type, 
                                 NexusShaderLanguage language, const char* source) {
    if (shader == NULL || source == NULL) {
        return false;
    }
    
    /* Create shader based on type */
    SDL_GPUShaderCreateInfo createInfo = {0};
    SDL_GPUShaderStage stage;
    
    /* Set shader stage based on type */
    switch (type) {
        case NEXUS_SHADER_TYPE_VERTEX:
            stage = SDL_GPU_SHADER_STAGE_VERTEX;
            break;
        case NEXUS_SHADER_TYPE_FRAGMENT:
            stage = SDL_GPU_SHADER_STAGE_FRAGMENT;
            break;
        case NEXUS_SHADER_TYPE_COMPUTE:
            stage = SDL_GPU_SHADER_STAGE_COMPUTE;
            break;
        default:
            fprintf(stderr, "Invalid shader type: %d\n", type);
            return false;
    }
    
    /* Set shader format based on language */
    switch (language) {
        case NEXUS_SHADER_LANGUAGE_GLSL:
            createInfo.format = SDL_GPU_SHADER_FORMAT_GLSL;
            break;
        case NEXUS_SHADER_LANGUAGE_HLSL:
            createInfo.format = SDL_GPU_SHADER_FORMAT_HLSL;
            break;
        case NEXUS_SHADER_LANGUAGE_MSL:
            createInfo.format = SDL_GPU_SHADER_FORMAT_MSL;
            break;
        case NEXUS_SHADER_LANGUAGE_SPIRV:
            createInfo.format = SDL_GPU_SHADER_FORMAT_SPIRV;
            break;
        default:
            fprintf(stderr, "Invalid shader language: %d\n", language);
            return false;
    }
    
    /* Configure shader creation info */
    createInfo.stage = stage;
    createInfo.src.text = source;
    createInfo.src.spirv_bytes = NULL;
    createInfo.src.spirv_byte_length = 0;
    createInfo.entry_point = "main";
    
    /* Create the shader */
    SDL_GPUShader* gpu_shader = SDL_CreateGPUShader(shader->device, &createInfo);
    if (gpu_shader == NULL) {
        fprintf(stderr, "Failed to create shader: %s\n", SDL_GetError());
        return false;
    }
    
    /* Store the shader based on type */
    switch (type) {
        case NEXUS_SHADER_TYPE_VERTEX:
            /* Release previous vertex shader if exists */
            if (shader->vertex_shader != NULL) {
                SDL_ReleaseGPUShader(shader->device, shader->vertex_shader);
            }
            shader->vertex_shader = gpu_shader;
            break;
        case NEXUS_SHADER_TYPE_FRAGMENT:
            /* Release previous fragment shader if exists */
            if (shader->fragment_shader != NULL) {
                SDL_ReleaseGPUShader(shader->device, shader->fragment_shader);
            }
            shader->fragment_shader = gpu_shader;
            break;
        default:
            /* Unsupported shader type for graphics pipeline */
            SDL_ReleaseGPUShader(shader->device, gpu_shader);
            return false;
    }
    
    return true;
}

/**
 * Load shader from file
 */
bool nexus_shader_load_from_file(NexusShader* shader, NexusShaderType type, 
                               NexusShaderLanguage language, const char* filename) {
    if (shader == NULL || filename == NULL) {
        return false;
    }
    
    /* Open file */
    FILE* file = fopen(filename, "rb");
    if (file == NULL) {
        fprintf(stderr, "Failed to open shader file: %s\n", filename);
        return false;
    }
    
    /* Get file size */
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    /* Allocate memory for file content */
    char* source = (char*)malloc(file_size + 1);
    if (source == NULL) {
        fprintf(stderr, "Failed to allocate memory for shader source!\n");
        fclose(file);
        return false;
    }
    
    /* Read file content */
    size_t read_size = fread(source, 1, file_size, file);
    fclose(file);
    
    if (read_size != (size_t)file_size) {
        fprintf(stderr, "Failed to read shader file: %s\n", filename);
        free(source);
        return false;
    }
    
    /* Null-terminate the source */
    source[file_size] = '\0';
    
    /* Load shader from source */
    bool result = nexus_shader_load_from_source(shader, type, language, source);
    
    /* Free source */
    free(source);
    
    return result;
}

/**
 * Compile shader
 */
bool nexus_shader_compile(NexusShader* shader) {
    if (shader == NULL || shader->vertex_shader == NULL || shader->fragment_shader == NULL) {
        fprintf(stderr, "Cannot compile shader: missing shader modules!\n");
        return false;
    }
    
    /* Create graphics pipeline */
    SDL_GPUGraphicsPipelineCreateInfo createInfo = {0};
    
    /* Set shader stages */
    createInfo.vertex_shader = shader->vertex_shader;
    createInfo.fragment_shader = shader->fragment_shader;
    
    /* Configure vertex attributes */
    SDL_GPUVertexInputState vertexInput = {0};
    
    /* Set up vertex buffer binding */
    SDL_GPUVertexBufferDescription vertexBuffer = {
        .stride = sizeof(float) * (3 + 3 + 2 + 4), /* pos(3) + normal(3) + uv(2) + color(4) */
        .input_rate = SDL_GPU_VERTEX_INPUT_RATE_VERTEX,
        .binding = 0
    };
    
    vertexInput.vertex_buffer_descriptions = &vertexBuffer;
    vertexInput.num_vertex_buffer_descriptions = 1;
    vertexInput.vertex_attributes = s_default_vertex_attributes;
    vertexInput.num_vertex_attributes = s_num_vertex_attributes;
    
    createInfo.vertex_input = &vertexInput;
    
    /* Set up rasterizer state */
    SDL_GPURasterizerState rasterizerState = {
        .fill_mode = SDL_GPU_FILL_MODE_SOLID,
        .cull_mode = SDL_GPU_CULL_MODE_BACK,
        .front_face = SDL_GPU_FRONT_FACE_COUNTER_CLOCKWISE,
        .depth_clip_enable = true,
        .scissor_enable = false,
        .line_width = 1.0f
    };
    
    createInfo.rasterizer = &rasterizerState;
    
    /* Set up depth-stencil state */
    SDL_GPUDepthStencilState depthStencilState = {
        .depth_test_enable = true,
        .depth_write_enable = true,
        .depth_compare_op = SDL_GPU_COMPARE_OP_LESS,
        .stencil_test_enable = false,
        .front = {
            .fail_op = SDL_GPU_STENCIL_OP_KEEP,
            .pass_op = SDL_GPU_STENCIL_OP_KEEP,
            .depth_fail_op = SDL_GPU_STENCIL_OP_KEEP,
            .compare_op = SDL_GPU_COMPARE_OP_ALWAYS,
            .compare_mask = 0xFF,
            .write_mask = 0xFF,
            .reference = 0
        },
        .back = {
            .fail_op = SDL_GPU_STENCIL_OP_KEEP,
            .pass_op = SDL_GPU_STENCIL_OP_KEEP,
            .depth_fail_op = SDL_GPU_STENCIL_OP_KEEP,
            .compare_op = SDL_GPU_COMPARE_OP_ALWAYS,
            .compare_mask = 0xFF,
            .write_mask = 0xFF,
            .reference = 0
        }
    };
    
    createInfo.depth_stencil = &depthStencilState;
    
    /* Set up multisample state */
    SDL_GPUMultisampleState multisampleState = {
        .sample_count = SDL_GPU_SAMPLE_COUNT_1,
        .sample_mask = 0xFFFFFFFF,
        .alpha_to_coverage_enable = false
    };
    
    createInfo.multisample = &multisampleState;
    
    /* Set up color blend state for the render target */
    SDL_GPUColorTargetBlendState colorTargetBlendState = {
        .blend_enable = false,
        .src_color_blend_factor = SDL_GPU_BLEND_FACTOR_ONE,
        .dst_color_blend_factor = SDL_GPU_BLEND_FACTOR_ZERO,
        .color_blend_op = SDL_GPU_BLEND_OP_ADD,
        .src_alpha_blend_factor = SDL_GPU_BLEND_FACTOR_ONE,
        .dst_alpha_blend_factor = SDL_GPU_BLEND_FACTOR_ZERO,
        .alpha_blend_op = SDL_GPU_BLEND_OP_ADD,
        .color_write_mask = SDL_GPU_COLOR_COMPONENT_R | SDL_GPU_COLOR_COMPONENT_G | 
                           SDL_GPU_COLOR_COMPONENT_B | SDL_GPU_COLOR_COMPONENT_A
    };
    
    /* Set up render target info */
    SDL_GPUColorTargetDescription colorTarget = {
        .format = SDL_GPU_TEXTURE_FORMAT_R8G8B8A8_UNORM,
        .blend = &colorTargetBlendState
    };
    
    /* Set up pipeline target info */
    SDL_GPUGraphicsPipelineTargetInfo targetInfo = {
        .color_targets = &colorTarget,
        .num_color_targets = 1,
        .depth_stencil_format = SDL_GPU_TEXTURE_FORMAT_D32_FLOAT
    };
    
    createInfo.target = &targetInfo;
    
    /* Create the pipeline */
    SDL_GPUGraphicsPipeline* pipeline = SDL_CreateGPUGraphicsPipeline(shader->device, &createInfo);
    if (pipeline == NULL) {
        fprintf(stderr, "Failed to create graphics pipeline: %s\n", SDL_GetError());
        return false;
    }
    
    /* Release previous pipeline if exists */
    if (shader->pipeline != NULL) {
        SDL_ReleaseGPUGraphicsPipeline(shader->device, shader->pipeline);
    }
    
    /* Store the pipeline */
    shader->pipeline = pipeline;
    
    return true;
}

/**
 * Bind shader to render pass
 */
void nexus_shader_bind(NexusShader* shader, SDL_GPURenderPass* render_pass) {
    if (shader == NULL || render_pass == NULL || shader->pipeline == NULL) {
        return;
    }
    
    /* Bind the pipeline to the render pass */
    SDL_BindGPUGraphicsPipeline(render_pass, shader->pipeline);
}

/**
 * Set uniform float
 */
void nexus_shader_set_uniform_float(NexusShader* shader, const char* name, float value) {
    if (shader == NULL || name == NULL) {
        return;
    }
    
    /* TODO: Implement uniform binding based on shader reflection or manual slot assignments */
    /* For now, this is a placeholder as the actual implementation depends on how uniforms
     * are handled in the SDL3 GPU API */
}

/**
 * Set uniform float2
 */
void nexus_shader_set_uniform_float2(NexusShader* shader, const char* name, float x, float y) {
    if (shader == NULL || name == NULL) {
        return;
    }
    
    /* TODO: Implement uniform binding */
}

/**
 * Set uniform float3
 */
void nexus_shader_set_uniform_float3(NexusShader* shader, const char* name, float x, float y, float z) {
    if (shader == NULL || name == NULL) {
        return;
    }
    
    /* TODO: Implement uniform binding */
}

/**
 * Set uniform float4
 */
void nexus_shader_set_uniform_float4(NexusShader* shader, const char* name, float x, float y, float z, float w) {
    if (shader == NULL || name == NULL) {
        return;
    }
    
    /* TODO: Implement uniform binding */
}

/**
 * Set uniform int
 */
void nexus_shader_set_uniform_int(NexusShader* shader, const char* name, int value) {
    if (shader == NULL || name == NULL) {
        return;
    }
    
    /* TODO: Implement uniform binding */
}

/**
 * Set uniform matrix4
 */
void nexus_shader_set_uniform_matrix4(NexusShader* shader, const char* name, const float* matrix) {
    if (shader == NULL || name == NULL || matrix == NULL) {
        return;
    }
    
    /* TODO: Implement uniform binding */
}
