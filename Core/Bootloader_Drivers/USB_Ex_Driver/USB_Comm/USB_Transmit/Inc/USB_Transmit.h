/*
 * USB_Transmit.h
 *
 *  Created on: Agust 06, 2025
 *      Author: Inanc Mert Altun
 */

#ifndef LW_USB_TRANSMIT_H_
#define LW_USB_TRANSMIT_H_

#include "main.h"
#include <stdint.h>
#include <string.h>
#include "USB_General.h"
#include "usbd_cdc_if.h"


uint8_t USB_Transmit(uint8_t* Buf, uint16_t len);
USBTxParameters_t* USB_Prepare_Transmit_Buffer(uint8_t packet_type, uint8_t command, uint8_t status_code, uint16_t data_len, uint8_t* data);


#endif /* LW_USB_TRANSMIT_H_ */
