/*
 * crc.h
 *
 *  Created on: Jan 9, 2026
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_CRC_INC_CRC_H_
#define BOOTLOADER_DRIVERS_CRC_INC_CRC_H_

#include <stdint.h>
#include <stddef.h>

uint32_t CRC32_Calculate(const uint8_t *data, uint32_t length);
uint8_t CRC32_Verify(const uint8_t *data,
                     uint32_t data_len,
                     uint32_t received_crc);

#endif /* BOOTLOADER_DRIVERS_CRC_INC_CRC_H_ */
