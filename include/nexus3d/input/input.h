/**
 * Nexus3D Input System
 * Handles keyboard, mouse, and gamepad input
 */

#ifndef NEXUS3D_INPUT_H
#define NEXUS3D_INPUT_H

#include <SDL3/SDL.h>
#include <stdbool.h>

/* Maximum number of gamepads that can be tracked */
#define NEXUS_MAX_GAMEPADS 8

/**
 * Key state enumeration
 */
typedef enum {
    NEXUS_KEY_UP,         /* Key is up */
    NEXUS_KEY_DOWN,       /* Key is down */
    NEXUS_KEY_PRESSED,    /* Key was just pressed this frame */
    NEXUS_KEY_RELEASED    /* Key was just released this frame */
} NexusKeyState;

/**
 * Mouse button enumeration
 */
typedef enum {
    NEXUS_MOUSE_BUTTON_LEFT,
    NEXUS_MOUSE_BUTTON_RIGHT,
    NEXUS_MOUSE_BUTTON_MIDDLE,
    NEXUS_MOUSE_BUTTON_X1,
    NEXUS_MOUSE_BUTTON_X2,
    NEXUS_MOUSE_BUTTON_COUNT
} NexusMouseButton;

/**
 * Input system structure
 */
typedef struct NexusInput {
    /* Keyboard state */
    bool keyboard_enabled;            /* Keyboard input enabled flag */
    bool keys_down[SDL_NUM_SCANCODES];  /* Current key state (true = down) */
    bool keys_pressed[SDL_NUM_SCANCODES]; /* Keys pressed this frame */
    bool keys_released[SDL_NUM_SCANCODES]; /* Keys released this frame */
    bool keys_prev[SDL_NUM_SCANCODES];  /* Previous frame key state */
    
    /* Mouse state */
    bool mouse_enabled;               /* Mouse input enabled flag */
    bool mouse_captured;              /* Mouse capture flag */
    bool mouse_visible;               /* Mouse visibility flag */
    bool mouse_buttons[NEXUS_MOUSE_BUTTON_COUNT]; /* Current mouse button state */
    bool mouse_buttons_pressed[NEXUS_MOUSE_BUTTON_COUNT]; /* Mouse buttons pressed this frame */
    bool mouse_buttons_released[NEXUS_MOUSE_BUTTON_COUNT]; /* Mouse buttons released this frame */
    bool mouse_buttons_prev[NEXUS_MOUSE_BUTTON_COUNT]; /* Previous frame mouse button state */
    float mouse_x;                    /* Current mouse X position */
    float mouse_y;                    /* Current mouse Y position */
    float mouse_delta_x;              /* Mouse X movement since last frame */
    float mouse_delta_y;              /* Mouse Y movement since last frame */
    float mouse_wheel_x;              /* Mouse wheel X delta */
    float mouse_wheel_y;              /* Mouse wheel Y delta */
    
    /* Gamepad state */
    bool gamepad_enabled;             /* Gamepad input enabled flag */
    SDL_Gamepad* gamepads[NEXUS_MAX_GAMEPADS]; /* Gamepad handles */
    bool gamepad_connected[NEXUS_MAX_GAMEPADS]; /* Gamepad connection state */
    float gamepad_axes[NEXUS_MAX_GAMEPADS][SDL_GAMEPAD_AXIS_MAX]; /* Gamepad axis values */
    bool gamepad_buttons[NEXUS_MAX_GAMEPADS][SDL_GAMEPAD_BUTTON_MAX]; /* Gamepad button state */
    bool gamepad_buttons_pressed[NEXUS_MAX_GAMEPADS][SDL_GAMEPAD_BUTTON_MAX]; /* Gamepad buttons pressed this frame */
    bool gamepad_buttons_released[NEXUS_MAX_GAMEPADS][SDL_GAMEPAD_BUTTON_MAX]; /* Gamepad buttons released this frame */
    bool gamepad_buttons_prev[NEXUS_MAX_GAMEPADS][SDL_GAMEPAD_BUTTON_MAX]; /* Previous frame gamepad button state */
    
    /* Touch state */
    bool touch_enabled;               /* Touch input enabled flag */
    
    /* Input mappings */
    char action_map[256][32];         /* Action name to key/button mapping */
    float input_deadzone;             /* Analog input deadzone */
} NexusInput;

