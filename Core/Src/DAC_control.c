#include "dac_control.h"
extern I2C_HandleTypeDef hi2c2;

void DAC_SetVoltage(uint16_t voltage) {
    uint8_t dac_buf[3];
    const uint8_t MCP4725_ADDR = 0x60 << 1;

    dac_buf[0] = 0x40;
    dac_buf[1] = (uint8_t)(voltage >> 4);
    dac_buf[2] = (uint8_t)(voltage << 4);

    HAL_I2C_Master_Transmit(&hi2c2, MCP4725_ADDR, dac_buf, 3, 10);
}