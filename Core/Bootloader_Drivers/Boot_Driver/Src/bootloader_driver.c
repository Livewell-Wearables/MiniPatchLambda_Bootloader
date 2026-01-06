/**
 * @file    bootloader_driver.c
 * @brief   Secure Bootloader control logic and application handover
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 *
 * This module implements the core bootloader state machine, application
 * validation and safe jump-to-application mechanism for STM32U5.
 *
 * MISRA-C:2012 aligned design.
 */

#include "bootloader_driver.h"
#include "stm32u5xx_hal.h"

extern S_AT24C32_t at24c32;

/* =========================================================
 * Local Type Definitions
 * ========================================================= */

/**
 * @brief Function pointer type for application entry point
 */
typedef void (*pFunction)(void);

/* =========================================================
 * Local Function Prototypes
 * ========================================================= */

/**
 * @brief Validate application vector table sanity
 *
 * @param[in] appBase  Application flash base address
 * @return true  Vector table looks valid
 * @return false Vector table invalid
 */
static bool BL_IsVectorTableSane(uint32_t appBase);

/**
 * @brief Perform low-level jump to application
 *
 * @param[in] appBase  Application flash base address
 */
static void BL_Jump(uint32_t appBase);

/**
 * @brief Check whether update mode is requested
 *
 * @param[in] ctx  Bootloader context pointer
 * @return true  Update requested
 * @return false Normal boot
 */
static bool BL_CheckUpdateRequest(BootloaderCtx_t *ctx);

/* =========================================================
 * Public Functions
 * ========================================================= */

/**
 * @brief Initialize bootloader context and internal state
 *
 * @param[in,out] ctx  Bootloader context structure
 */
void Bootloader_Init(BootloaderCtx_t *ctx)
{
    if (ctx == NULL)
    {
        return;
    }

    ctx->state              = BL_STATE_INIT;
    ctx->error              = BL_ERR_NONE;

    ctx->tick_start         = HAL_GetTick();
    ctx->boot_elapsed_ms    = 0U;

    ctx->update_requested   = false;
    ctx->update_in_progress = false;

    ctx->app_base           = BL_APP_BASE_ADDRESS;
    ctx->app_valid          = false;

    ctx->fw_size            = 0U;
    ctx->fw_received        = 0U;
    ctx->fw_crc_expected    = 0U;
    ctx->fw_crc_calculated  = 0U;

    ctx->last_event         = 0U;
    ctx->reset_reason       = RCC->CSR;

    ctx->state              = BL_STATE_CHECK_UPDATE;
}

/**
 * @brief Bootloader main state machine task
 *
 * This function shall be called periodically from main loop.
 *
 * @param[in,out] ctx  Bootloader context structure
 */
void Bootloader_Task(BootloaderCtx_t *ctx)
{
    if (ctx == NULL)
    {
        return;
    }

    ctx->boot_elapsed_ms = HAL_GetTick() - ctx->tick_start;

    switch (ctx->state)
    {
        case BL_STATE_CHECK_UPDATE:
        {
            ctx->update_requested = BL_CheckUpdateRequest(ctx);

            if (ctx->update_requested == true)
            {
                ctx->state = BL_STATE_UPDATE_MODE;
            }
            else
            {
                ctx->state = BL_STATE_WAIT;
            }
            break;
        }

        case BL_STATE_WAIT:
        {
            if (ctx->boot_elapsed_ms >= BL_BOOT_WINDOW_MS)
            {
                ctx->app_valid = BL_IsVectorTableSane(ctx->app_base);

                if (ctx->app_valid == true)
                {
                    ctx->state = BL_STATE_JUMP;
                }
                else
                {
                    ctx->error = BL_ERR_INVALID_VECTOR;
                    ctx->state = BL_STATE_UPDATE_MODE;
                }
            }
            break;
        }

        case BL_STATE_UPDATE_MODE:
        {
            /* CDC update logic will be implemented here */
            break;
        }

        case BL_STATE_VERIFY:
        {
            if (ctx->fw_crc_calculated == ctx->fw_crc_expected)
            {
                ctx->state = BL_STATE_JUMP;
            }
            else
            {
                ctx->error = BL_ERR_CRC_MISMATCH;
                ctx->state = BL_STATE_ERROR;
            }
            break;
        }

        case BL_STATE_JUMP:
        {
            (void)Bootloader_JumpToApplication(ctx);
            ctx->state = BL_STATE_ERROR;
            break;
        }

        case BL_STATE_ERROR:
        default:
        {
            /* Stay here on fatal error */
            break;
        }
    }
}

/**
 * @brief Validate application and perform jump
 *
 * @param[in,out] ctx  Bootloader context structure
 * @return true  Jump executed
 * @return false Jump not possible
 */
bool Bootloader_JumpToApplication(BootloaderCtx_t *ctx)
{
    if (ctx == NULL)
    {
        return false;
    }

    if (BL_IsVectorTableSane(ctx->app_base) == false)
    {
        ctx->error = BL_ERR_INVALID_VECTOR;
        return false;
    }

    BL_Jump(ctx->app_base);
    return true;
}

/* =========================================================
 * Local Helper Functions
 * ========================================================= */

/**
 * @brief Check application vector table integrity
 *
 * @param[in] appBase  Application flash base address
 * @return true  Vector table valid
 * @return false Vector table invalid
 */
