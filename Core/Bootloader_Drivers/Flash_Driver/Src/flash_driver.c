/*
 * flash_driver.c
 *
 *  Created on: Jan 15, 2026
 *      Author: Fatih
 */

#include <string.h>
#include "bootloader_driver.h"

void Flash_Read(uint32_t flash_addr, void *dst, uint32_t len)
{
    if ((dst == NULL) || (len == 0U))
    {
        return;
    }

    const uint8_t *src = (const uint8_t *)flash_addr;

    memcpy(dst, src, len);
}

bool Flash_Erase(uint32_t address)
{
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error = 0U;

    /* STM32U5:
     * Page size = 8 KB
     * Page index = (Address - FLASH_BASE) / FLASH_PAGE_SIZE
     * Metadata region is in BANK2 (0x083C0000 ...)
     */

    uint32_t page = (address - FLASH_BASE) / FLASH_PAGE_SIZE;

    HAL_FLASH_Unlock();

    erase_init.TypeErase = FLASH_TYPEERASE_PAGES;
    erase_init.Banks     = FLASH_BANK_2;   /* 0x083C0000 -> Bank 2 */
    erase_init.Page      = page;
    erase_init.NbPages   = 1;

    if (HAL_FLASHEx_Erase(&erase_init, &page_error) != HAL_OK)
    {
        HAL_FLASH_Lock();
        return false;
    }

    HAL_FLASH_Lock();
    return true;
}

bool Flash_Write(uint32_t address, const uint8_t *data, uint32_t length)
{
    uint32_t write_addr = address;
    uint32_t offset = 0U;

    /* STM32U5: address must be 16-byte aligned */
    if ((write_addr % 16U) != 0U)
    {
        return false;
    }

    HAL_FLASH_Unlock();

    while (offset < length)
    {
        uint8_t quad_buf[16];
        memset(quad_buf, 0xFF, sizeof(quad_buf));

        uint32_t chunk = length - offset;
        if (chunk > 16U)
        {
            chunk = 16U;
        }

        memcpy(quad_buf, &data[offset], chunk);

        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_QUADWORD,
                              write_addr,
                              (uint32_t)quad_buf) != HAL_OK)
        {
            HAL_FLASH_Lock();
            return false;
        }

        write_addr += 16U;
        offset     += chunk;
    }

    HAL_FLASH_Lock();
    return true;
}


