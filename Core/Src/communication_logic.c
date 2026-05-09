#include "communication_logic.h"
#include "main.h"

extern float start_voltage;
extern float end_voltage;
extern float step;
extern volatile uint8_t is_scanning;
extern volatile uint32_t step_delay; // ???????: uint32_t!

void COMM_ProcessPacket(uint8_t *packet) {
    if (packet[0] != 0xAA) return;

    switch (packet[1]) {
        case 0x01: start_voltage = (float)((packet[2] << 8) | packet[3]); break;
        case 0x02: step = (float)((packet[2] << 8) | packet[3]); break;
        case 0x03: is_scanning = packet[3]; break;
        case 0x04: end_voltage = (float)((packet[2] << 8) | packet[3]); break;
        case 0x05: step_delay = (uint32_t)((packet[2] << 8) | packet[3]); break;
        // ?????? HAL_Delay ???!
    }
}