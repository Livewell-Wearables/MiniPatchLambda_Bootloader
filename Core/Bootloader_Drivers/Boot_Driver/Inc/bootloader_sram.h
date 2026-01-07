/*
 * bootloader_sram.h
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_SRAM_H_
#define BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_SRAM_H_

#include <stdint.h>
#include <stdbool.h>
#include "stm32u5xx_hal.h"

/* =========================================================
 * RTC Backup Register Update Flag
 * ========================================================= */

#define BL_UPDATE_MAGIC   (0x55AA55AAUL)

/* =========================================================
 * Public API
 * ========================================================= */

/**
 * @brief Initialize RTC backup access (PWR + backup domain)
 *
 * This function must be called once before any
 * backup register read/write operation.
 *
 * @param[in] hrtc  RTC handle
 */
void BL_RTCBackup_Init(RTC_HandleTypeDef *hrtc);

/**
 * @brief Check whether update request is present
 *
 * @param[in] hrtc  RTC handle
 *
 * @return true  Update requested
 * @return false Normal boot
 */
bool BL_RTCBackup_IsUpdateRequested(RTC_HandleTypeDef *hrtc);

/**
 * @brief Set update request flag
 *
 * @param[in] hrtc  RTC handle
 */
void BL_RTCBackup_SetUpdateRequest(RTC_HandleTypeDef *hrtc);

/**
 * @brief Clear update request flag
 *
 * @param[in] hrtc  RTC handle
 */
void BL_RTCBackup_ClearUpdateRequest(RTC_HandleTypeDef *hrtc);

#endif /* BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_SRAM_H_ */
