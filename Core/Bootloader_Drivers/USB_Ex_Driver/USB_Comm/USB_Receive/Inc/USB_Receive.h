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

void System_USB_Communication_Receive_Function(void);
void USB_RXCallback(uint8_t *buf, uint32_t *len);


#endif /* LW_USB_RECEIVE_H_ */
