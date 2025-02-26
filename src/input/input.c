/**
 * Nexus3D Input System Implementation
 * Handles keyboard, mouse, and gamepad input
 */

#include "nexus3d/input/input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/* Input system structure creation */
NexusInput* nexus_input_create(void) {
    /* Allocate input structure */
    NexusInput* input = (NexusInput*)malloc(sizeof(NexusInput));
    if (input == NULL) {
        printf("Failed to allocate memory for input system!\n");
        return NULL;
    }

    /* Initialize input structure */
    memset(input, 0, sizeof(NexusInput));
    
    /* Set default values */
    input->keyboard_enabled = true;
    input->mouse_enabled = true;
    input->mouse_visible = true;
    input->gamepad_enabled = true;
    input->touch_enabled = true;
    input->input_deadzone = 0.1f; /* 10% deadzone */
    
    /* Initialize action map */
    memset(input->action_map, 0, sizeof(input->action_map));
    
    printf("Input system created\n");
    
    return input;
}

/* Input system destruction */
void nexus_input_destroy(NexusInput* input) {
    if (input == NULL) {
        return;
    }
    
    /* Close all gamepads */
    if (input->gamepad_enabled) {
        for (int i = 0; i < NEXUS_MAX_GAMEPADS; i++) {
            if (input->gamepads[i] != NULL) {
                SDL_CloseGamepad(input->gamepads[i]);
                input->gamepads[i] = NULL;
            }
        }
    }
    
    /* Free input structure */
    free(input);
}

