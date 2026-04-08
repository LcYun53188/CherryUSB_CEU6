#include "app_mapper.h"

/* ==============================================================================
 *                     USER CONFIGURATION: SBUS MAPPING TABLE
 * ==============================================================================
 * 
 * MAP_AXIS(axis_name, out_min, out_max)
 *      - Maps an analog stick to an SBUS channel. 
 *      - To INVERT the axis, simple swap out_min and out_max! (e.g., 1800, 200).
 * 
 * MAP_BUTTON(btn_name, val_pressed, val_released)
 *      - Maps a physical button to an SBUS channel.
 *      - e.g., MAP_BUTTON(GAMEPAD_BTN_A, 1800, 200) outputs 1800 when A is pressed.
 * 
 * MAP_NONE()
 *      - Unused channel, outputs default 1024.
 * ==============================================================================*/

const SbusMappingConfig_t sbus_map_config[16] = {
    // CH0: Left Stick X
    MAP_AXIS(GAMEPAD_AXIS_LX, 200, 1800),
    // CH1: Left Stick Y
    MAP_AXIS(GAMEPAD_AXIS_LY, 200, 1800),
    // CH2: Right Stick X
    MAP_AXIS(GAMEPAD_AXIS_RX, 200, 1800),
    // CH3: Right Stick Y
    MAP_AXIS(GAMEPAD_AXIS_RY, 200, 1800),
    
    // CH4: Button A
    MAP_BUTTON(GAMEPAD_BTN_A, 1800, 200),
    // CH5: Button B
    MAP_BUTTON(GAMEPAD_BTN_B, 1800, 200),
    // CH6: Button X
    MAP_BUTTON(GAMEPAD_BTN_X, 1800, 200),
    // CH7: Button Y
    MAP_BUTTON(GAMEPAD_BTN_Y, 1800, 200),

    // CH8: D-Pad Up
    MAP_BUTTON(GAMEPAD_BTN_DUP, 1800, 200),
    // CH9: D-Pad Down
    MAP_BUTTON(GAMEPAD_BTN_DDOWN, 1800, 200),

    // CH10~CH15: Unmapped
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE()
};

/* ==============================================================================
 *                     USER CONFIGURATION: UART MAPPING TABLE
 * ==============================================================================
 * Independent table for pure UART code structure output.
 * Here you can map to completely different scales (-30000~30000, 0~1 etc.)
 * ==============================================================================*/
const SbusMappingConfig_t uart_map_config[16] = {
    // CH0: Left Stick X
    MAP_AXIS(GAMEPAD_AXIS_LX, -30000, 30000),
    // CH1: Left Stick Y
    MAP_AXIS(GAMEPAD_AXIS_LY, -30000, 30000),
    // CH2: Right Stick X
    MAP_AXIS(GAMEPAD_AXIS_RX, -30000, 30000),
    // CH3: Right Stick Y
    MAP_AXIS(GAMEPAD_AXIS_RY, -30000, 30000),
    
    // CH4: Button A (Send 1 for pressed, 0 for released over UART instead of 1800/200)
    MAP_BUTTON(GAMEPAD_BTN_A, 1, 0),
    MAP_BUTTON(GAMEPAD_BTN_B, 1, 0),
    MAP_BUTTON(GAMEPAD_BTN_X, 1, 0),
    MAP_BUTTON(GAMEPAD_BTN_Y, 1, 0),

    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE()
};

/* ==============================================================================
 *                     USER CONFIGURATION: DJI RC MAPPING TABLE
 * ==============================================================================
 * Data format based on DJI DT7 SBUS / RoboMaster RC_ctrl_t protocol.
 * The center value is 1024, and the range is +/- 660 (364 ~ 1684).
 * CH0: Right Stick X (Roll)
 * CH1: Right Stick Y (Pitch)
 * CH2: Left Stick Y  (Throttle)
 * CH3: Left Stick X  (Yaw)
 * CH4: Dial/Wheel    (Mapped to RT)
 * ==============================================================================*/
const SbusMappingConfig_t dji_rc_map_config[16] = {
    // CH0: Right Stick X (Roll)
    MAP_AXIS(GAMEPAD_AXIS_RX, 364, 1684),
    // CH1: Right Stick Y (Pitch)
    MAP_AXIS(GAMEPAD_AXIS_RY, 364, 1684),
    // CH2: Left Stick Y (Throttle)
    MAP_AXIS(GAMEPAD_AXIS_LY, 364, 1684),
    // CH3: Left Stick X (Yaw)
    MAP_AXIS(GAMEPAD_AXIS_LX, 364, 1684),
    // CH4: Dial (Controlled via A,B,X,Y buttons in code)
    MAP_NONE(),

    // CH5~CH15: Unmapped
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE(),
    MAP_NONE()
};

/* ==============================================================================
 *                     INTERNAL STATE & ENGINE
 * ==============================================================================*/

MapperMode_t g_current_mode = MAP_MODE_DJI_RC;

static GamepadState_t g_gameState = {0};

/* DJI DT7 Switch Internal Status */
static uint8_t g_s0_base = RC_SW_DOWN; // Switch Left (s[0]) base: Down or Mid
static uint8_t g_s1_base = RC_SW_DOWN; // Switch Right (s[1]) base: Down or Mid
static bool g_last_lb = false;
static bool g_last_rb = false;

