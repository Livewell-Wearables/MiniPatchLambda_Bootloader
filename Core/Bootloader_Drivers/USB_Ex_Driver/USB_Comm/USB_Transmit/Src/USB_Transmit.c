/*
 * USB_Transmit.c
 *
 *  Created on: Agust 06, 2025
 *      Author: Inanc Mert Altun
 */

#include "USB_Transmit.h"

extern USBCommParameters_t USB_Comm_Parameters;

uint8_t Calculate_Checksum(const uint8_t* buf, uint16_t len);


uint8_t USB_Transmit(uint8_t* Buf, uint16_t len)
{
    uint8_t usb_transmit_status = CDC_Transmit_HS(Buf, len);
    return usb_transmit_status;
}

USBTxParameters_t* USB_Prepare_Transmit_Buffer(uint8_t packet_type, uint8_t command, uint8_t status_code, uint16_t data_len, uint8_t* data)
{
    static USBTxParameters_t txPacket;
    uint16_t index = 0;
    

    txPacket.usbTxBuf[index++] = USB_PACKET_HEADER_1;
    txPacket.usbTxBuf[index++] = USB_PACKET_HEADER_2;
    txPacket.usbTxBuf[index++] = packet_type;
    txPacket.usbTxBuf[index++] = command;
    txPacket.usbTxBuf[index++] = status_code;       // flash için bir response'dir
    txPacket.usbTxBuf[index++] = data_len >> 8;
    txPacket.usbTxBuf[index++] = data_len & 0xFF;

    if (data != NULL && data_len > 0)
    {
        memcpy(&txPacket.usbTxBuf[index], data, data_len);
        index += data_len;
        
    }
    
    uint8_t checksum = Calculate_Checksum(&txPacket.usbTxBuf[USB_INDEX_3_PACKET_TYPE], USB_CONSTANT_PACKET_VALUES_FOR_CHECKSUM + data_len);
    txPacket.usbTxBuf[index++] = checksum;

    txPacket.usbTxBuf[index++] = USB_PACKET_FOOTER_1;
    txPacket.usbTxBuf[index++] = USB_PACKET_FOOTER_2;

    txPacket.usbTxBufLen = index;

    return &txPacket;
}

uint8_t Calculate_Checksum(const uint8_t* buf, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++)
    {
        sum ^= buf[i];
    }
    return sum;
}