/* Process an SDL event */
void nexus_input_process_event(NexusInput* input, const SDL_Event* event) {
    if (input == NULL || event == NULL) {
        return;
    }
    
    /* Process event based on type */
    switch (event->type) {
        /* Keyboard events */
        case SDL_EVENT_KEY_DOWN:
            if (input->keyboard_enabled && event->key.repeat == 0) {
                input->keys_pressed[event->key.scancode] = true;
                input->keys_down[event->key.scancode] = true;
            }
            break;
            
        case SDL_EVENT_KEY_UP:
            if (input->keyboard_enabled) {
                input->keys_released[event->key.scancode] = true;
                input->keys_down[event->key.scancode] = false;
            }
            break;
            
        /* Mouse events */
        case SDL_EVENT_MOUSE_MOTION:
            if (input->mouse_enabled) {
                input->mouse_delta_x = event->motion.x - input->mouse_x;
                input->mouse_delta_y = event->motion.y - input->mouse_y;
                input->mouse_x = event->motion.x;
                input->mouse_y = event->motion.y;
            }
            break;
            
        case SDL_EVENT_MOUSE_BUTTON_DOWN:
            if (input->mouse_enabled) {
                switch (event->button.button) {
                    case SDL_BUTTON_LEFT:
                        input->mouse_buttons_pressed[NEXUS_MOUSE_BUTTON_LEFT] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_LEFT] = true;
                        break;
                    case SDL_BUTTON_RIGHT:
                        input->mouse_buttons_pressed[NEXUS_MOUSE_BUTTON_RIGHT] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_RIGHT] = true;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        input->mouse_buttons_pressed[NEXUS_MOUSE_BUTTON_MIDDLE] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_MIDDLE] = true;
                        break;
                    case SDL_BUTTON_X1:
                        input->mouse_buttons_pressed[NEXUS_MOUSE_BUTTON_X1] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_X1] = true;
                        break;
                    case SDL_BUTTON_X2:
                        input->mouse_buttons_pressed[NEXUS_MOUSE_BUTTON_X2] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_X2] = true;
                        break;
                }
            }
            break;
            
        case SDL_EVENT_MOUSE_BUTTON_UP:
            if (input->mouse_enabled) {
                switch (event->button.button) {
                    case SDL_BUTTON_LEFT:
                        input->mouse_buttons_released[NEXUS_MOUSE_BUTTON_LEFT] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_LEFT] = false;
                        break;
                    case SDL_BUTTON_RIGHT:
                        input->mouse_buttons_released[NEXUS_MOUSE_BUTTON_RIGHT] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_RIGHT] = false;
                        break;
                    case SDL_BUTTON_MIDDLE:
                        input->mouse_buttons_released[NEXUS_MOUSE_BUTTON_MIDDLE] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_MIDDLE] = false;
                        break;
                    case SDL_BUTTON_X1:
                        input->mouse_buttons_released[NEXUS_MOUSE_BUTTON_X1] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_X1] = false;
                        break;
                    case SDL_BUTTON_X2:
                        input->mouse_buttons_released[NEXUS_MOUSE_BUTTON_X2] = true;
                        input->mouse_buttons[NEXUS_MOUSE_BUTTON_X2] = false;
                        break;
                }
            }
            break;
            
        case SDL_EVENT_MOUSE_WHEEL:
            if (input->mouse_enabled) {
                input->mouse_wheel_x = event->wheel.x;
                input->mouse_wheel_y = event->wheel.y;
            }
            break;
            
        /* Gamepad events */
        case SDL_EVENT_GAMEPAD_ADDED:
            if (input->gamepad_enabled && event->gdevice.which < NEXUS_MAX_GAMEPADS) {
                /* Open gamepad */
                input->gamepads[event->gdevice.which] = SDL_OpenGamepad(event->gdevice.which);
                if (input->gamepads[event->gdevice.which] != NULL) {
                    input->gamepad_connected[event->gdevice.which] = true;
                    printf("Gamepad connected: %s\n", SDL_GetGamepadName(input->gamepads[event->gdevice.which]));
                }
            }
            break;
            
        case SDL_EVENT_GAMEPAD_REMOVED:
            if (input->gamepad_enabled && event->gdevice.which < NEXUS_MAX_GAMEPADS) {
                /* Close gamepad */
                if (input->gamepads[event->gdevice.which] != NULL) {
                    SDL_CloseGamepad(input->gamepads[event->gdevice.which]);
                    input->gamepads[event->gdevice.which] = NULL;
                    input->gamepad_connected[event->gdevice.which] = false;
                    
                    /* Clear all gamepad state */
                    memset(input->gamepad_axes[event->gdevice.which], 0, sizeof(float) * SDL_GAMEPAD_AXIS_COUNT);
                    memset(input->gamepad_buttons[event->gdevice.which], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
                    memset(input->gamepad_buttons_pressed[event->gdevice.which], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
                    memset(input->gamepad_buttons_released[event->gdevice.which], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
                    
                    printf("Gamepad disconnected\n");
                }
            }
            break;
            
        case SDL_EVENT_GAMEPAD_AXIS_MOTION:
            if (input->gamepad_enabled && 
                event->gaxis.which < NEXUS_MAX_GAMEPADS && 
                event->gaxis.axis < SDL_GAMEPAD_AXIS_COUNT) {
                
                /* Convert to -1.0 to 1.0 range */
                float value = (float)event->gaxis.value / 32767.0f;
                
                /* Apply deadzone */
                if (fabsf(value) < input->input_deadzone) {
                    value = 0.0f;
                } else {
                    /* Scale value to 0-1 range after deadzone */
                    float sign = (value > 0.0f) ? 1.0f : -1.0f;
                    value = sign * (fabsf(value) - input->input_deadzone) / (1.0f - input->input_deadzone);
                }
                
                input->gamepad_axes[event->gaxis.which][event->gaxis.axis] = value;
            }
            break;
            
        case SDL_EVENT_GAMEPAD_BUTTON_DOWN:
            if (input->gamepad_enabled && 
                event->gbutton.which < NEXUS_MAX_GAMEPADS && 
                event->gbutton.button < SDL_GAMEPAD_BUTTON_COUNT) {
                
                input->gamepad_buttons_pressed[event->gbutton.which][event->gbutton.button] = true;
                input->gamepad_buttons[event->gbutton.which][event->gbutton.button] = true;
            }
            break;
            
        case SDL_EVENT_GAMEPAD_BUTTON_UP:
            if (input->gamepad_enabled && 
                event->gbutton.which < NEXUS_MAX_GAMEPADS && 
                event->gbutton.button < SDL_GAMEPAD_BUTTON_COUNT) {
                
                input->gamepad_buttons_released[event->gbutton.which][event->gbutton.button] = true;
                input->gamepad_buttons[event->gbutton.which][event->gbutton.button] = false;
            }
            break;
            
        /* Window events */
        case SDL_EVENT_WINDOW_FOCUS_LOST:
            /* Reset all input when window loses focus for safety */
            nexus_input_reset_states(input);
            break;
    }
}

/* Update input system */
void nexus_input_update(NexusInput* input) {
    if (input == NULL) {
        return;
    }
    
    /* Store previous key states */
    memcpy(input->keys_prev, input->keys_down, sizeof(input->keys_prev));
    
    /* Store previous mouse button states */
    memcpy(input->mouse_buttons_prev, input->mouse_buttons, sizeof(input->mouse_buttons_prev));
    
    /* Store previous gamepad button states */
    for (int i = 0; i < NEXUS_MAX_GAMEPADS; i++) {
        memcpy(input->gamepad_buttons_prev[i], input->gamepad_buttons[i], sizeof(input->gamepad_buttons_prev[i]));
    }
    
    /* Reset one-frame events */
    memset(input->keys_pressed, 0, sizeof(input->keys_pressed));
    memset(input->keys_released, 0, sizeof(input->keys_released));
    memset(input->mouse_buttons_pressed, 0, sizeof(input->mouse_buttons_pressed));
    memset(input->mouse_buttons_released, 0, sizeof(input->mouse_buttons_released));
    input->mouse_wheel_x = 0.0f;
    input->mouse_wheel_y = 0.0f;
    input->mouse_delta_x = 0.0f;
    input->mouse_delta_y = 0.0f;
    
    for (int i = 0; i < NEXUS_MAX_GAMEPADS; i++) {
        memset(input->gamepad_buttons_pressed[i], 0, sizeof(input->gamepad_buttons_pressed[i]));
        memset(input->gamepad_buttons_released[i], 0, sizeof(input->gamepad_buttons_released[i]));
    }
    
    /* Update gamepad state */
    SDL_UpdateGamepads();
}

/* Reset all input states */
void nexus_input_reset_states(NexusInput* input) {
    if (input == NULL) {
        return;
    }
    
    /* Reset keyboard state */
    memset(input->keys_down, 0, sizeof(input->keys_down));
    memset(input->keys_pressed, 0, sizeof(input->keys_pressed));
    memset(input->keys_released, 0, sizeof(input->keys_released));
    memset(input->keys_prev, 0, sizeof(input->keys_prev));
    
    /* Reset mouse state */
    memset(input->mouse_buttons, 0, sizeof(input->mouse_buttons));
    memset(input->mouse_buttons_pressed, 0, sizeof(input->mouse_buttons_pressed));
    memset(input->mouse_buttons_released, 0, sizeof(input->mouse_buttons_released));
    memset(input->mouse_buttons_prev, 0, sizeof(input->mouse_buttons_prev));
    input->mouse_wheel_x = 0.0f;
    input->mouse_wheel_y = 0.0f;
    input->mouse_delta_x = 0.0f;
    input->mouse_delta_y = 0.0f;
    
    /* Reset gamepad state */
    for (int i = 0; i < NEXUS_MAX_GAMEPADS; i++) {
        memset(input->gamepad_axes[i], 0, sizeof(float) * SDL_GAMEPAD_AXIS_COUNT);
        memset(input->gamepad_buttons[i], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
        memset(input->gamepad_buttons_pressed[i], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
        memset(input->gamepad_buttons_released[i], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
        memset(input->gamepad_buttons_prev[i], 0, sizeof(bool) * SDL_GAMEPAD_AXIS_COUNT);
    }
}

/* --- Keyboard Functions --- */

/* Enable/disable keyboard input */
void nexus_input_set_keyboard_enabled(NexusInput* input, bool enabled) {
    if (input == NULL) {
        return;
    }
    
    input->keyboard_enabled = enabled;
}

/* Check if keyboard input is enabled */
bool nexus_input_is_keyboard_enabled(const NexusInput* input) {
    if (input == NULL) {
        return false;
    }
    
    return input->keyboard_enabled;
}

/* Get key state */
NexusKeyState nexus_input_get_key_state(const NexusInput* input, SDL_Scancode key) {
    if (input == NULL || !input->keyboard_enabled || key < 0 || key >= SDL_SCANCODE_COUNT) {
        return NEXUS_KEY_UP;
    }
    
    if (input->keys_pressed[key]) {
        return NEXUS_KEY_PRESSED;
    } else if (input->keys_released[key]) {
        return NEXUS_KEY_RELEASED;
    } else if (input->keys_down[key]) {
        return NEXUS_KEY_DOWN;
    } else {
        return NEXUS_KEY_UP;
    }
}

/* Check if key is down */
bool nexus_input_is_key_down(const NexusInput* input, SDL_Scancode key) {
    if (input == NULL || !input->keyboard_enabled || key < 0 || key >= SDL_SCANCODE_COUNT) {
        return false;
    }
    
    return input->keys_down[key];
}

/* Check if key was just pressed */
bool nexus_input_is_key_pressed(const NexusInput* input, SDL_Scancode key) {
    if (input == NULL || !input->keyboard_enabled || key < 0 || key >= SDL_SCANCODE_COUNT) {
        return false;
    }
    
    return input->keys_pressed[key];
}

/* Check if key was just released */
bool nexus_input_is_key_released(const NexusInput* input, SDL_Scancode key) {
    if (input == NULL || !input->keyboard_enabled || key < 0 || key >= SDL_SCANCODE_COUNT) {
        return false;
    }
    
    return input->keys_released[key];
}

/* Check if any key is down */
bool nexus_input_is_any_key_down(const NexusInput* input) {
    if (input == NULL || !input->keyboard_enabled) {
        return false;
    }
    
    for (int i = 0; i < SDL_SCANCODE_COUNT; i++) {
        if (input->keys_down[i]) {
            return true;
        }
    }
    
    return false;
}

/* --- Mouse Functions --- */

/* Enable/disable mouse input */
void nexus_input_set_mouse_enabled(NexusInput* input, bool enabled) {
    if (input == NULL) {
        return;
    }
    
    input->mouse_enabled = enabled;
}

/* Check if mouse input is enabled */
bool nexus_input_is_mouse_enabled(const NexusInput* input) {
    if (input == NULL) {
        return false;
    }
    
    return input->mouse_enabled;
}

/* Set mouse captured */
void nexus_input_set_mouse_captured(NexusInput* input, bool captured) {
    if (input == NULL) {
        return;
    }
    
    input->mouse_captured = captured;
    SDL_CaptureMouse(captured);
}

/* Check if mouse is captured */
bool nexus_input_is_mouse_captured(const NexusInput* input) {
    if (input == NULL) {
        return false;
    }
    
    return input->mouse_captured;
}

/* Set mouse visibility */
void nexus_input_set_mouse_visible(NexusInput* input, bool visible) {
    if (input == NULL) {
        return;
    }
    
    input->mouse_visible = visible;
    
    if (visible) {
        SDL_ShowCursor();
    } else {
        SDL_HideCursor();
    }
}

/* Check if mouse is visible */
bool nexus_input_is_mouse_visible(const NexusInput* input) {
    if (input == NULL) {
        return true;
    }
    
    return input->mouse_visible;
}

/* Get mouse position */
void nexus_input_get_mouse_position(const NexusInput* input, float* x, float* y) {
    if (input == NULL || !input->mouse_enabled) {
        if (x) *x = 0.0f;
        if (y) *y = 0.0f;
        return;
    }
    
    if (x) *x = input->mouse_x;
    if (y) *y = input->mouse_y;
}

/* Get mouse movement since last frame */
void nexus_input_get_mouse_delta(const NexusInput* input, float* dx, float* dy) {
    if (input == NULL || !input->mouse_enabled) {
        if (dx) *dx = 0.0f;
        if (dy) *dy = 0.0f;
        return;
    }
    
    if (dx) *dx = input->mouse_delta_x;
    if (dy) *dy = input->mouse_delta_y;
}

/* Get mouse wheel movement */
void nexus_input_get_mouse_wheel(const NexusInput* input, float* x, float* y) {
    if (input == NULL || !input->mouse_enabled) {
        if (x) *x = 0.0f;
        if (y) *y = 0.0f;
        return;
    }
    
    if (x) *x = input->mouse_wheel_x;
    if (y) *y = input->mouse_wheel_y;
}

/* Get mouse button state */
NexusKeyState nexus_input_get_mouse_button_state(const NexusInput* input, NexusMouseButton button) {
    if (input == NULL || !input->mouse_enabled || button < 0 || button >= NEXUS_MOUSE_BUTTON_COUNT) {
        return NEXUS_KEY_UP;
    }
    
    if (input->mouse_buttons_pressed[button]) {
        return NEXUS_KEY_PRESSED;
    } else if (input->mouse_buttons_released[button]) {
        return NEXUS_KEY_RELEASED;
    } else if (input->mouse_buttons[button]) {
        return NEXUS_KEY_DOWN;
    } else {
        return NEXUS_KEY_UP;
    }
}

/* Check if mouse button is down */
bool nexus_input_is_mouse_button_down(const NexusInput* input, NexusMouseButton button) {
    if (input == NULL || !input->mouse_enabled || button < 0 || button >= NEXUS_MOUSE_BUTTON_COUNT) {
        return false;
    }
    
    return input->mouse_buttons[button];
}

/* Check if mouse button was just pressed */
bool nexus_input_is_mouse_button_pressed(const NexusInput* input, NexusMouseButton button) {
    if (input == NULL || !input->mouse_enabled || button < 0 || button >= NEXUS_MOUSE_BUTTON_COUNT) {
        return false;
    }
    
    return input->mouse_buttons_pressed[button];
}

/* Check if mouse button was just released */
bool nexus_input_is_mouse_button_released(const NexusInput* input, NexusMouseButton button) {
    if (input == NULL || !input->mouse_enabled || button < 0 || button >= NEXUS_MOUSE_BUTTON_COUNT) {
        return false;
    }
    
    return input->mouse_buttons_released[button];
}

/* --- Gamepad Functions --- */

/* Enable/disable gamepad input */
void nexus_input_set_gamepad_enabled(NexusInput* input, bool enabled) {
    if (input == NULL) {
        return;
    }
    
    input->gamepad_enabled = enabled;
}

/* Check if gamepad input is enabled */
bool nexus_input_is_gamepad_enabled(const NexusInput* input) {
    if (input == NULL) {
        return false;
    }
    
    return input->gamepad_enabled;
}

/* Check if gamepad is connected */
bool nexus_input_is_gamepad_connected(const NexusInput* input, int gamepad_index) {
    if (input == NULL || !input->gamepad_enabled || gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS) {
        return false;
    }
    
    return input->gamepad_connected[gamepad_index];
}

/* Get gamepad axis value */
float nexus_input_get_gamepad_axis(const NexusInput* input, int gamepad_index, SDL_GamepadAxis axis) {
    if (input == NULL || !input->gamepad_enabled || 
        gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS ||
        axis < 0 || axis >= SDL_GAMEPAD_AXIS_COUNT ||
        !input->gamepad_connected[gamepad_index]) {
        return 0.0f;
    }
    
    return input->gamepad_axes[gamepad_index][axis];
}

/* Get gamepad button state */
NexusKeyState nexus_input_get_gamepad_button_state(const NexusInput* input, int gamepad_index, SDL_GamepadButton button) {
    if (input == NULL || !input->gamepad_enabled || 
        gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS ||
        button < 0 || button >= SDL_GAMEPAD_BUTTON_COUNT ||
        !input->gamepad_connected[gamepad_index]) {
        return NEXUS_KEY_UP;
    }
    
    if (input->gamepad_buttons_pressed[gamepad_index][button]) {
        return NEXUS_KEY_PRESSED;
    } else if (input->gamepad_buttons_released[gamepad_index][button]) {
        return NEXUS_KEY_RELEASED;
    } else if (input->gamepad_buttons[gamepad_index][button]) {
        return NEXUS_KEY_DOWN;
    } else {
        return NEXUS_KEY_UP;
    }
}

/* Check if gamepad button is down */
bool nexus_input_is_gamepad_button_down(const NexusInput* input, int gamepad_index, SDL_GamepadButton button) {
    if (input == NULL || !input->gamepad_enabled || 
        gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS ||
        button < 0 || button >= SDL_GAMEPAD_BUTTON_COUNT ||
        !input->gamepad_connected[gamepad_index]) {
        return false;
    }
    
    return input->gamepad_buttons[gamepad_index][button];
}

/* Check if gamepad button was just pressed */
bool nexus_input_is_gamepad_button_pressed(const NexusInput* input, int gamepad_index, SDL_GamepadButton button) {
    if (input == NULL || !input->gamepad_enabled || 
        gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS ||
        button < 0 || button >= SDL_GAMEPAD_BUTTON_COUNT ||
        !input->gamepad_connected[gamepad_index]) {
        return false;
    }
    
    return input->gamepad_buttons_pressed[gamepad_index][button];
}

/* Check if gamepad button was just released */
bool nexus_input_is_gamepad_button_released(const NexusInput* input, int gamepad_index, SDL_GamepadButton button) {
    if (input == NULL || !input->gamepad_enabled || 
        gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS ||
        button < 0 || button >= SDL_GAMEPAD_BUTTON_COUNT ||
        !input->gamepad_connected[gamepad_index]) {
        return false;
    }
    
    return input->gamepad_buttons_released[gamepad_index][button];
}

/* Get gamepad name */
const char* nexus_input_get_gamepad_name(const NexusInput* input, int gamepad_index) {
    if (input == NULL || !input->gamepad_enabled || 
        gamepad_index < 0 || gamepad_index >= NEXUS_MAX_GAMEPADS ||
        !input->gamepad_connected[gamepad_index] ||
        input->gamepads[gamepad_index] == NULL) {
        return NULL;
    }
    
    return SDL_GetGamepadName(input->gamepads[gamepad_index]);
}

/* --- Action Mapping Functions --- */

/* Map action to keyboard key */
void nexus_input_map_action(NexusInput* input, const char* action_name, SDL_Scancode key) {
    if (input == NULL || action_name == NULL || key < 0 || key >= SDL_SCANCODE_COUNT) {
        return;
    }
    
    /* Find empty action slot or existing action with same name */
    int slot = -1;
    for (int i = 0; i < 256; i++) {
        if (input->action_map[i][0] == '\0') {
            /* Empty slot */
            if (slot == -1) {
                slot = i;
            }
        } else if (strcmp(input->action_map[i], action_name) == 0) {
            /* Existing action */
            slot = i;
            break;
        }
    }
    
    /* No slot found */
    if (slot == -1) {
        printf("Action map is full!\n");
        return;
    }
    
    /* Store action name */
    strncpy(input->action_map[slot], action_name, 31);
    input->action_map[slot][31] = '\0';
}

/* Map action to mouse button */
void nexus_input_map_action_to_mouse(NexusInput* input, const char* action_name, NexusMouseButton button) {
    /* Implementation depends on how you want to store mouse button mappings */
    /* This is a placeholder implementation */
    if (input == NULL || action_name == NULL || button < 0 || button >= NEXUS_MOUSE_BUTTON_COUNT) {
        return;
    }
    
    /* For now, just print a message */
    printf("Mapped action '%s' to mouse button %d\n", action_name, button);
}

/* Map action to gamepad button */
void nexus_input_map_action_to_gamepad(NexusInput* input, const char* action_name, SDL_GamepadButton button) {
    /* Implementation depends on how you want to store gamepad button mappings */
    /* This is a placeholder implementation */
    if (input == NULL || action_name == NULL || button < 0 || button >= SDL_GAMEPAD_BUTTON_COUNT) {
        return;
    }
    
    /* For now, just print a message */
    printf("Mapped action '%s' to gamepad button %d\n", action_name, button);
}

/* Set deadzone for analog inputs */
void nexus_input_set_deadzone(NexusInput* input, float deadzone) {
    if (input == NULL) {
        return;
    }
    
    /* Clamp deadzone to [0.0, 1.0] */
    if (deadzone < 0.0f) deadzone = 0.0f;
    if (deadzone > 1.0f) deadzone = 1.0f;
    
    input->input_deadzone = deadzone;
}
