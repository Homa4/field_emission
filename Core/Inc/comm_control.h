#ifndef COMM_CONTROL_H
#define COMM_CONTROL_H

#include "stm32f1xx_hal.h"
#include "cmsis_os.h"
#include <stdint.h>

/* ? ??? ???? ??? ???? ??? */
typedef enum {
    MODE_IDLE = 0,
    MODE_SCAN = 1,
} ScanMode;

extern volatile ScanMode current_mode;

#define CMD_SET_START_VOLTAGE    0x01
#define CMD_SET_END_VOLTAGE      0x02
#define CMD_SET_MAX_CURRENT      0x03
#define CMD_SET_STEP             0x04
#define CMD_SET_STEP_DELAY       0x05
#define CMD_SET_NUM_POINTS       0x06
#define CMD_SET_NUM_POINTS_DELAY 0x07
#define CMD_SET_NPCC             0x08
#define CMD_STOP                 0x09

void COMM_SendPacket(int16_t channels[4]);
void COMM_ReceiveCommand(void);

#endif