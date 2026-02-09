/*
 * bootloader_metadata.c
 *
 *  Created on: Jan 15, 2026
 *      Author: Fatih
 *
 * Independent metadata + slot decision module
 * Journal-based (append-only) metadata in flash:
 *   0x083C0000 - 0x083FFFFF (256KB, 32 pages @ 8KB)
 *
 * NOTE:
 * - This module DOES NOT include bootloader_driver.h
 * - It uses HAL flash erase/program (main.h dependency is OK for HAL).
 * - CRC32 is implemented in software to avoid CRC peripheral dependency.
 */

#include "bootloader_metadata.h"
#include "bootloader_driver.h"
#include "main.h"
#include <string.h>

/* =========================================================
 * Public API
 * ========================================================= */
void Meta_Init(meta_record_t *meta)
{
    meta_record_t flash_meta;
    bool meta_valid = true;

    if (meta == NULL)
    {
        return;
    }

    /* 1- Flash’tan metadata oku */
    Meta_Read(&flash_meta);

    /* 2- Tümü 0xFF mi? (ilk kurulum) */
    if (Meta_IsEmpty(&flash_meta))
    {
        Meta_Init_FromSlots(meta);
        return;
    }

    /* 3- Magic kontrolü */
    if (flash_meta.magic != META_MAGIC)
    {
        meta_valid = false;
    }

    /* 4- Seq kontrolü */
    if ((flash_meta.seq == 0U) || (flash_meta.seq == 0xFFFFFFFFU))
    {
        meta_valid = false;
    }

    meta_record_t tmp = flash_meta;
    tmp.crc = 0U;

    /* 5- CRC kontrolü */
    uint32_t calc_crc = CRC32_Calculate(
        ((uint8_t *)&tmp) + sizeof(uint32_t),
        sizeof(meta_record_t) - sizeof(uint32_t)
    );

    if (calc_crc != flash_meta.crc)
    {
        meta_valid = false;
    }

    /* 6- Metadata geçerli mi? */
    if (meta_valid == true)
    {
        *meta = flash_meta;

        /* Aktif slot gerçekten dolu mu? */
        if (!Slot_IsValid(Meta_SlotToBaseAddr(meta->active_slot)))
        {
            if (Slot_IsValid(Meta_SlotToBaseAddr(meta->target_slot)))
            {
                meta_slot_t t = meta->active_slot;
                meta->active_slot = meta->target_slot;
                meta->target_slot = t;
            }
            else
            {
                Meta_Init_FromSlots(meta);
            }
        }

        return;
    }

    /* ❌ Metadata bozuk → slot bazlı yeniden kur */
    Meta_Init_FromSlots(meta);
}

uint32_t Meta_CalcCrc_NoSelf(const meta_record_t *m)
{
    meta_record_t tmp = *m;
    tmp.crc = 0U;  /* CRC alanı hesaplamaya dahil olmasın */

    return CRC32_Calculate(
        ((const uint8_t *)&tmp) + sizeof(uint32_t),     /* magic hariç */
        sizeof(meta_record_t) - sizeof(uint32_t)
    );
}


bool Meta_IsEmpty(const meta_record_t *meta)
{
    const uint32_t *p = (const uint32_t *)meta;

    for (uint32_t i = 0; i < (sizeof(meta_record_t) / 4U); i++)
    {
        if (p[i] != 0xFFFFFFFFU)
        {
            return false;
        }
    }
    return true;
}

bool Meta_Read(meta_record_t *meta)
{
    uint32_t calculated_crc;

    if (meta == NULL)
    {
        return false;
    }

    /* -------------------------------------------------
     * Read raw metadata from flash
     * ------------------------------------------------- */
    Flash_Read(
        META_FLASH_ADDR,
        (uint8_t *)meta,
        sizeof(meta_record_t)
    );

    /* -------------------------------------------------
     * Check magic
     * ------------------------------------------------- */
    if (meta->magic != META_MAGIC)
    {
        return false;
    }

    /* -------------------------------------------------
     * CRC check (exclude crc field itself)
     * ------------------------------------------------- */
    calculated_crc = Meta_CalcCrc_NoSelf(meta);

    if (calculated_crc != meta->crc)
    {
        return false;
    }

    /* -------------------------------------------------
     * Slot sanity check
     * ------------------------------------------------- */
    if (meta->active_slot != META_SLOT_A &&
        meta->active_slot != META_SLOT_B)
    {
        return false;
    }

    if (meta->target_slot != META_SLOT_A &&
        meta->target_slot != META_SLOT_B)
    {
        return false;
    }

    if (meta->active_slot == meta->target_slot)
    {
        return false;
    }

    /* -------------------------------------------------
     * Update state sanity check
     * ------------------------------------------------- */
    switch (meta->update_state)
    {
        case META_UPDATE_IDLE:
        case META_UPDATE_IN_PROGRESS:
        //case META_UPDATE_DONE:
        case META_UPDATE_NO_APP:
            break;

        default:
            return false;
    }

    return true;
}

