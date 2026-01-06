/*
 * bootloader_sram.c
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */
#include "bootloader_sram.h"

/**
 * @brief Backup SRAM'den context okur
 */
bool BL_Backup_Read(bl_backup_ctx_t *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }

    const bl_backup_ctx_t *bk =
        (const bl_backup_ctx_t *)BL_BK_BASE_ADDR;

    if (bk->magic != BL_BK_MAGIC)
    {
        return false;
    }

    *ctx = *bk;
    return true;
}

/**
 * @brief Backup SRAM'e context yazar
 */
bool BL_Backup_Write(const bl_backup_ctx_t *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }

    bl_backup_ctx_t *bk =
        (bl_backup_ctx_t *)BL_BK_BASE_ADDR;

    *bk = *ctx;
    return true;
}

/**
 * @brief Backup SRAM içeriğini sıfırlar
 */
void BL_Backup_Clear(void)
{
    bl_backup_ctx_t *bk =
        (bl_backup_ctx_t *)BL_BK_BASE_ADDR;

    bk->magic = 0U;
    bk->state = BL_BK_STATE_NONE;
    bk->last_error = 0U;
}


