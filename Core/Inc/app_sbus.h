#ifndef APP_SBUS_H
#define APP_SBUS_H

#include <stdint.h>
#include <string.h>

/* SBUS frame size is 25 bytes
 * [0] = 0x0F
 * [1..22] = 16 channels, 11 bits each
 * [23] = Flags (ch17, ch18, frame lost, failsafe)
 * [24] = 0x00
 */

static inline void sbus_build_frame(uint8_t *buf, uint16_t *channels) {
    memset(buf, 0, 25);
    buf[0] = 0x0F;
    
    // Pack 16 channels into 22 bytes
    buf[1] = (uint8_t)(channels[0] & 0x07FF);
    buf[2] = (uint8_t)((channels[0] & 0x07FF) >> 8 | (channels[1] & 0x07FF) << 3);
    buf[3] = (uint8_t)((channels[1] & 0x07FF) >> 5 | (channels[2] & 0x07FF) << 6);
    buf[4] = (uint8_t)((channels[2] & 0x07FF) >> 2);
    buf[5] = (uint8_t)((channels[2] & 0x07FF) >> 10 | (channels[3] & 0x07FF) << 1);
    buf[6] = (uint8_t)((channels[3] & 0x07FF) >> 7 | (channels[4] & 0x07FF) << 4);
    buf[7] = (uint8_t)((channels[4] & 0x07FF) >> 4 | (channels[5] & 0x07FF) << 7);
    buf[8] = (uint8_t)((channels[5] & 0x07FF) >> 1);
    buf[9] = (uint8_t)((channels[5] & 0x07FF) >> 9 | (channels[6] & 0x07FF) << 2);
    buf[10] = (uint8_t)((channels[6] & 0x07FF) >> 6 | (channels[7] & 0x07FF) << 5);
    buf[11] = (uint8_t)((channels[7] & 0x07FF) >> 3);
    buf[12] = (uint8_t)((channels[8] & 0x07FF));
    buf[13] = (uint8_t)((channels[8] & 0x07FF) >> 8 | (channels[9] & 0x07FF) << 3);
    buf[14] = (uint8_t)((channels[9] & 0x07FF) >> 5 | (channels[10] & 0x07FF) << 6);
    buf[15] = (uint8_t)((channels[10] & 0x07FF) >> 2);
    buf[16] = (uint8_t)((channels[10] & 0x07FF) >> 10 | (channels[11] & 0x07FF) << 1);
    buf[17] = (uint8_t)((channels[11] & 0x07FF) >> 7 | (channels[12] & 0x07FF) << 4);
    buf[18] = (uint8_t)((channels[12] & 0x07FF) >> 4 | (channels[13] & 0x07FF) << 7);
    buf[19] = (uint8_t)((channels[13] & 0x07FF) >> 1);
    buf[20] = (uint8_t)((channels[13] & 0x07FF) >> 9 | (channels[14] & 0x07FF) << 2);
    buf[21] = (uint8_t)((channels[14] & 0x07FF) >> 6 | (channels[15] & 0x07FF) << 5);
    buf[22] = (uint8_t)((channels[15] & 0x07FF) >> 3);
    
    buf[23] = 0x00;
    buf[24] = 0x00;
}

#endif
