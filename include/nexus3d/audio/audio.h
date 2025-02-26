/**
 * Nexus3D Audio System
 * Handles audio playback using SDL3 audio
 */

#ifndef NEXUS3D_AUDIO_H
#define NEXUS3D_AUDIO_H

#include <SDL3/SDL.h>
#include <stdbool.h>
#include <cglm/cglm.h>
#include "../core/config.h"

/* Maximum number of sound sources */
#define NEXUS_MAX_SOUND_SOURCES 128

/**
 * Audio format enumeration
 */
typedef enum {
    NEXUS_AUDIO_FORMAT_UNKNOWN,
    NEXUS_AUDIO_FORMAT_WAV,
    NEXUS_AUDIO_FORMAT_OGG,
    NEXUS_AUDIO_FORMAT_MP3
} NexusAudioFormat;

/**
 * Sound structure
 */
typedef struct NexusSound {
    Uint8* buffer;           /* Sound data buffer */
    Uint32 length;           /* Buffer length in bytes */
    SDL_AudioSpec spec;      /* Audio specification */
    char name[64];           /* Sound name */
} NexusSound;

/**
 * Sound source structure
 */
typedef struct NexusSoundSource {
    NexusSound* sound;       /* Sound data */
    bool is_playing;         /* Playing flag */
    bool is_looping;         /* Looping flag */
    float volume;            /* Volume [0.0, 1.0] */
    float pitch;             /* Pitch multiplier */
    float pan;               /* Panning [-1.0 (left) to 1.0 (right)] */
    Uint32 position;         /* Current play position in bytes */
    bool is_3d;              /* 3D positioning flag */
    vec3 position3d;         /* 3D position */
    float min_distance;      /* Minimum distance for 3D attenuation */
    float max_distance;      /* Maximum distance for 3D attenuation */
    float attenuation;       /* Attenuation factor */
    Uint32 id;               /* Source ID */
} NexusSoundSource;

/**
 * Audio system configuration
 */
// typedef struct {
//     int sample_rate;         /* Sample rate in Hz */
//     int channels;            /* Number of output channels */
//     int buffer_size;         /* Audio buffer size */
//     float master_volume;     /* Master volume [0.0, 1.0] */
//     float sfx_volume;        /* Sound effects volume [0.0, 1.0] */
//     float music_volume;      /* Music volume [0.0, 1.0] */
// } NexusAudioConfig;

/**
 * Main audio system structure
 */
typedef struct NexusAudio {
    NexusAudioConfig config;                         /* Audio configuration */
    SDL_AudioDeviceID audio_device;                  /* SDL audio device */
    NexusSoundSource sources[NEXUS_MAX_SOUND_SOURCES]; /* Sound sources array */
    int source_count;                                /* Number of active sources */
    SDL_AudioStream* audio_stream;                   /* SDL audio stream */
    bool initialized;                                /* Initialization flag */
    vec3 listener_position;                          /* Listener position */
    vec3 listener_forward;                           /* Listener forward vector */
    vec3 listener_up;                                /* Listener up vector */
} NexusAudio;

/* Audio system functions */
NexusAudio* nexus_audio_create(const NexusAudioConfig* config);
void nexus_audio_destroy(NexusAudio* audio);
void nexus_audio_update(NexusAudio* audio);
void nexus_audio_pause(NexusAudio* audio, bool pause);
bool nexus_audio_is_paused(const NexusAudio* audio);
void nexus_audio_set_master_volume(NexusAudio* audio, float volume);
float nexus_audio_get_master_volume(const NexusAudio* audio);
void nexus_audio_set_sfx_volume(NexusAudio* audio, float volume);
float nexus_audio_get_sfx_volume(const NexusAudio* audio);
void nexus_audio_set_music_volume(NexusAudio* audio, float volume);
float nexus_audio_get_music_volume(const NexusAudio* audio);

/* Listener functions */
void nexus_audio_set_listener_position(NexusAudio* audio, float x, float y, float z);
void nexus_audio_set_listener_orientation(NexusAudio* audio, float fx, float fy, float fz, float ux, float uy, float uz);
void nexus_audio_get_listener_position(const NexusAudio* audio, vec3 position);
void nexus_audio_get_listener_forward(const NexusAudio* audio, vec3 forward);
void nexus_audio_get_listener_up(const NexusAudio* audio, vec3 up);

/* Sound functions */
NexusSound* nexus_sound_load_from_file(const char* filename);
NexusSound* nexus_sound_load_from_memory(const void* data, size_t size, NexusAudioFormat format);
void nexus_sound_destroy(NexusSound* sound);

/* Sound source functions */
Uint32 nexus_audio_play_sound(NexusAudio* audio, NexusSound* sound, float volume, float pitch, bool loop);
Uint32 nexus_audio_play_sound_3d(NexusAudio* audio, NexusSound* sound, float x, float y, float z, float min_dist, float max_dist, float volume, float pitch, bool loop);
void nexus_audio_stop_sound(NexusAudio* audio, Uint32 source_id);
void nexus_audio_pause_sound(NexusAudio* audio, Uint32 source_id, bool pause);
bool nexus_audio_is_sound_playing(const NexusAudio* audio, Uint32 source_id);
void nexus_audio_set_sound_volume(NexusAudio* audio, Uint32 source_id, float volume);
void nexus_audio_set_sound_pitch(NexusAudio* audio, Uint32 source_id, float pitch);
void nexus_audio_set_sound_pan(NexusAudio* audio, Uint32 source_id, float pan);
void nexus_audio_set_sound_position(NexusAudio* audio, Uint32 source_id, float x, float y, float z);
void nexus_audio_set_sound_min_distance(NexusAudio* audio, Uint32 source_id, float min_distance);
void nexus_audio_set_sound_max_distance(NexusAudio* audio, Uint32 source_id, float max_distance);
void nexus_audio_set_sound_attenuation(NexusAudio* audio, Uint32 source_id, float attenuation);
void nexus_audio_stop_all_sounds(NexusAudio* audio);
void nexus_audio_pause_all_sounds(NexusAudio* audio, bool pause);

#endif /* NEXUS3D_AUDIO_H */
