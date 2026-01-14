/*
 * USB_Receive.h
 *
 *  Created on: Agust 06, 2025
 *      Author: Inanc Mert Altun
 */

#ifndef LW_USB_RECEIVE_H_
#define LW_USB_RECEIVE_H_

#include "main.h"
#include <stdint.h>
#include "USB_General.h"

#define USB_RX_DEBUG_BUF_SIZE 2048

typedef struct
{
    uint32_t rx_callback_count;      /* USB_RXCallback kaç kez çağrıldı */
    uint32_t total_received_bytes;   /* Frame boyunca toplanan byte sayısı */
    uint16_t expected_frame_len;     /* Hesaplanan toplam frame uzunluğu */
    uint16_t last_rx_len;             /* Son USB paketinin uzunluğu */
    uint8_t  frame_in_progress;      /* 1 = header yakalandı, frame toplanıyor */
    uint8_t  frame_completed;        /* 1 = frame tamamlandı */
    uint8_t  data_len_zero;           /* 1 = DataLen == 0 olan frame */
    uint8_t  overflow_error;          /* Buffer overflow oldu mu */
    uint8_t  footer_error;            /* Footer hatası oldu mu */
    uint8_t  raw_bytes[USB_RX_DEBUG_BUF_SIZE];
    uint16_t raw_len;
} USB_RxDebug_t;

void System_USB_Communication_Receive_Function(USBCommParameters_t *USB_Comm_ParametersLocal);
void USB_RXCallback(uint8_t *buf, uint32_t *len);


#endif /* LW_USB_RECEIVE_H_ */
