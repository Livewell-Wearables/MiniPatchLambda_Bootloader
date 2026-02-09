/*
 * bootloader_metadata.h
 *
 *  Created on: Jan 15, 2026
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_METADATA_DRIVER_INC_BOOTLOADER_METADATA_H_
#define BOOTLOADER_DRIVERS_METADATA_DRIVER_INC_BOOTLOADER_METADATA_H_

#include <stdint.h>
#include <stdbool.h>
#include "flash_driver.h"
#include "crc.h"

#ifdef __cplusplus
extern "C" {
#endif

/* =========================================================
 * Flash region (fixed by memory map)
 * ========================================================= */
#define BL_META_BASE_ADDR     	0x083C0000UL
#define META_FLASH_ADDR			BL_META_BASE_ADDR
#define BL_META_SIZE_BYTES   	(256UL * 1024UL)
#define BL_META_PAGE_SIZE    	(8UL * 1024UL)

#define META_MAGIC   (0x4D455441UL)   /* 'META' ASCII */

/* =========================================================
 * Slot abstraction (driver'dan bağımsız)
 * ========================================================= */
typedef enum
{
    META_SLOT_NONE = 0,
    META_SLOT_A    = 1,
    META_SLOT_B    = 2
} meta_slot_t;

/* =========================================================
 * Commit / Update phases
 * ========================================================= */
typedef enum
{
    META_UPDATE_IDLE = 0,      /* Güncelleme yok, normal çalış */
    META_UPDATE_IN_PROGRESS,   /* Update devam ediyor */
    META_UPDATE_SWAP_PENDING,  /* Reset sonrası slot swap */
    META_UPDATE_NO_APP         /* Hiç geçerli uygulama yok */
} meta_update_state_t;

/* =========================================================
 * Firmware info (generic)
 * ========================================================= */
typedef struct
{
    uint32_t size_bytes;
    uint32_t crc32;
    uint8_t  version_major;
    uint8_t  version_minor;
    uint8_t  version_patch;
} meta_fw_info_t;

/* =========================================================
 * Slot info
 * ========================================================= */
typedef struct
{
    uint8_t         valid;
    meta_fw_info_t  fw;
} meta_slot_info_t;

/* =========================================================
 * Persistent metadata record
 * ========================================================= */
typedef struct
{
    uint32_t magic;
    uint32_t seq;
    uint32_t crc;

    meta_slot_t     active_slot;
    meta_slot_t     target_slot;
    meta_update_state_t update_state;

    uint32_t        progress_bytes;

    meta_slot_info_t slotA;
    meta_slot_info_t slotB;
} meta_record_t;

/* =========================================================
 * PUBLIC API (Driver bundan başka hiçbir şey görmez)
 * ========================================================= */

/* Init & load */
void Meta_Init(meta_record_t *meta);

uint32_t Meta_CalcCrc_NoSelf(const meta_record_t *m);

/* ---- Slot / boot decisions ---- */
void Meta_Init_FromSlots(meta_record_t *meta);
bool Slot_IsValid(uint32_t slot_base_addr);

/* ---- Update lifecycle ---- */
bool Meta_Abort(void);

/* ---- Query helpers ---- */
bool Meta_IsEmpty(const meta_record_t *meta);

bool Meta_Read(meta_record_t *meta);
bool Meta_Write(meta_record_t *meta);

uint32_t Meta_SlotToBaseAddr(meta_slot_t slot);

#ifdef __cplusplus
}
#endif

#endif /* BOOTLOADER_DRIVERS_METADATA_DRIVER_INC_BOOTLOADER_METADATA_H_ */
