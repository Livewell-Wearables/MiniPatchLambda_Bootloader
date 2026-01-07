/*
 * USB_General.c
 *
 *  Created on: Agust 06, 2025
 *      Author: Inanc Mert Altun
 */

#include "USB_General.h"

USBCommParameters_t USB_Comm_Parameters;

void USB_Comm_Initialization(void)
{
    memset(&USB_Comm_Parameters, 0 , sizeof(USBCommParameters_t));

}
