/*
 * flash_driver.h
 *
 *  Created on: Jan 15, 2026
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_FLASH_DRIVER_INC_FLASH_DRIVER_H_
#define BOOTLOADER_DRIVERS_FLASH_DRIVER_INC_FLASH_DRIVER_H_

#include "main.h"

void Flash_Read(uint32_t flash_addr, void *dst, uint32_t len);
bool Flash_Erase(uint32_t address);
bool Flash_Write(uint32_t address, const uint8_t *data, uint32_t length);

#endif /* BOOTLOADER_DRIVERS_FLASH_DRIVER_INC_FLASH_DRIVER_H_ */
