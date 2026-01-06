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
        return false;
    }

    return true;
}

/**
 * @brief EEPROM'a boot metadata yazar
 */
bool BL_EEPROM_Write(S_AT24C32_t *at24c32, const bl_eeprom_meta_t *meta)
{
    bl_eeprom_meta_t tmp;

    if (meta == NULL)
    {
        return false;
    }

    tmp = *meta;
    tmp.magic = BL_EE_MAGIC;

    /* CRC hesapla */
/*
    tmp.crc = CRC32_Calculate((uint8_t *)&tmp,
                              sizeof(bl_eeprom_meta_t) - sizeof(uint32_t));
*/
    /* EEPROM'a yaz */
    /*
    return EEPROM_Write(BL_EE_META_ADDR,
                         (const uint8_t *)&tmp,
                         sizeof(bl_eeprom_meta_t));
    */
}

/**
 * @brief EEPROM üzerindeki update isteğini temizler
 */
bool BL_EEPROM_ClearUpdateFlag(S_AT24C32_t *at24c32)
{
    bl_eeprom_meta_t meta;

    if (BL_EEPROM_Read(at24c32, &meta) != true)
    {
        return false;
    }

    meta.update_flag = BL_EE_FLAG_NONE;
    return BL_EEPROM_Write(at24c32, &meta);
}

