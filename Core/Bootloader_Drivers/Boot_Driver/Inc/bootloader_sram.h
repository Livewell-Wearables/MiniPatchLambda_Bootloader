/*
 * bootloader_sram.h
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_SRAM_H_
#define BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_SRAM_H_

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

/* =========================================================
 * Backup SRAM Definitions
 * ========================================================= */

#define BL_BK_MAGIC              (0x55AA55AAUL)
#define BL_BK_BASE_ADDR          (0x38800000UL)  /* STM32U5 Backup SRAM */

/* =========================================================
 * Backup State Definitions
 * ========================================================= */

typedef enum
{
    BL_BK_STATE_NONE = 0U,             /* Normal reset */
    BL_BK_STATE_AFTER_UPDATE_RESET,    /* Application update sonrası reset */
    BL_BK_STATE_UPDATE_IN_PROGRESS     /* Bootloader update sürecinde */
} bl_backup_state_t;

/* =========================================================
 * Backup SRAM Context Structure
 * ========================================================= */

typedef struct
{
    uint32_t            magic;          /* Yapı geçerlilik kontrolü */
    bl_backup_state_t   state;          /* Reset / update durumu */
    uint32_t            last_error;     /* Son bootloader hata kodu */
} bl_backup_ctx_t;

/* =========================================================
 * Public API
 * ========================================================= */

bool BL_Backup_Read(bl_backup_ctx_t *ctx);
bool BL_Backup_Write(const bl_backup_ctx_t *ctx);
void BL_Backup_Clear(void);

#endif /* BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_SRAM_H_ */
