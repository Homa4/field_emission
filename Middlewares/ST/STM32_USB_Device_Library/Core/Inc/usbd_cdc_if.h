#ifndef USBD_CDC_IF_H
#define USBD_CDC_IF_H

#include "usbd_cdc.h"

extern USBD_CDC_ItfTypeDef USBD_Interface_fops_FS;

/* ????? ??????? — ????????? ? comm_control.c */
extern uint8_t  rx_buf[];
extern uint32_t rx_len;
extern uint8_t  rx_ready;

uint8_t CDC_Transmit_FS(uint8_t *Buf, uint16_t Len);

#endif