/* Input system functions */
NexusInput* nexus_input_create(void);
void nexus_input_destroy(NexusInput* input);
void nexus_input_update(NexusInput* input);
void nexus_input_process_event(NexusInput* input, const SDL_Event* event);
void nexus_input_reset_states(NexusInput* input);

/* Keyboard functions */
void nexus_input_set_keyboard_enabled(NexusInput* input, bool enabled);
bool nexus_input_is_keyboard_enabled(const NexusInput* input);
NexusKeyState nexus_input_get_key_state(const NexusInput* input, SDL_Scancode key);
bool nexus_input_is_key_down(const NexusInput* input, SDL_Scancode key);
bool nexus_input_is_key_pressed(const NexusInput* input, SDL_Scancode key);
bool nexus_input_is_key_released(const NexusInput* input, SDL_Scancode key);
bool nexus_input_is_any_key_down(const NexusInput* input);

/* Mouse functions */
void nexus_input_set_mouse_enabled(NexusInput* input, bool enabled);
bool nexus_input_is_mouse_enabled(const NexusInput* input);
void nexus_input_set_mouse_captured(NexusInput* input, bool captured);
bool nexus_input_is_mouse_captured(const NexusInput* input);
void nexus_input_set_mouse_visible(NexusInput* input, bool visible);
bool nexus_input_is_mouse_visible(const NexusInput* input);
void nexus_input_get_mouse_position(const NexusInput* input, float* x, float* y);
void nexus_input_get_mouse_delta(const NexusInput* input, float* dx, float* dy);
void nexus_input_get_mouse_wheel(const NexusInput* input, float* x, float* y);
NexusKeyState nexus_input_get_mouse_button_state(const NexusInput* input, NexusMouseButton button);
bool nexus_input_is_mouse_button_down(const NexusInput* input, NexusMouseButton button);
bool nexus_input_is_mouse_button_pressed(const NexusInput* input, NexusMouseButton button);
bool nexus_input_is_mouse_button_released(const NexusInput* input, NexusMouseButton button);

/* Gamepad functions */
void nexus_input_set_gamepad_enabled(NexusInput* input, bool enabled);
bool nexus_input_is_gamepad_enabled(const NexusInput* input);
bool nexus_input_is_gamepad_connected(const NexusInput* input, int gamepad_index);
float nexus_input_get_gamepad_axis(const NexusInput* input, int gamepad_index, SDL_GamepadAxis axis);
NexusKeyState nexus_input_get_gamepad_button_state(const NexusInput* input, int gamepad_index, SDL_GamepadButton button);
bool nexus_input_is_gamepad_button_down(const NexusInput* input, int gamepad_index, SDL_GamepadButton button);
bool nexus_input_is_gamepad_button_pressed(const NexusInput* input, int gamepad_index, SDL_GamepadButton button);
bool nexus_input_is_gamepad_button_released(const NexusInput* input, int gamepad_index, SDL_GamepadButton button);
const char* nexus_input_get_gamepad_name(const NexusInput* input, int gamepad_index);

/* Action mapping functions */
void nexus_input_map_action(NexusInput* input, const char* action_name, SDL_Scancode key);
void nexus_input_map_action_to_mouse(NexusInput* input, const char* action_name, NexusMouseButton button);
void nexus_input_map_action_to_gamepad(NexusInput* input, const char* action_name, SDL_GamepadButton button);
bool nexus_input_is_action_pressed(NexusInput* input, const char* action_name);
bool nexus_input_is_action_released(NexusInput* input, const char* action_name);
bool nexus_input_is_action_down(NexusInput* input, const char* action_name);
float nexus_input_get_action_value(NexusInput* input, const char* action_name);
void nexus_input_set_deadzone(NexusInput* input, float deadzone);

#endif /* NEXUS3D_INPUT_H */
