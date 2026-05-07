#include "ADC_control.h"
#include "usbd_cdc_if.h" 
/* ??? ?? ???? ??? ??????? ??? PGA */
static const float PGA_LSB_UV[] = {
    187.5f,   /* ±6.144? */
    125.0f,   /* ±4.096? */
     62.5f,   /* ±2.048? */
     31.25f,  /* ±1.024? */
     15.625f, /* ±0.512? */
      7.8125f /* ±0.256? */
};

/* ?????????? ????????? ????????? ? ????? ?????? */
ADC_Handle hadc_ext;

/* -------------------------------------------------- */
/*  ?????????????                                      */
/* -------------------------------------------------- */
void ADC_Init(ADC_Handle *h,
              I2C_HandleTypeDef *hi2c,
              ADS1115_PGA pga,
              ADS1115_SPS sps)
{
    h->hi2c        = hi2c;
    /* h->huart = huart; ? ??????? */
    h->pga         = pga;
    h->sps         = sps;
    h->initialized = 1;

    for (uint8_t i = 0; i < 4; i++) {
        h->channels[i] = 0;
        h->voltage[i]  = 0.0f;
    }
}

/* -------------------------------------------------- */
/*  ?????????: ???????? ??????? ??? SPS               */
/* -------------------------------------------------- */
static void ADC_WaitConversion(ADC_Handle *h)
{
    uint32_t delay_ms;
    switch (h->sps) {
        case ADS1115_SPS_8:   delay_ms = 130; break;
        case ADS1115_SPS_16:  delay_ms =  70; break;
        case ADS1115_SPS_32:  delay_ms =  40; break;
        case ADS1115_SPS_64:  delay_ms =  20; break;
        case ADS1115_SPS_128: delay_ms =  10; break;
        case ADS1115_SPS_250: delay_ms =   6; break;
        case ADS1115_SPS_475: delay_ms =   4; break;
        case ADS1115_SPS_860: delay_ms =   3; break;
        default:              delay_ms =  10; break;
    }
    osDelay(delay_ms);  /* ?? ?????? ???? ?????? FreeRTOS */
}

/* -------------------------------------------------- */
/*  ??????? ?????? ??????                             */
/* -------------------------------------------------- */
int16_t ADC_ReadChannel(ADC_Handle *h, uint8_t channel)
{
    if (!h->initialized || channel > 3) return 0;

    uint8_t  cfg_buf[3];
    uint16_t cfg = 0x8000                          /* OS: ????? ????????????  */
                 | ((uint16_t)(channel + 4) << 12) /* MUX: AIN0=4 .. AIN3=7  */
                 | (uint16_t)h->pga                /* PGA                     */
                 | 0x0100                          /* MODE: single-shot       */
                 | (uint16_t)h->sps                /* DR: ???????             */
                 | 0x0003;                         /* COMP: ????????          */

    cfg_buf[0] = ADS1115_REG_CONFIG;
    cfg_buf[1] = (cfg >> 8) & 0xFF;
    cfg_buf[2] =  cfg       & 0xFF;

    /* ????????? ??????-??????? */
    if (HAL_I2C_Master_Transmit(h->hi2c, ADS1115_ADDR,
                                 cfg_buf, 3, 100) != HAL_OK) {
        return 0;
    }

    /* ??????? ?????????? ???????????? */
    ADC_WaitConversion(h);

    /* ???????? ??????? ?????????? */
    uint8_t reg = ADS1115_REG_CONVERT;
    if (HAL_I2C_Master_Transmit(h->hi2c, ADS1115_ADDR,
                                 &reg, 1, 100) != HAL_OK) {
        return 0;
    }

    /* ??????? 2 ????? ?????????? */
    uint8_t raw[2] = {0, 0};
    if (HAL_I2C_Master_Receive(h->hi2c, ADS1115_ADDR,
                                raw, 2, 100) != HAL_OK) {
        return 0;
    }

    return (int16_t)((raw[0] << 8) | raw[1]);
}

/* -------------------------------------------------- */
/*  ??????? ???? 4 ???????                            */
/* -------------------------------------------------- */
void ADC_ReadAllChannels(ADC_Handle *h)
{
    for (uint8_t i = 0; i < 4; i++) {
        h->channels[i] = ADC_ReadChannel(h, i);
        h->voltage[i]  = ADC_ToVoltage(h, h->channels[i]);
    }
}

/* -------------------------------------------------- */
/*  ???????????? raw ? ??????                         */
/* -------------------------------------------------- */
float ADC_ToVoltage(ADC_Handle *h, int16_t raw)
{
    uint8_t idx = ((uint16_t)h->pga >> 9) & 0x07;
    if (idx > 5) idx = 5;
    return (float)raw * PGA_LSB_UV[idx] / 1000000.0f;
}

/* -------------------------------------------------- */
/*  ????????? ?????? ?? UART                          */
/*  ??????: [0xAA][0xBB][ch0H][ch0L]...[ch3H][ch3L]  */
/* -------------------------------------------------- */