bool Meta_Write(meta_record_t *meta)
{
    if (meta == NULL)
    {
        return false;
    }

    /* -------------------------------------------------
     * MAGIC kontrolü
     * ------------------------------------------------- */
    if (meta->magic != META_MAGIC)
    {
        return false;
    }

    /* -------------------------------------------------
     * CRC yeniden hesapla
     *  - crc alanı hariç tutulur
     * ------------------------------------------------- */
    meta->crc = 0U;
    meta->crc = CRC32_Calculate(((uint8_t *)meta)+4, sizeof(meta_record_t)-4);

    /* -------------------------------------------------
     * Flash erase
     * ------------------------------------------------- */
    if (Flash_Erase(META_FLASH_ADDR) != true)
    {
        return false;
    }

    /* -------------------------------------------------
     * Flash write
     * ------------------------------------------------- */
    if (Flash_Write(
            META_FLASH_ADDR,
            (uint8_t *)meta,
            sizeof(meta_record_t)
        ) != true)
    {
        return false;
    }

    return true;
}

void Meta_Init_FromSlots(meta_record_t *meta)
{
    memset(meta, 0, sizeof(meta_record_t));

    bool slotA_valid = Slot_IsValid(Meta_SlotToBaseAddr(META_SLOT_A));
    bool slotB_valid = Slot_IsValid(Meta_SlotToBaseAddr(META_SLOT_B));

    meta->magic = META_MAGIC;
    meta->seq   = 1U;

    if (slotA_valid)
    {
        meta->active_slot = META_SLOT_A;
        meta->target_slot = META_SLOT_B;
        meta->update_state = META_UPDATE_IDLE;
    }
    else if (slotB_valid)
    {
        meta->active_slot = META_SLOT_B;
        meta->target_slot = META_SLOT_A;
        meta->update_state = META_UPDATE_IDLE;
    }
    else
    {
        meta->active_slot = META_SLOT_NONE;
        meta->target_slot = META_SLOT_NONE;
        meta->update_state = META_UPDATE_NO_APP;
        return;
    }

    meta->crc = 0U;
    meta->crc = Meta_CalcCrc_NoSelf(meta);

    Flash_Erase(META_FLASH_ADDR);
    Flash_Write(META_FLASH_ADDR, (uint8_t *)meta, sizeof(meta_record_t));
}

/*
 * Slot validity check
 * - Slot base address boş değil mi
 * - İlk word (SP) makul SRAM aralığında mı
 * - Reset handler (PC) Flash adresinde mi
 */
bool Slot_IsValid(uint32_t slot_base_addr)
{
    if ((slot_base_addr < FLASH_BASE) || (slot_base_addr > 0x083FFFFFu))
    {
        return false;
    }

    uint32_t *vector = (uint32_t *)slot_base_addr;

    uint32_t initial_sp = vector[0];
    uint32_t reset_pc  = vector[1];

    /* Empty flash check */
    if ((initial_sp == 0xFFFFFFFFu) || (reset_pc == 0xFFFFFFFFu))
    {
        return false;
    }

    /* SRAM range check (STM32U5A5) */
    if ((initial_sp < 0x20000000u) || (initial_sp > 0x203FFFFFu))
    {
        return false;
    }

    /* Thumb bit must be set */
    if ((reset_pc & 1u) == 0u)
    {
        return false;
    }

    uint32_t pc = reset_pc & ~1u;

    /* Flash address check */
    if ((pc < FLASH_BASE) || (pc > 0x083FFFFFu))
    {
        return false;
    }

    return true;
}

uint32_t Meta_SlotToBaseAddr(meta_slot_t slot)
{
    switch (slot)
    {
        case META_SLOT_A:
            return SLOT_A_BASE_ADDR;  // 0x08040000
        case META_SLOT_B:
            return SLOT_B_BASE_ADDR;  // 0x08200000
        default:
            return 0x00000000;
    }
}

