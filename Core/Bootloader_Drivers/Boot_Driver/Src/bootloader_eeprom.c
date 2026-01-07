/*
 * bootloader_eeprom.c
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */

#include "bootloader_eeprom.h"

/**
 * @brief EEPROM'dan boot metadata okur ve doğrular
 */
bool BL_EEPROM_Read(S_AT24C32_t *at24c32, bl_eeprom_meta_t *meta)
{
    if (meta == NULL)
    {
        return false;
    }

    /* EEPROM'dan ham veri oku */
    if(AT24C32_ReadU32(at24c32, (uint16_t)EEPROM_DEVICE_UPDATE_FLAG_ADDRESS, &meta->magic) != HAL_OK)
    {
        return false;
    }

    /* Magic number kontrolü */
    if (meta->magic != BL_EE_MAGIC)
    {
    	meta->update_flag = BL_EE_FLAG_NONE;
        return false;
    }

    meta->update_flag = BL_EE_FLAG_UPDATE_REQUEST;

    return true;
}

/**
 * @brief EEPROM'a boot metadata yazar
 */
HAL_StatusTypeDef BL_EEPROM_ClearUpdateFlag(S_AT24C32_t *at24c32)
{
    return AT24C32_WriteU32(at24c32, EEPROM_DEVICE_UPDATE_FLAG_ADDRESS, 0xFFFFFFFF);
}


