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

/* =========================================================
 * Global Variables
 * ========================================================= */
USBCommParameters_t usbCommParameters;

/* =========================================================
 * External Variables
 * ========================================================= */
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
extern volatile USB_RxDebug_t g_usb_rx_debug;
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

    /* =====================================================
     * META DATA INIT
     * ===================================================== */
    Meta_Init(&ctx->meta);

    /* =====================================================
     * BOOTLOADER INIT
     * ===================================================== */
    ctx->state              			= BL_STATE_INIT;
    ctx->error              			= BL_ERR_NONE;

    ctx->tick_start         			= HAL_GetTick();
    ctx->boot_elapsed_ms    			= 0U;

    ctx->update_requested   			= false;
    ctx->update_in_progress 			= false;

    //ctx->app_base           			= BL_APP_BASE_ADDRESS;
    if(ctx->meta.active_slot == META_SLOT_B)
    	ctx->app_base = BL_APP_SLOT2_ADDRESS;
    else
    	ctx->app_base = BL_APP_BASE_ADDRESS;
    ctx->app_valid          			= false;

    ctx->update_info.fw_size_bytes		= 0U;
    ctx->update_info.fw_crc32  			= 0U;
    ctx->update_info.fw_format 			= 0U;
    ctx->update_info.fw_version.major 	= 0U;
    ctx->update_info.fw_version.minor 	= 0U;
    ctx->update_info.fw_version.patch 	= 0U;

    ctx->last_event         			= 0U;
    ctx->reset_reason       			= RCC->CSR;

    ctx->state              			= BL_STATE_CHECK_UPDATE;
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

    static uint32_t updateInfoTime = 0;
    ctx->boot_elapsed_ms = HAL_GetTick() - ctx->tick_start;

    System_USB_Communication_Receive_Function(&usbCommParameters);

    {
    	/*
    	 * USB COMMAND MESSAGE DIRECTION
    	 */
    	if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_FIRMWARE_UPDATE &&
    			usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command.USB_firmware_update_command_id == USB_FIRMWARE_CMD_EXIT_BOOTLOADER)
    	{
    		// Bootloader dan çıkar...
    	}

    	if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_FIRMWARE_UPDATE &&
    			usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command.USB_firmware_update_command_id == USB_FIRMWARE_CMD_SHUTDOWN_DEVICE)
    	{
    		// Cihazı kapatır...
    		HAL_GPIO_WritePin(SYSTEM_SHUTDOWN_GPIO_Port, SYSTEM_SHUTDOWN_Pin, GPIO_PIN_RESET);
    	}

    	if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_FIRMWARE_UPDATE &&
    			usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command.USB_firmware_update_command_id == USB_FIRMWARE_CMD_GO_APPLICATION)
    	{
    		// VARSA Application koduna atlar...
    		ctx->state = BL_STATE_JUMP;
    	}

    	if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_FIRMWARE_UPDATE &&
    			usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command.USB_firmware_update_command_id == USB_FIRMWARE_CMD_RESET_DEVICE)
    	{
    		// Cihaza reset atar...
    		HAL_NVIC_SystemReset();
    	}

    }

    switch (ctx->state)
    {
        case BL_STATE_CHECK_UPDATE:
        {
            ctx->update_requested = BL_CheckUpdateRequest(ctx);

            if (ctx->update_requested == true)
            {
                ctx->state 			= BL_STATE_SELECT_TARGET;
                ctx->updateState	= BL_UPDATE_IDLE;
        		updateInfoTime 		= HAL_GetTick();
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
                    ctx->error 			= BL_ERR_INVALID_VECTOR;
                    ctx->state 			= BL_STATE_SELECT_TARGET;
                    ctx->updateState	= BL_UPDATE_IDLE;
            		updateInfoTime 		= HAL_GetTick();
                }
            }
            break;
        }

        case BL_STATE_SELECT_TARGET:
        {
            /* Slot doluluk bilgisi:
             * Basit yaklaşım: app_base üzerinden aktif slot biliniyor varsayımı
             * app_base = SLOT_A_BASE_ADDR veya SLOT_B_BASE_ADDR
             */

        	if(ctx->meta.active_slot == META_SLOT_NONE || ctx->meta.target_slot  == META_SLOT_NONE)
        	{
        		/*
        		 * TODO: Belki tüm flash temizlenip hedef slot A olarak belirlenebilir.
        		 */
    			ctx->update_target_info.g_target_slot 			= BL_SLOT_A;
        		ctx->update_target_info.g_target_base_addr 		= SLOT_A_BASE_ADDR;
        		ctx->update_target_info.g_target_end_addr  		= SLOT_A_END_ADDR;
        	}
        	else
        	{
        		if(ctx->meta.target_slot == META_SLOT_A)
        		{
        			ctx->update_target_info.g_target_slot 		= BL_SLOT_A;
            		ctx->update_target_info.g_target_base_addr 	= SLOT_A_BASE_ADDR;
            		ctx->update_target_info.g_target_end_addr  	= SLOT_A_END_ADDR;
        		}
        		else
        		{
        			ctx->update_target_info.g_target_slot 		= BL_SLOT_B;
            		ctx->update_target_info.g_target_base_addr 	= SLOT_B_BASE_ADDR;
            		ctx->update_target_info.g_target_end_addr  	= SLOT_B_END_ADDR;
        		}
        	}

            ctx->state 			= BL_STATE_ERASE_TARGET;

            break;
        }

        case BL_STATE_ERASE_TARGET:
        {
            FLASH_EraseInitTypeDef erase_cfg;
            uint32_t page_error = 0;

            uint32_t base_addr = ctx->update_target_info.g_target_base_addr;
            uint32_t end_addr  = ctx->update_target_info.g_target_end_addr;

            uint32_t start_page = (base_addr - FLASH_BASE) / _FLASH_PAGE_SIZE;
            uint32_t end_page   = (end_addr  - FLASH_BASE) / _FLASH_PAGE_SIZE;

            erase_cfg.TypeErase = FLASH_TYPEERASE_PAGES;
            erase_cfg.Banks     = FLASH_BANK_1;   /* Slot A/B bank ayırımı sende zaten var */
            erase_cfg.Page      = start_page;
            erase_cfg.NbPages   = (end_page - start_page) + 1U;

            HAL_FLASH_Unlock();

            if (HAL_FLASHEx_Erase(&erase_cfg, &page_error) != HAL_OK)
            {
                HAL_FLASH_Lock();
                ctx->error = BL_ERR_FLASH_WRITE;
                ctx->state = BL_STATE_ERROR;
                break;
            }

            HAL_FLASH_Lock();

            /* Yazma adreslerini target’a göre ayarla */
            ctx->update_packet_info.startAddress   = base_addr;
            ctx->update_packet_info.currentAddress = base_addr;

            ctx->update_packet_info.remainingDataLength =
                    (ctx->update_info.fw_size_bytes > 0U) ?
                     ctx->update_info.fw_size_bytes :
                     BL_APP_MAX_SIZE;

            ctx->update_packet_info.requestedDataLength = BL_PACKET_SIZE;

            ctx->state 			= BL_STATE_UPDATE_MODE;
            ctx->updateState 	= BL_UPDATE_IDLE;

            break;
        }

        case BL_STATE_UPDATE_MODE:
        {
            /* CDC update logic will be implemented here */

        	/*
        	 * 1- Masaüstü tarafına boot moda girdiğimizi söylememiz lazım (USB Tarafından boot moda girdiğine dair mesaj gönder)
        	 *
        	 * 2- Bilgi bekliyoruz.
        	 *
        	 * 3- Masaüstü tarafından paket uzunluğu bekleniyor. (Paket uzunluğunu gönderecek)
        	 *
        	 * (1 - 3 : Ben update sekansına girdim senden yüklenecek dosyanın bilgilerini bekliyorum)
        	 *
        	 *
        	 */

        	switch(ctx->updateState)
        	{
        	case BL_UPDATE_IDLE:
        		/*
        		 * Bazı kontroller yapılır ve BL_UPDATE_READY ye yönlendirilir.
        		 *
        		 * 30 sn içerisinde PC tarafından komut gelmezse sistemi kapat yada applicationa geç
        		 */

        		if(usbCommParameters.USB_rx_parameters.usbRxFlag)
        		{
    				usbCommParameters.USB_rx_parameters.usbRxFlag = 0;

        			if(usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type 								== USB_PACKET_FIRMWARE_UPDATE &&
        				usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command.USB_firmware_update_command_id 	== USB_FIRMWARE_UPDATE_STATUS_REQ)
        			{
                        ctx->updateState	= BL_UPDATE_READY;

                		updateInfoTime = HAL_GetTick();
        			}

        			memset(&usbCommParameters, 0, sizeof(usbCommParameters));
        		}

        		if(abs(ctx->boot_elapsed_ms - updateInfoTime) >= 30000)
        		{
        			// TODO: Go to shutdown or application
        		}

        		break;

        	case BL_UPDATE_READY:

        		g_usb_rx_debug.rx_callback_count 	= 0;
        		g_usb_rx_debug.frame_completed 		= 0;
        	    g_usb_rx_debug.total_received_bytes = 0;

        		/*
        		 * Her 1 saniye de 1 masaüstü uygulamasına mesaj gönderilir.
        		 */
        		if(abs(ctx->boot_elapsed_ms - updateInfoTime) >= 100)
        		{
            		updateInfoTime = HAL_GetTick();

            		uint8_t payload[2];

            		/*
            		 * MCU - > PC : Send BL Update Ready Info
            		 */

                    /* Active slot */
                    switch (ctx->meta.active_slot)
                    {
                        case META_SLOT_A: payload[0] = USB_MSG_BL_SLOT_A; break;
                        case META_SLOT_B: payload[0] = USB_MSG_BL_SLOT_B; break;
                        default:          payload[0] = USB_MSG_BL_SLOT_NONE; break;
                    }

                    /* Target slot */
                    switch (ctx->meta.target_slot)
                    {
                        case META_SLOT_A: payload[1] = USB_MSG_BL_SLOT_A; break;
                        case META_SLOT_B: payload[1] = USB_MSG_BL_SLOT_B; break;
                        default:          payload[1] = USB_MSG_BL_SLOT_NONE; break;
                    }

                    usbCommParameters.USB_tx_parameters =
                        *USB_Prepare_Transmit_Buffer(
                            USB_PACKET_FIRMWARE_UPDATE,
                            USB_FIRMWARE_UPDATE_READY,
                            0,
                            sizeof(payload),
                            payload
                        );

                    uint8_t transmitStatus = USB_Transmit(
                        usbCommParameters.USB_tx_parameters.usbTxBuf,
                        usbCommParameters.USB_tx_parameters.usbTxBufLen
                    );

            		if(transmitStatus != USBD_OK)
            		{
            			(void)USB_Transmit(usbCommParameters.USB_tx_parameters.usbTxBuf,
		 	 	 	 	 	 	  usbCommParameters.USB_tx_parameters.usbTxBufLen);
            		}

        			memset(&usbCommParameters, 0, sizeof(usbCommParameters));

                    ctx->updateState	= BL_UPDATE_REQUEST_UPDATE_INFO;
        		}

        		break;

        	case BL_UPDATE_REQUEST_UPDATE_INFO:

        		g_usb_rx_debug.rx_callback_count 	= 0;
        		g_usb_rx_debug.frame_completed 		= 0;
        	    g_usb_rx_debug.total_received_bytes = 0;

        		if (usbCommParameters.USB_rx_parameters.usbRxFlag)
        		{
        		    usbCommParameters.USB_rx_parameters.usbRxFlag = 0;

        		    if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type ==
        		            USB_PACKET_FIRMWARE_UPDATE &&
        		        usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command
        		            .USB_firmware_update_command_id ==
        		            USB_FIRMWARE_UPDATE_PACKET_INFO)
        		    {
        		        uint8_t *rx;

        		        /* RX data pointer */
        		        rx = usbCommParameters.USB_rx_parameters.USB_rx_packet_info.data;

        		        /* Güvenlik: uzunluk kontrolü */
        		        if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.data_len >= 13U)
        		        {
        		            bl_update_info_t *info = &ctx->update_info;

        		            /* fw_size_bytes */
        		            info->fw_size_bytes =
        		                  ((uint32_t)rx[0])
        		                | ((uint32_t)rx[1] << 8)
        		                | ((uint32_t)rx[2] << 16)
        		                | ((uint32_t)rx[3] << 24);

        		            /* fw_crc32 */
        		            info->fw_crc32 =
        		                  ((uint32_t)rx[4])
        		                | ((uint32_t)rx[5] << 8)
        		                | ((uint32_t)rx[6] << 16)
        		                | ((uint32_t)rx[7] << 24);

        		            /* fw_format */
        		            info->fw_format = (bl_fw_format_t)rx[8];

        		            /* fw_version */
        		            info->fw_version.major = rx[11];
        		            info->fw_version.minor = rx[10];
        		            info->fw_version.patch = rx[9];

        		            /* State ilerlet */
        		            ctx->updateState = BL_UPDATE_CHECK_INFO;
        		        }
        		        else
        		        {
        		            /* Paket eksik / hatalı */
        		            ctx->updateState = BL_UPDATE_ERROR;
        		        }
        		    }
        		    else if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type ==
        		                USB_PACKET_FIRMWARE_UPDATE &&
        		             usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command
        		                .USB_firmware_update_command_id == USB_FIRMWARE_FLASH_ERASE)
        		    {
        		        FLASH_EraseInitTypeDef erase_cfg;
        		        uint32_t page_error = 0;

        		        /* -------------------------------------------------
        		         * SLOT A ERASE
        		         * ------------------------------------------------- */
        		        {
        		            const uint32_t base_addr = SLOT_A_BASE_ADDR;
        		            const uint32_t end_addr  = SLOT_A_END_ADDR;

        		            const uint32_t start_page = (base_addr - FLASH_BASE) / _FLASH_PAGE_SIZE;
        		            const uint32_t end_page   = (end_addr  - FLASH_BASE) / _FLASH_PAGE_SIZE;

        		            erase_cfg.TypeErase = FLASH_TYPEERASE_PAGES;

        		            /* Slot A için bank seçimi: */
        		            erase_cfg.Banks   = FLASH_BANK_1;
        		            erase_cfg.Page    = start_page;
        		            erase_cfg.NbPages = (end_page - start_page) + 1U;

        		            HAL_FLASH_Unlock();

        		            if (HAL_FLASHEx_Erase(&erase_cfg, &page_error) != HAL_OK)
        		            {
        		                HAL_FLASH_Lock();
        		                ctx->error = BL_ERR_FLASH_WRITE;
        		                //ctx->state = BL_STATE_ERROR;
        		                return;
        		            }

        		            HAL_FLASH_Lock();
        		        }

        		        /* -------------------------------------------------
        		         * SLOT B ERASE
        		         * ------------------------------------------------- */
        		        {
        		            const uint32_t base_addr = SLOT_B_BASE_ADDR;
        		            const uint32_t end_addr  = SLOT_B_END_ADDR;

        		            const uint32_t start_page = (base_addr - FLASH_BASE) / _FLASH_PAGE_SIZE;
        		            const uint32_t end_page   = (end_addr  - FLASH_BASE) / _FLASH_PAGE_SIZE;

        		            erase_cfg.TypeErase = FLASH_TYPEERASE_PAGES;

        		            /* Slot B için bank seçimi: */
        		            erase_cfg.Banks   = FLASH_BANK_1;
        		            erase_cfg.Page    = start_page;
        		            erase_cfg.NbPages = (end_page - start_page) + 1U;

        		            HAL_FLASH_Unlock();

        		            if (HAL_FLASHEx_Erase(&erase_cfg, &page_error) != HAL_OK)
        		            {
        		                HAL_FLASH_Lock();
        		                ctx->error = BL_ERR_FLASH_WRITE;
        		                //ctx->state = BL_STATE_ERROR;
        		                return;
        		            }

        		            HAL_FLASH_Lock();
        		        }

        		        /* -------------------------------------------------
        		         * 2) Metadata temizle + güncelle (iki slot da boş)
        		         *    - Meta flash alanını erase et
        		         *    - Tutarlı bir meta record yaz (CRC dahil)
        		         * ------------------------------------------------- */
        		        {
        		            meta_record_t m;
        		            memset(&m, 0, sizeof(m));

        		            /* "Temiz + boş" meta kaydı oluştur */
        		            m.magic        = META_MAGIC;
        		            m.seq          = (ctx->meta.seq == 0xFFFFFFFFu || ctx->meta.seq == 0u) ? 1u : (ctx->meta.seq + 1u);

        		            /* Her iki slot boş: valid=0 */
        		            m.slotA.valid  = 0u;
        		            m.slotB.valid  = 0u;
        		            m.progress_bytes = 0u;

        		            /* Sanity check’lerden geçsin diye A/B set ediyoruz,
        		               ama state NO_APP ile “geçerli app yok” bilgisini veriyoruz. */
        		            m.active_slot  = META_SLOT_A;
        		            m.target_slot  = META_SLOT_B;
        		            m.update_state = META_UPDATE_NO_APP;

        		            /* Meta_Write zaten erase + crc + write yapıyor.
        		               Ama önce meta flash’ı tamamen temizlemek istedin, o yüzden ayrıca erase de atıyoruz. */
        		            (void)Flash_Erase(META_FLASH_ADDR);

        		            if (Meta_Write(&m) != true)
        		            {
        		                ctx->error = BL_ERR_FLASH_WRITE;
        		                ctx->state = BL_STATE_ERROR;
        		                return;
        		            }

        		            /* RAM'deki meta’yı da senkronla */
        		            ctx->meta = m;
        		        }

        		        /* -------------------------------------------------
        		         * 3) Hedef slotu SLOT A olarak işaretle
        		         * ------------------------------------------------- */
        		        ctx->update_target_info.g_target_slot      = BL_SLOT_A;
        		        ctx->update_target_info.g_target_base_addr = SLOT_A_BASE_ADDR;
        		        ctx->update_target_info.g_target_end_addr  = SLOT_A_END_ADDR;

        		    }
        		}

        		break;

        	case BL_UPDATE_CHECK_INFO:

        		g_usb_rx_debug.rx_callback_count 	= 0;
        		g_usb_rx_debug.frame_completed 		= 0;
        	    g_usb_rx_debug.total_received_bytes = 0;

        		if(ctx->update_info.fw_size_bytes <= BL_APP_MAX_SIZE)
        		{
        			ctx->updateState 		= BL_UPDATE_REQUEST_PACKET;
        			ctx->update_requested 	= true;

        			ctx->update_packet_info.startAddress 		= 0x00000000;
        			ctx->update_packet_info.remainingDataLength = ctx->update_info.fw_size_bytes;
        		}
        		else
        		{
        			ctx->updateState = BL_UPDATE_ERROR;
        		}

        		break;

        	case BL_UPDATE_REQUEST_PACKET:

        		if(ctx->update_requested && ctx->update_in_progress == false)
        		{
        			ctx->update_requested 	= false;
        			ctx->update_in_progress	= true;

        			/*
        			 * Belirli bir adresten itibaren belirli uzunlukta veri talep et
        			 */
        			if(ctx->update_packet_info.remainingDataLength >= 1024)
        			{
        				ctx->update_packet_info.requestedDataLength = 1024;
        			}
        			else
        			{
        				ctx->update_packet_info.requestedDataLength = ctx->update_packet_info.remainingDataLength;
        			}

        			uint8_t packet[8] 	= {0};
        			uint16_t dataLength = 8;

        			packet[0] = (uint8_t)( ctx->update_packet_info.startAddress >> 24 & 0xFF);
        			packet[1] = (uint8_t)( ctx->update_packet_info.startAddress >> 16 & 0xFF);
        			packet[2] = (uint8_t)( ctx->update_packet_info.startAddress >> 8  & 0xFF);
        			packet[3] = (uint8_t)( ctx->update_packet_info.startAddress >> 0  & 0xFF);
        			packet[4] = (uint8_t)( ctx->update_packet_info.requestedDataLength >> 24 & 0xFF);
        			packet[5] = (uint8_t)( ctx->update_packet_info.requestedDataLength >> 16 & 0xFF);
        			packet[6] = (uint8_t)( ctx->update_packet_info.requestedDataLength >> 8  & 0xFF);
        			packet[7] = (uint8_t)( ctx->update_packet_info.requestedDataLength >> 0  & 0xFF);


            		usbCommParameters.USB_tx_parameters = *USB_Prepare_Transmit_Buffer(USB_PACKET_FIRMWARE_UPDATE,
																					   USB_FIRMWARE_UPDATE_GET_PACKET,
																					   0,
																					   dataLength,
																					   packet);
            		uint8_t transmitStatus = USB_Transmit(usbCommParameters.USB_tx_parameters.usbTxBuf,
            					 	 	 	 	 	 	  usbCommParameters.USB_tx_parameters.usbTxBufLen);

            		if(transmitStatus != USBD_OK)
            		{
            			(void)USB_Transmit(usbCommParameters.USB_tx_parameters.usbTxBuf,
		 	 	 	 	 	 	  usbCommParameters.USB_tx_parameters.usbTxBufLen);
            		}

        			memset(&usbCommParameters, 0, sizeof(usbCommParameters));

        			ctx->updateState = BL_UPDATE_RECEIVE_DATA;
        		}

        		break;

        	case BL_UPDATE_RECEIVE_DATA:

        		if (usbCommParameters.USB_rx_parameters.usbRxFlag)
        		{
        		    usbCommParameters.USB_rx_parameters.usbRxFlag = 0;

        		    if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.packet_type ==
        		            USB_PACKET_FIRMWARE_UPDATE &&
        		        usbCommParameters.USB_rx_parameters.USB_rx_packet_info.command
        		            .USB_firmware_update_command_id ==
        		            		USB_FIRMWARE_UPDATE_SEND_PACKET)
        		    {
        		        uint8_t *rx;

        		        /* RX data pointer */
        		        rx = usbCommParameters.USB_rx_parameters.USB_rx_packet_info.data;

        		        /* Güvenlik: uzunluk kontrolü */
        		        if (usbCommParameters.USB_rx_parameters.USB_rx_packet_info.data_len >= ctx->update_packet_info.requestedDataLength)
        		        {
        		        	/*
        		        	 * Gelen data içerisinden ilk ctx->update_packet_info.requestedDataLength kadarı veriyi
        		        	 * ondan sonraki 4 byte ise CRC32 yi içermektedir. İlk olarak crc32 kontorlünün yapılması gerekir.
        		        	 */

        		        	Bootloader_Packet_Parser(&ctx->update_packet,
        		        							 rx,
        		        							 usbCommParameters.USB_rx_parameters.USB_rx_packet_info.data_len);

                			ctx->updateState = BL_UPDATE_VERIFY;
        		        }

        		    }
        		}

        		break;

        	case BL_UPDATE_VERIFY:

        		if(CRC32_Verify(ctx->update_packet.packetBuff, ctx->update_packet.packetLen, ctx->update_packet.packetCRC))
        		{
        			/*
        			 * CRC OK Gönder ve devam et
        			 */

        			ctx->update_packet.requestCounter	= 0;
        			ctx->update_packet.crcStatus 		= USB_CRC_OK;
        			ctx->updateState 					= BL_UPDATE_WRITE_FLASH;
        		}
        		else
        		{
        			/*
        			 * CRC NOK Gönder ve paketi tekrar iste
        			 */

        			ctx->update_packet.crcStatus 		= USB_CRC_NOK;
        			ctx->update_packet.requestCounter	+= 1;

        			if(ctx->update_packet.requestCounter >= 4)
        			{
        				// ERROR
        				ctx->updateState = BL_UPDATE_ERROR;
        				ctx->state		 = BL_STATE_ERROR;
        			}
        		}

    			uint8_t usbPacket[1] 	 = {0};
    			uint16_t usbPacketLength = 1;

     			usbPacket[0]   = ctx->update_packet.crcStatus;

        		usbCommParameters.USB_tx_parameters = *USB_Prepare_Transmit_Buffer(USB_PACKET_FIRMWARE_UPDATE, USB_FIRMWARE_UPDATE_VERIFY_PACKET, 0, usbPacketLength, usbPacket);
        		uint8_t transmitStatus = USB_Transmit(usbCommParameters.USB_tx_parameters.usbTxBuf, usbCommParameters.USB_tx_parameters.usbTxBufLen);

        		if(transmitStatus != USBD_OK)
        		{
        			(void)USB_Transmit(usbCommParameters.USB_tx_parameters.usbTxBuf,
	 	 	 	 	 	 	  usbCommParameters.USB_tx_parameters.usbTxBufLen);
        		}

    			memset(&usbCommParameters, 0, sizeof(usbCommParameters));

        		break;

        	case BL_UPDATE_WRITE_FLASH:

        	    bool flash_status	= 0;
        	    uint8_t retry_cnt = 0U;

        	    uint32_t targetAddress = ctx->update_target_info.g_target_base_addr + ctx->update_packet.packetAddr;

        	    /* -------------------------------------------------
        	     * Write data to flash (with retry)
        	     * ------------------------------------------------- */
        	    while ((flash_status == false) && (retry_cnt < BL_FLASH_WRITE_RETRY_COUNT))
        	    {
        	        flash_status = Flash_Write(
        	        					targetAddress,
        	                            ctx->update_packet.packetBuff,
        	                            ctx->update_packet.packetLen
        	                        );

        	        retry_cnt++;
        	    }

        	    /* -------------------------------------------------
        	     * Update flags
        	     * ------------------------------------------------- */
        	    ctx->update_requested						= true;
        	    ctx->update_in_progress						= false;

        	    /* -------------------------------------------------
        	     * Flash write failed after retries
        	     * ------------------------------------------------- */
        	    if (flash_status != true)
        	    {
        	        ctx->error = BL_ERR_FLASH_WRITE;
        	        ctx->state = BL_STATE_ERROR;
        	        break;
        	    }

        	    /* -------------------------------------------------
        	     * Update progress
        	     * ------------------------------------------------- */
        	    ctx->update_packet_info.remainingDataLength -= ctx->update_packet.packetLen;
        	    ctx->update_packet_info.startAddress 		= ctx->update_packet.packetAddr + ctx->update_packet.packetLen;

        	    if(ctx->update_packet_info.remainingDataLength <= 0)
        	    {
            	    /* -------------------------------------------------
            	     * Finish packet
            	     * ------------------------------------------------- */
            	    ctx->updateState = BL_UPDATE_FINISH;
        	    }
        	    else
        	    {
            	    /* -------------------------------------------------
            	     * Ready for next packet
            	     * ------------------------------------------------- */
            	    ctx->updateState = BL_UPDATE_REQUEST_PACKET;
        	    }


        		break;

        	case BL_UPDATE_ERROR:

        		break;

        	case BL_UPDATE_FINISH:

        		/*
        		 * Global firmware doğrulama  : Alınan paketlerin tamamını doğrula (full packet crc)
        		 * Metadata güncelleme		  : Determine metadata side			  ()
        		 * Update Tamamlanama Bilgisi : Masaüstü tarafına güncelleme sekansının tamamlandığını aktarma
        		 */
        		uint32_t calculated_crc	= 0;
        		uint32_t expected_crc	= 0;

        		expected_crc 			= ctx->update_info.fw_crc32;

        		calculated_crc = CRC32_Calculate(
												 (uint8_t *)ctx->update_target_info.g_target_base_addr,
												 ctx->update_info.fw_size_bytes
												);

        		if (calculated_crc != expected_crc)
        		{
        		    ctx->error = BL_ERR_APP_CRC;
        		    ctx->state = BL_STATE_ERROR;
        		    return;
        		}

        		ctx->state = BL_STATE_VERIFY;

        		break;

        	default:

        		break;
        	}

            break;
        }

        case BL_STATE_VERIFY:
        {
            meta_record_t meta;
            bool write_ok;

            /* -------------------------------------------------
             * 1) Metadata oku
             * ------------------------------------------------- */
            if (Meta_Read(&meta) != true)
            {
                /* Metadata okunamadı → slotlara bakarak oluştur */
                Meta_Init_FromSlots(&meta);
            }

            /* -------------------------------------------------
             * 2) Metadata geçerli mi?  (CRC self-field hariç)
             * ------------------------------------------------- */
            {
                meta_record_t tmp = meta;
                tmp.crc = 0U;

                uint32_t calc_crc = CRC32_Calculate(
                    ((uint8_t *)&tmp) + sizeof(uint32_t),      /* magic hariç */
                    sizeof(meta_record_t) - sizeof(uint32_t)   /* crc dahil struct boyutu */
                );

                if ((meta.magic != META_MAGIC) || (calc_crc != meta.crc))
                {
                    /* Bozuk metadata → slotlara bakarak yeniden oluştur */
                    Meta_Init_FromSlots(&meta);
                }
            }

            /* -------------------------------------------------
             * 3) Target slot kontrolü
             * ------------------------------------------------- */
            meta_slot_t new_slot = meta.target_slot;

            if (new_slot == META_SLOT_NONE)
            {
                ctx->error = BL_ERR_INVALID_VECTOR;
                ctx->state = BL_STATE_ERROR;
                break;
            }

            /* -------------------------------------------------
             * 4) Metadata güncelle
             * ------------------------------------------------- */
            meta.active_slot    = new_slot;
            meta.target_slot    = (new_slot == META_SLOT_A) ? META_SLOT_B : META_SLOT_A;
            meta.update_state   = META_UPDATE_IDLE;
            meta.seq++;
            meta.progress_bytes = 0U;

            meta_slot_info_t *slot =
                (new_slot == META_SLOT_A) ? &meta.slotA : &meta.slotB;

            /* -------------------------------------------------
             * 4.1) Slot firmware bilgilerini YAPIYA UYUMLU yaz
             * ------------------------------------------------- */
            slot->fw.size_bytes    = ctx->update_info.fw_size_bytes;
            slot->fw.crc32         = ctx->update_info.fw_crc32;
            slot->fw.version_major = ctx->update_info.fw_version.major;
            slot->fw.version_minor = ctx->update_info.fw_version.minor;
            slot->fw.version_patch = ctx->update_info.fw_version.patch;
            slot->valid            = 1U;

            /* -------------------------------------------------
             * 5) CRC yeniden hesapla (KRİTİK: önce 0'la)
             * ------------------------------------------------- */
/*
            meta.crc = 0U;
            meta.crc = CRC32_Calculate(
                ((uint8_t *)&meta) + sizeof(uint32_t),
                sizeof(meta_record_t) - sizeof(uint32_t)
            );
*/
            /* -------------------------------------------------
             * 6) Metadata yaz
             * ------------------------------------------------- */
            write_ok = Meta_Write(&meta);
            if (write_ok != true)
            {
                ctx->error = BL_ERR_FLASH_WRITE;
                ctx->state = BL_STATE_ERROR;
                break;
            }

            /* -------------------------------------------------
             * 6.1) Active slota göre app_base ayarla
             * ------------------------------------------------- */
            if (meta.active_slot == META_SLOT_B)
                ctx->app_base = BL_APP_SLOT2_ADDRESS;
            else
                ctx->app_base = BL_APP_BASE_ADDRESS;

            /* -------------------------------------------------
             * 7) Güncelleme tamam → uygulamaya geç
             * ------------------------------------------------- */
            ctx->state = BL_STATE_JUMP;
            break;
        }

        case BL_STATE_JUMP:
        {
            usbCommParameters.USB_tx_parameters =
                *USB_Prepare_Transmit_Buffer(
                    USB_PACKET_FIRMWARE_UPDATE,
					USB_FIRMWARE_JUMPING_APPLICATION,
                    0,
                    0,
                    NULL
                );

            uint8_t transmitStatus = USB_Transmit(
                usbCommParameters.USB_tx_parameters.usbTxBuf,
                usbCommParameters.USB_tx_parameters.usbTxBufLen
            );

    		if(transmitStatus != USBD_OK)
    		{
    			(void)USB_Transmit(usbCommParameters.USB_tx_parameters.usbTxBuf,
 	 	 	 	 	 	  usbCommParameters.USB_tx_parameters.usbTxBufLen);
    		}

			memset(&usbCommParameters, 0, sizeof(usbCommParameters));


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

void Bootloader_Packet_Parser(bl_update_packet_t *ctxPacket, uint8_t *buff, uint16_t len)
{
    /* Start address */
    ctxPacket->packetAddr =
        ((uint32_t)buff[0] << 24) |
        ((uint32_t)buff[1] << 16) |
        ((uint32_t)buff[2] << 8)  |
        ((uint32_t)buff[3]);

    /* Data length */
    ctxPacket->packetLen =
        ((uint32_t)buff[4] << 24) |
        ((uint32_t)buff[5] << 16) |
        ((uint32_t)buff[6] << 8)  |
        ((uint32_t)buff[7]);

    /* Data copy */
    for (uint32_t i = 0; i < ctxPacket->packetLen; i++)
    {
        ctxPacket->packetBuff[i] = buff[8 + i];
    }

    /* CRC32 */
    ctxPacket->packetCRC =
        ((uint32_t)buff[len - 4] << 24) |
        ((uint32_t)buff[len - 3] << 16) |
        ((uint32_t)buff[len - 2] << 8)  |
        ((uint32_t)buff[len - 1]);
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
    bool             update_from_eeprom = false;
    bool             update_from_backup = false;

    if (ctx == NULL)
    {
        return false;
    }

    /* =========================================================
     * Step 1: Check RTC Backup Register (reset-based trigger)
     * ========================================================= */
    if (BL_RTCBackup_IsUpdateRequested(&hrtc) == true)
    {
        /* Tek seferlik davranış */
        update_from_backup = true;
    }

    /* =========================================================
     * Step 2: Check persistent update request from EEPROM
     * ========================================================= */
    if (BL_EEPROM_Read(&at24c32, &ee_meta) == true)
    {
        if ((ee_meta.magic == BL_EE_MAGIC) &&
            (ee_meta.update_flag == BL_EE_FLAG_UPDATE_REQUEST))
        {
            update_from_eeprom = true;
        }
    }

    /* =========================================================
     * Step 3: Final decision (OR logic)
     * ========================================================= */
    if ((update_from_backup == false) &&
        (update_from_eeprom == false))
    {
        /* Ne EEPROM ne de Backup update istiyor */
        return false;
    }

    /* =========================================================
     * Step 4: Update bootloader context
     * ========================================================= */
    BL_RTCBackup_ClearUpdateRequest(&hrtc);
    BL_EEPROM_ClearUpdateFlag(&at24c32);

    ctx->update_requested   = true;
    ctx->update_in_progress = false;
    ctx->last_event         = BL_STATE_UPDATE_MODE;

    return true;
}