void Mapper_UpdateState(const GamepadState_t *newState) {
    if (newState) {
        // Toggle Logic for Left Switch (LB)
        if (newState->buttons[GAMEPAD_BTN_LB] && !g_last_lb) {
            // Rising Edge: Toggle between DOWN and MID
            g_s0_base = (g_s0_base == RC_SW_DOWN) ? RC_SW_MID : RC_SW_DOWN;
        }
        g_last_lb = newState->buttons[GAMEPAD_BTN_LB];

        // Toggle Logic for Right Switch (RB)
        if (newState->buttons[GAMEPAD_BTN_RB] && !g_last_rb) {
            // Rising Edge: Toggle between DOWN and MID
            g_s1_base = (g_s1_base == RC_SW_DOWN) ? RC_SW_MID : RC_SW_DOWN;
        }
        g_last_rb = newState->buttons[GAMEPAD_BTN_RB];

        g_gameState = *newState;
    }
}

static int32_t map_linear(int32_t x, int32_t in_min, int32_t in_max, int32_t out_min, int32_t out_max) {
    return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

static int32_t clamp(int32_t val, int32_t min_limit, int32_t max_limit) {
    if (val < min_limit) return min_limit;
    if (val > max_limit) return max_limit;
    return val;
}

static void compute_channels_from_config(const SbusMappingConfig_t *configs, uint16_t *channels) {
    if (!channels) return;

    for (int i = 0; i < 16; i++) {
        const SbusMappingConfig_t* cfg = &configs[i];
        int32_t result = 1024; // Default safe middle value for unmapped

        // If mapped to NONE but we want 0 for NONE in UART, we could handle it differently.
        // For simplicity, MAP_NONE() has out_min/out_max = 1024.
        // It's recommended to just use MAP_AXIS with 0,0 if 0 is wanted, or we just leave MAP_NONE as 1024.

        if (cfg->type == MAP_SRC_AXIS) {
            if (cfg->index < GAMEPAD_AXIS_MAX) {
                int32_t raw_val = g_gameState.axes[cfg->index];
                result = map_linear(raw_val, cfg->in_min, cfg->in_max, cfg->out_min, cfg->out_max);
                
                int32_t limit1 = cfg->out_min;
                int32_t limit2 = cfg->out_max;
                if (limit1 < limit2) result = clamp(result, limit1, limit2);
                else                 result = clamp(result, limit2, limit1);
            }
        } 
        else if (cfg->type == MAP_SRC_BTN) {
            if (cfg->index < GAMEPAD_BTN_MAX) {
                bool is_pressed = g_gameState.buttons[cfg->index];
                result = is_pressed ? cfg->out_min : cfg->out_max;
            }
        } else {
            // MAP_NONE logic
            result = cfg->out_min; // typically 1024
        }
        channels[i] = (uint16_t)result;
    }
}

void Mapper_GetSbusByMode(uint16_t *channels) {
    if (g_current_mode == MAP_MODE_DJI_RC) {
        Mapper_GetDjiRcChannels(channels);
    } else {
        Mapper_GetSbusChannels(channels);
    }
}

void Mapper_GetSbusChannels(uint16_t *channels) {
    compute_channels_from_config(sbus_map_config, channels);
}

void Mapper_GetUartChannels(uint16_t *channels) {
    compute_channels_from_config(uart_map_config, channels);
}

void Mapper_GetDjiRcChannels(uint16_t *channels) {
    if (!channels) return;
    
    // 1. Fill basic channels (0-4: sticks and dial) from config table
    compute_channels_from_config(dji_rc_map_config, channels);

    // 2. Dial (CH4) Logic from A, B, X, Y buttons
    // Center: 1024, Range: +/- 660. Half: +/- 330.
    uint16_t dial = 1024;
    if (g_gameState.buttons[GAMEPAD_BTN_B])      dial = 364;  // Full Down
    else if (g_gameState.buttons[GAMEPAD_BTN_A]) dial = 694;  // Half Down
    else if (g_gameState.buttons[GAMEPAD_BTN_Y]) dial = 1684; // Full Up
    else if (g_gameState.buttons[GAMEPAD_BTN_X]) dial = 1354; // Half Up
    channels[4] = dial;

    // 3. Calculate dynamic Switch S1/S2 states
    // Trigger Threshold: Axes are -32768 to 32767. Using 10000 as pressed.
    const int32_t TRIGGER_PRESS_THRESHOLD = 10000;

    // Switch Left (channels[5]) logic
    uint16_t s0 = g_s0_base;
    if (g_s0_base == RC_SW_MID) {
        if (g_gameState.axes[GAMEPAD_AXIS_LT] > TRIGGER_PRESS_THRESHOLD) {
            s0 = RC_SW_UP;
        }
    }
    channels[5] = s0;

    // Switch Right (channels[6]) logic
    uint16_t s1 = g_s1_base;
    if (g_s1_base == RC_SW_MID) {
        if (g_gameState.axes[GAMEPAD_AXIS_RT] > TRIGGER_PRESS_THRESHOLD) {
            s1 = RC_SW_UP;
        }
    }
    channels[6] = s1;
}

