/*
 * bootloader_eeprom.h
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_EEPROM_H_
#define BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_EEPROM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <at24c32_driver.h>

/* =========================================================
 * EEPROM Layout Definitions
 * ========================================================= */

/* EEPROM içeriğinin geçerli olduğunu gösteren magic number */
#define BL_EE_MAGIC               (0xAA55AA55UL)

/* EEPROM adresleri (örnek – senin AT24Cxx haritana göre güncellersin) */
#define BL_EE_META_ADDR           (0x0000U)

/* =========================================================
 * EEPROM Update Flags
 * ========================================================= */

typedef enum
{
    BL_EE_FLAG_NONE = 0U,            /* Normal boot */
    BL_EE_FLAG_UPDATE_REQUEST = 1U   /* Application update talep etti */
} bl_ee_flag_t;

/* =========================================================
 * EEPROM Metadata Structure
 * ========================================================= */

typedef struct
{
    uint32_t        magic;            /* EEPROM geçerlilik kontrolü */
    uint32_t        device_version;   /* Cihazın mevcut firmware versiyonu */
    uint32_t        target_version;   /* Güncellenmek istenen versiyon */
    bl_ee_flag_t    update_flag;      /* Update isteği var mı? */
} bl_eeprom_meta_t;

/* =========================================================
 * Public API
 * ========================================================= */

/**
 * @brief EEPROM'dan boot metadata okur
 */
bool BL_EEPROM_Read(S_AT24C32_t *at24c32, bl_eeprom_meta_t *meta);

/**
 * @brief EEPROM update flag'ini temizler
 */
HAL_StatusTypeDef BL_EEPROM_ClearUpdateFlag(S_AT24C32_t *at24c32);

#endif /* BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_EEPROM_H_ */
