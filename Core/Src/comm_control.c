#include "comm_control.h"
#include "usbd_cdc_if.h"
#include <string.h>

extern float start_voltage;
extern float end_voltage;
extern float max_current;
extern float step;
extern float step_delay;
extern int   number_of_points_in_scan;
extern int   number_of_points_in_scan_delay;
extern int   npcc;
extern volatile ScanMode current_mode;  /* ? ????? */

void COMM_SendPacket(int16_t channels[4])
{
    uint8_t packet[10];
    packet[0] = 0xAA;
    packet[1] = 0xBB;

    for (uint8_t i = 0; i < 4; i++) {
        packet[2 + i * 2]     = (channels[i] >> 8) & 0xFF;
        packet[2 + i * 2 + 1] =  channels[i]       & 0xFF;
    }

    CDC_Transmit_FS(packet, 10);
}

void COMM_ReceiveCommand(void)
{
    if (!rx_ready) return;
    rx_ready = 0;

    /* ???????? ????? 7 ???? ?? ????? */
    if (rx_len != 7)       return;
    if (rx_buf[0] != 0xCC) return;
    if (rx_buf[1] != 0xDD) return;

    uint8_t cmd = rx_buf[2];
    float value;
    memcpy(&value, &rx_buf[3], 4);

    switch (cmd) {
        case CMD_SET_START_VOLTAGE: start_voltage = value; break;
        case CMD_SET_END_VOLTAGE:   end_voltage   = value; break;
        case CMD_SET_STEP:          step          = value; break;
        case CMD_SET_NPCC:          current_mode  = MODE_SCAN; break;
        case CMD_STOP:              current_mode  = MODE_IDLE; break;
        default: break;
    }
}