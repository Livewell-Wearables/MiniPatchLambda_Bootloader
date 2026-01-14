/*
 * bootloader_driver.h
 *
 *  Created on: Dec 30, 2025
 *      Author: Fatih
 */

#ifndef BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_DRIVER_H_
#define BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_DRIVER_H_

#include "main.h"
#include <stdint.h>
#include <stdbool.h>
#include "bootloader_eeprom.h"
#include "bootloader_sram.h"
#include "crc.h"
#include "USB_Receive.h"
#include "USB_Transmit.h"

extern RTC_HandleTypeDef hrtc;

/* =========================================================
 * Flash / Memory Configuration
 * ========================================================= */

#define BL_APP_BASE_ADDRESS      (0x08040000UL)
#define BL_APP_SLOT2_ADDRESS     (0x08200000UL)
#define BL_APP_MAX_SIZE			 1792000

#define BL_SRAM_BASE             (0x20000000UL)
#define BL_SRAM_SIZE             (2496UL * 1024UL)
#define BL_SRAM_END              (BL_SRAM_BASE + BL_SRAM_SIZE)

/* Boot decision window (ms) */
#define BL_BOOT_WINDOW_MS        (3000U)

#define BL_PACKET_SIZE			 (1036)

typedef enum
{
	USB_CRC_OK 	= 0,
	USB_CRC_NOK = 1
}crc_status_t;

/* =========================================================
 * Bootloader States
 * ========================================================= */
typedef enum
{
    BL_STATE_RESET = 0,
    BL_STATE_INIT,
    BL_STATE_CHECK_UPDATE,
    BL_STATE_WAIT,
    BL_STATE_UPDATE_MODE,
    BL_STATE_VERIFY,
    BL_STATE_JUMP,
    BL_STATE_ERROR
} bl_state_t;

typedef enum
{
	BL_UPDATE_IDLE	= 0,			// Henüz sistem boot modda değil, hazırlıklar sürüyor.
	BL_UPDATE_READY,				// USB üzerinden PC ye bootlaoder moda geçtiğimizi ifade eden mesaj (1 sn de 1 gönder)
	BL_UPDATE_REQUEST_UPDATE_INFO,	// PC'den güncelleme paketinin bilgileri istenir.
	BL_UPDATE_CHECK_INFO,			// Gelen bilgi kontol edilir ve uygun ise uygun mesajı gönderilip paket talep edilecek
	BL_UPDATE_REQUEST_PACKET,		// USB üzerinden PC ye sıradaki paketi göndermesi söylenir
	BL_UPDATE_RECEIVE_DATA,			// PC den gönderilen paket alınır ve ayrıştırılır
	BL_UPDATE_VERIFY,				// PC den gelen paket doğrulanır
	BL_UPDATE_WRITE_FLASH,			// PC den gelen ayrıştırılmış ve doğrulanmış paket Flash'a yazılır
	BL_UPDATE_ERROR,				// Herhangi bir hata durumunda PC ye bilgi verilip sistem kapatılacak
	BL_UPDATE_FINISH				// Güncelleme tamamalanacak ve application a geçilecek
}bl_update_state_t;

/* =========================================================
 * Bootloader Error Codes
 * ========================================================= */
typedef enum
{
    BL_ERR_NONE = 0,
    BL_ERR_INVALID_VECTOR,
    BL_ERR_CRC_MISMATCH,
    BL_ERR_FLASH_WRITE,
    BL_ERR_TIMEOUT,
    BL_ERR_UNKNOWN
} bl_error_t;

typedef enum
{
    BL_FW_FORMAT_BIN = 0,
    BL_FW_FORMAT_HEX = 1
} bl_fw_format_t;

/* =========================================================
 * Bootloader Control / Context Structure
 * ========================================================= */
typedef struct
{
    uint8_t major;   /* Major version */
    uint8_t minor;   /* Minor version */
    uint8_t patch;   /* Patch version */
} bl_fw_version_t;

typedef struct
{
    uint32_t 			fw_size_bytes;     // Toplam firmware boyutu
    uint32_t 			fw_crc32;          // Tüm dosyanın CRC32
    bl_fw_format_t  	fw_format;         // 0=BIN, 1=HEX
    bl_fw_version_t		fw_version;        // opsiyonel
} bl_update_info_t;

typedef struct
{
	uint32_t			startAddress;
	uint32_t			currentAddress;
	uint32_t			requestedDataLength;
	uint32_t			remainingDataLength;
}bl_update_request_packet_info_t;

typedef struct
{
	uint8_t 			packetBuff[BL_PACKET_SIZE];
	uint32_t			packetStartAddress;
	uint16_t			packetLen;
	uint32_t			packetCRC;

	crc_status_t		crcStatus;
}bl_update_packet_t;

typedef struct
{
    /* --- Core state --- */
    bl_state_t   					state;
    bl_update_state_t				updateState;
    bl_error_t   					error;

    uint32_t     					tick_start;
    uint32_t     					boot_elapsed_ms;

    /* --- Update flags --- */
    bool         					update_requested;
    bool         					update_in_progress;

    /* --- Application info --- */
    uint32_t     					app_base;
    bool         					app_valid;

    /* --- Firmware transfer  --- */
    bl_update_info_t 				update_info;
    bl_update_request_packet_info_t	update_packet_info;
    bl_update_packet_t				update_packet;

    /* --- Debug / diagnostics --- */
    uint32_t     					last_event;
    uint32_t     					reset_reason;
} BootloaderCtx_t;

/* =========================================================
 * Public API
 * ========================================================= */

/**
 * @brief Bootloader context initialization
 */
void Bootloader_Init(BootloaderCtx_t *ctx);

/**
 * @brief Bootloader main control task
 */
void Bootloader_Task(BootloaderCtx_t *ctx);

/**
 * @brief Try to jump to application
 */
bool Bootloader_JumpToApplication(BootloaderCtx_t *ctx);

void Bootloader_Packet_Parser(bl_update_packet_t *ctxPacket, uint8_t *buff, uint16_t len);

#endif /* BOOTLOADER_DRIVERS_BOOT_DRIVER_INC_BOOTLOADER_DRIVER_H_ */