static bool BL_IsVectorTableSane(uint32_t appBase)
{
    uint32_t msp;
    uint32_t rst;

    msp = *(volatile uint32_t *)(appBase);
    rst = *(volatile uint32_t *)(appBase + 4U);

    if ((msp < BL_SRAM_BASE) || (msp > BL_SRAM_END))
    {
        return false;
    }

    if ((rst < appBase) || (rst > (appBase + 0x200000UL)))
    {
        return false;
    }

    if ((rst & 0x1U) == 0U)
    {
        return false;
    }

    return true;
}

/**
 * @brief Perform low-level application jump
 *
 * Transfers execution from bootloader to application safely.
 *
 * @param[in] appBase  Application flash base address
 */
static void BL_Jump(uint32_t appBase)
{
    uint32_t appStack;   /* Application'ın başlangıç Main Stack Pointer değeri */
    uint32_t appEntry;   /* Application'ın Reset_Handler (giriş noktası) adresi */

    appStack = *(volatile uint32_t *)(appBase);        /* Vector table [0]: initial MSP */
    appEntry = *(volatile uint32_t *)(appBase + 4U);   /* Vector table [1]: reset handler */

    __disable_irq();     /* Jump sırasında kesmelerin çalışmasını engelle */

    SysTick->CTRL = 0U;  /* SysTick timer'ını tamamen durdur */
    SysTick->LOAD = 0U;  /* SysTick reload değerini sıfırla */
    SysTick->VAL  = 0U;  /* SysTick sayaç değerini temizle */

    HAL_DeInit();        /* HAL tarafından açılmış tüm periferikleri kapat */

    for (uint32_t i = 0U; i < 16U; i++)
    {
        NVIC->ICER[i] = 0xFFFFFFFFUL;  /* Enable edilmiş tüm interrupt'ları devre dışı bırak */
        NVIC->ICPR[i] = 0xFFFFFFFFUL;  /* Pending durumdaki interrupt'ları temizle */
    }

    SCB->VTOR = appBase; /* Vector Table Offset Register'ı application adresine taşı */
    __DSB();             /* VTOR güncellemesinin veri yolunda tamamlanmasını garanti et */
    __ISB();             /* Instruction pipeline'ı yeni vector table ile senkronize et */

    __set_MSP(appStack); /* Main Stack Pointer'ı application stack adresine ayarla */
    __DSB();             /* MSP güncellemesinin tamamlanmasını garanti et */
    __ISB();             /* Yeni stack ile instruction pipeline'ı senkronize et */

    __enable_irq();

    ((pFunction)appEntry)(); /* Application'ın Reset_Handler fonksiyonuna dallan */

    for (;;)
    {
        /* Güvenlik önlemi:
         * Reset_Handler geri dönmemelidir,
         * dönerse burada takılı kalınır */
    }
}

/**
 * @brief Check whether bootloader update mode shall be entered
 *
 * This function determines if the system was reset due to a firmware
 * update request coming from the application.
 *
 * Decision sources:
 *  - EEPROM : persistent update request flag (written by application)
 *  - Backup SRAM : reset awareness and boot-stage tracking
 *
 * @param[in,out] ctx  Bootloader context structure
 *
 * @return true  Bootloader shall enter update mode
 * @return false Normal boot flow shall continue
 */
static bool BL_CheckUpdateRequest(BootloaderCtx_t *ctx)
{
    bl_eeprom_meta_t ee_meta;
    bl_backup_ctx_t  bk_ctx;

    if (ctx == NULL)
    {
        return false;
    }

    /* ---------------------------------------------------------
     * Step 1: Read persistent update request from EEPROM
     * --------------------------------------------------------- */
    if (BL_EEPROM_Read(&at24c32, &ee_meta) != true)
    {
        /* EEPROM okunamıyorsa update isteği yok varsayılır */
        return false;
    }

    /* EEPROM içeriği geçerli mi? */
    if (ee_meta.magic != BL_EE_MAGIC)
    {
        return false;
    }

    /* Application update istemiş mi? */
    if (ee_meta.update_flag != BL_EE_FLAG_UPDATE_REQUEST)
    {
        return false;
    }

    /* ---------------------------------------------------------
     * Step 2: Confirm reset context using Backup SRAM
     * --------------------------------------------------------- */

    /* Backup SRAM'deki mevcut context'i oku (varsa) */
    if (BL_Backup_Read(&bk_ctx) != true)
    {
        /* İlk kez giriliyor olabilir, yeni context oluştur */
        bk_ctx.magic      = BL_BK_MAGIC;
        bk_ctx.state      = BL_BK_STATE_AFTER_UPDATE_RESET;
        bk_ctx.last_error = BL_ERR_NONE;
    }
    else
    {
        /* Önceden update sürecindeysek tekrar girmeyelim */
        if (bk_ctx.state == BL_BK_STATE_UPDATE_IN_PROGRESS)
        {
            ctx->update_in_progress = true;
            return true;
        }
    }

    /* ---------------------------------------------------------
     * Step 3: Mark reset as update-related in Backup SRAM
     * --------------------------------------------------------- */

    bk_ctx.magic      = BL_BK_MAGIC;
    bk_ctx.state      = BL_BK_STATE_AFTER_UPDATE_RESET;
    bk_ctx.last_error = BL_ERR_NONE;

    (void)BL_Backup_Write(&bk_ctx);

    /* ---------------------------------------------------------
     * Step 4: Update bootloader context for debug/trace
     * --------------------------------------------------------- */

    ctx->update_requested   = true;
    ctx->update_in_progress = false;
    ctx->last_event         = BL_STATE_UPDATE_MODE;

    /* ---------------------------------------------------------
     * Step 5: Enter update mode
     * --------------------------------------------------------- */
    return true;
}
