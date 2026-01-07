/*
 * bootloader_sram.c
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */
#include "bootloader_sram.h"

/**
 * @brief Check update request flag stored in RTC backup register
 */
bool BL_RTCBackup_IsUpdateRequested(RTC_HandleTypeDef *hrtc)
{
    uint32_t value;

    if (hrtc == NULL)
    {
        return false;
    }

    value = HAL_RTCEx_BKUPRead(hrtc, RTC_BKP_DR10);

    if (value == BL_UPDATE_MAGIC)
    {
        return true;
    }

    return false;
}

/**
 * @brief Set update request flag into RTC backup register
 */
void BL_RTCBackup_SetUpdateRequest(RTC_HandleTypeDef *hrtc)
{
    if (hrtc == NULL)
    {
        return;
    }

    HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR10, BL_UPDATE_MAGIC);
}

/**
 * @brief Clear update request flag from RTC backup register
 */
void BL_RTCBackup_ClearUpdateRequest(RTC_HandleTypeDef *hrtc)
{
    if (hrtc == NULL)
    {
        return;
    }

    HAL_RTCEx_BKUPWrite(hrtc, RTC_BKP_DR10, 0U);
}
