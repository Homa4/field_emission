#include "usbd_cdc_if.h"
#include <string.h>

#define APP_RX_DATA_SIZE  64
#define APP_TX_DATA_SIZE  64

uint8_t UserRxBufferFS[APP_RX_DATA_SIZE];
uint8_t UserTxBufferFS[APP_TX_DATA_SIZE];

/* ????? ??? ??????? ?????? ? ?? */
uint8_t  rx_buf[64];
uint32_t rx_len   = 0;
uint8_t  rx_ready = 0;

extern USBD_HandleTypeDef hUsbDeviceFS;

static int8_t CDC_Init_FS(void);
static int8_t CDC_DeInit_FS(void);
static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length);
static int8_t CDC_Receive_FS(uint8_t *pbuf, uint32_t *Len);

USBD_CDC_ItfTypeDef USBD_Interface_fops_FS = {
    CDC_Init_FS,
    CDC_DeInit_FS,
    CDC_Control_FS,
    CDC_Receive_FS
};

static int8_t CDC_Init_FS(void)
{
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, 0);
    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, UserRxBufferFS);
    return USBD_OK;
}

static int8_t CDC_DeInit_FS(void)
{
    return USBD_OK;
}

static int8_t CDC_Control_FS(uint8_t cmd, uint8_t *pbuf, uint16_t length)
{
    switch (cmd) {
        case CDC_SET_LINE_CODING:   break;
        case CDC_GET_LINE_CODING:   break;
        case CDC_SET_CONTROL_LINE_STATE: break;
        default: break;
    }
    return USBD_OK;
}

/* ???????????? ??????????? ???? ??????? ???? ? ?? */
static int8_t CDC_Receive_FS(uint8_t *Buf, uint32_t *Len)
{
    memcpy(rx_buf, Buf, *Len);
    rx_len   = *Len;
    rx_ready = 1;  /* ????????? — ???? ???? ?????? */

    USBD_CDC_SetRxBuffer(&hUsbDeviceFS, &Buf[0]);
    USBD_CDC_ReceivePacket(&hUsbDeviceFS);
    return USBD_OK;
}

/* ????????? ????? ?? ?? */
uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len)
{
    USBD_CDC_HandleTypeDef *hcdc =
        (USBD_CDC_HandleTypeDef *)hUsbDeviceFS.pClassData;

    if (hcdc->TxState != 0) return USBD_BUSY;

    memcpy(UserTxBufferFS, Buf, Len);
    USBD_CDC_SetTxBuffer(&hUsbDeviceFS, UserTxBufferFS, Len);
    return USBD_CDC_TransmitPacket(&hUsbDeviceFS);
}