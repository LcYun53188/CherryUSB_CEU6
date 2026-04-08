#ifndef __APP_MAPPER_H
#define __APP_MAPPER_H

#include <stdint.h>
#include <stdbool.h>

/* ==============================================================================
 *                              GAMEPAD DATA TYPES
 * ==============================================================================*/

// Gamepad Button Enums
typedef enum {
    GAMEPAD_BTN_DUP = 0,
    GAMEPAD_BTN_DDOWN,
    GAMEPAD_BTN_DLEFT,
    GAMEPAD_BTN_DRIGHT,
    GAMEPAD_BTN_START,
    GAMEPAD_BTN_BACK,
    GAMEPAD_BTN_L3,
    GAMEPAD_BTN_R3,
    GAMEPAD_BTN_LB,
    GAMEPAD_BTN_RB,
    GAMEPAD_BTN_XBOX,
    GAMEPAD_BTN_A,
    GAMEPAD_BTN_B,
    GAMEPAD_BTN_X,
    GAMEPAD_BTN_Y,
    GAMEPAD_BTN_MAX
} GamepadButton_t;

// DJI Switch States (Matches DT7 protocol)
#define RC_SW_UP   (1)
#define RC_SW_DOWN (2)
#define RC_SW_MID  (3)

// Gamepad Axis Enums
typedef enum {
    GAMEPAD_AXIS_LX = 0,
    GAMEPAD_AXIS_LY,
    GAMEPAD_AXIS_RX,
    GAMEPAD_AXIS_RY,
    GAMEPAD_AXIS_LT,
    GAMEPAD_AXIS_RT,
    GAMEPAD_AXIS_MAX
} GamepadAxis_t;

// Unified Gamepad State Structure
typedef struct {
    int32_t axes[GAMEPAD_AXIS_MAX];
    bool    buttons[GAMEPAD_BTN_MAX];
} GamepadState_t;

/* ==============================================================================
 *                              MAPPING CONFIGURATION
 * ==============================================================================*/

typedef enum {
    MAP_SRC_NONE = 0,
    MAP_SRC_AXIS,
    MAP_SRC_BTN
} MapSourceType_t;

// Structure defining how an interface input maps to an SBUS channel
typedef struct {
    MapSourceType_t type;    // MAP_SRC_AXIS or MAP_SRC_BTN or MAP_SRC_NONE
    uint8_t         index;   // Which axis or button (e.g. GAMEPAD_AXIS_LX)

    int32_t         in_min;  // Minimum value of original input (-32768 for axes)
    int32_t         in_max;  // Maximum value of original input (32767 for axes)

    int32_t         out_min; // SBUS channel min value (often 200, or 1024 if inverted center)
    int32_t         out_max; // SBUS channel max value (often 1800)
    
    int32_t         deadzone;// (Optional extension) Deadzone for center
} SbusMappingConfig_t;


/* --- Helper Macros for extremely clean configuration in app_mapper.c --- */

/**
 * @brief Map a gamepad axis to an SBUS channel
 * @param axis GamepadAxis_t (e.g., GAMEPAD_AXIS_LX)
 * @param _out_min The expected SBUS value when axis is at minimum (-32768)
 * @param _out_max The expected SBUS value when axis is at maximum (32767)
 *                 Tip: swap out_min and out_max to invert the axis!
 */
#define MAP_AXIS(axis, _out_min, _out_max) \
    { MAP_SRC_AXIS, (axis), -32768, 32767, (_out_min), (_out_max), 0 }

/**
 * @brief Map a gamepad button to an SBUS channel
 * @param btn GamepadButton_t (e.g., GAMEPAD_BTN_A)
 * @param _val_pressed The SBUS value when button is pressed (e.g., 1800)
 * @param _val_released The SBUS value when button is released (e.g., 200)
 */
#define MAP_BUTTON(btn, _val_pressed, _val_released) \
    { MAP_SRC_BTN, (btn), 0, 1, (_val_pressed), (_val_released), 0 }

/**
 * @brief Do not map anything to this SBUS channel (outputs default 1024)
 */
#define MAP_NONE() \
    { MAP_SRC_NONE, 0, 0, 0, 1024, 1024, 0 }
/* ==============================================================================
 *                              PUBLIC API
 * ==============================================================================*/

typedef enum {
    MAP_MODE_STANDARD_SBUS = 0,
    MAP_MODE_DJI_RC
} MapperMode_t;

extern MapperMode_t g_current_mode;

/**
 * @brief Updates the centralized Gamepad State. Call this in your USB callback.
 */
void Mapper_UpdateState(const GamepadState_t *newState);

/**
 * @brief Unified interface to get SBUS channels based on g_current_mode.
 */
void Mapper_GetSbusByMode(uint16_t *channels);

/**
 * @brief Calculates the 16 SBUS channels based on current state and mapping config.
 * @param channels Array of 16 uint16_t to hold the generated SBUS data.
 */
void Mapper_GetSbusChannels(uint16_t *channels);

/**
 * @brief Calculates the 16 UART channels based on current state and uart mapping config.
 * @param channels Array of 16 uint16_t to hold the generated UART data.
 */
void Mapper_GetUartChannels(uint16_t *channels);

/**
 * @brief Calculates the 16 DJI RC channels based on current state and DJI RC mapping config.
 * @param channels Array of 16 uint16_t to hold the generated DJI RC data.
 */
void Mapper_GetDjiRcChannels(uint16_t *channels);

#endif /* __APP_MAPPER_H */
