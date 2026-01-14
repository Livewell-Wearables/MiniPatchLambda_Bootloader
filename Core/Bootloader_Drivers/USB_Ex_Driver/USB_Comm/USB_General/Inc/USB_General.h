/*
 * USB_General.h
 *
 *  Created on: Agust 06, 2025
 *      Author: Inanc Mert Altun
 */

#ifndef LW_USB_COMM_H_
#define LW_USB_COMM_H_

#include "main.h"
#include <stdint.h>
#include <string.h>
#include <stddef.h>

/* Package Structure */
/*
       0xAA,       0x55,         XX,            XX             XX              XX              XX          XX       CRC,        0x55        0xAA 
    |Header 1|  |Header 2|  |Packet Type|  |Command ID|  |Process Type|  |Data Len MSB|  |Data Len LSB|  |Data|  |Checksum|  |Footer 1|  |Footer 2|

*/

/*---------------- Packet Type -------------------*/
/*

    USB_PACKET_PACKET_TYPE_TEST         	-> 0x52,
    USB_PACKET_PACKET_TYPE_CONFIG       	-> 0x57,
	USB_PACKET_PACKET_FLASH			    	-> 0x58,
*/
/*-------------------------------------------------*/


/*------------------ Command ID -------------------*/
/*

    //Test için

    USB_COMMAND_ID_LSM6DSOX                 -> 0x10 
    USB_COMMAND_ID_ADS1192                  -> 0x11 
    USB_COMMAND_ID_AT24C32                  -> 0x12 //eproom
    USB_COMMAND_ID_RT9426                   -> 0x13 
    USB_COMMAND_ID_RTC                      -> 0x14 
    USB_COMMAND_ID_LED                      -> 0x15 
    USB_COMMAND_ID_SD_CARD                  -> 0x16 
    USB_COMMAND_ID_VIBRATION                -> 0x17 


    //Config için

    USB_COMMAND_ID_RTC                      -> 0x40
    USB_COMMAND_ID_FIRMWARE_VERSION         -> 0x41
    USB_COMMAND_ID_HARDWARE_VERSION         -> 0x42
    USB_COMMAND_ID_DEVICE_MODEL             -> 0x43
    USB_COMMAND_ID_DEVICE_NUMBER            -> 0x44
    USB_COMMAND_ID_SERIAL_NUMBER            -> 0x45
    USB_COMMAND_ID_MANUFACTORING_DATE       -> 0x46
    USB_COMMAND_ID_BATCH_NUMBER             -> 0x47
    USB_COMMAND_ID_LAST_SERVICE_DATE        -> 0x48
    USB_COMMAND_ID_PATIENT_NAME             -> 0x49
    USB_COMMAND_ID_BATTERY_CYCLE_COUNT      -> 0x4A
    USB_COMMAND_ID_BATTERY_STATE_OF_HEALTH  -> 0x4B
    USB_COMMAND_ID_BATTERY_STATE_OF_CHARGE  -> 0x4C
    USB_COMMAND_ID_BATTERY_CAPACITY         -> 0x4D
    


    //Flash
	USB_COMMAND_ID_RECORD_COUNT_AND_PAGE				= 0x60,
	USB_COMMAND_ID_RECORD_REQUEST,
	USB_COMMAND_ID_RECORD_ACK,
	USB_COMMAND_ID_RECORD_NACK,
	USB_COMMAND_ID_RECORD_FINISH
    ...
    ...
    ...
    ...
*/
/*--------------------------------------------------*/

/*---------------- Process Type --------------------*/
/*
    USB_PROCESS_TYPE_READ   ->0x01
    USB_PROCESS_TYPE_WRITE  ->0x02
*/
/*--------------------------------------------------*/

#define USB_DATA_PAGE_COUNT							1

#define USB_MAX_BUFFER_LEN          				5000 * USB_DATA_PAGE_COUNT

#define USB_INDEX_1_HEADER_1                        0
#define USB_INDEX_2_HEADER_2                        1
#define USB_INDEX_3_PACKET_TYPE                     2
#define USB_INDEX_4_COMMAND_ID                      3
#define USB_INDEX_5_PROCESS_TYPE                    4
#define USB_INDEX_6_DATA_LEN_MSB                    5
#define USB_INDEX_7_DATA_LEN_LSB                    6
#define USB_INDEX_DATA_START                        7
#define USB_CONSTANT_PACKET_VALUES_FOR_CHECKSUM     5
#define USB_OVERHEAD_BYTES  						10u   /* 2(H) +3(type/cmd/proc)+2(len)+1(crc)+2(F) */


#define SUCCESSFULL 	0
#define FAILED  		1		// MCU tarafındaki hata
#define WRONG 			2		// Yanlış paket hatası


typedef enum
{
    USB_PACKET_HEADER_1     = 0xAA,
    USB_PACKET_HEADER_2     = 0x55,
}USBPacketHeader_t;

typedef enum
{
    USB_PACKET_PACKET_TYPE_TEST         = 0x52,
    USB_PACKET_PACKET_TYPE_CONFIG       = 0x57,
	USB_PACKET_PACKET_FLASH			    = 0x58,
    USB_PACKET_PACKET_FLASH_DEBUG       = 0x59,
	USB_PACKET_FIRMWARE_UPDATE			= 0x60,
}USBPacketPacketType_t;

typedef enum
{
	USB_FIRMWARE_UPDATE_STATUS_REQ		= 0x10,		// PC  - - - > MCU
	USB_FIRMWARE_UPDATE_READY			= 0x11,		// MCU - - - > PC
	USB_FIRMWARE_UPDATE_PACKET_INFO		= 0x12,		// PC  - - - > MCU
	USB_FIRMWARE_UPDATE_GET_PACKET		= 0x13, 	// MCU - - - > PC
	USB_FIRMWARE_UPDATE_SEND_PACKET		= 0x14,     // PC  - - - > MCU
	USB_FIRMWARE_UPDATE_VERIFY_PACKET	= 0x15,     // MCU - - - > PC
}USBFirmwareUpdateCommandID_t;

typedef enum
{
    USB_TEST_COMMAND_ID_LSM6DSOX        = 0x10,
    USB_TEST_COMMAND_ID_ADS1192,
    USB_TEST_COMMAND_ID_AT24C32,
    USB_TEST_COMMAND_ID_MAX17303,
    USB_TEST_COMMAND_ID_BLE,
    USB_TEST_COMMAND_ID_LED,
    USB_TEST_COMMAND_ID_VIBRATION,
    USB_TEST_COMMAND_ID_BATTERY_AVG_VCELL_MV,
    USB_TEST_COMMAND_ID_BATTERY_AVG_CURRENT_MA,
    USB_TEST_COMMAND_ID_BATTERY_TEMP_C,
    USB_TEST_COMMAND_ID_BATTERY_SOC_PERCENT,
    USB_TEST_COMMAND_ID_BATTERY_REP_CAPACITY_MAH,
    USB_TEST_COMMAND_ID_BATTERY_FULL_CAPACITY_MAH,
    USB_TEST_COMMAND_ID_BATTERY_TTE_MINUTES,
    USB_TEST_COMMAND_ID_BATTERY_TTF_MINUTES,
    USB_TEST_COMMAND_ID_BATTERY_CYCLE_COUNT,
	USB_TEST_COMMAND_ID_BATTERY_SOH_PERCENT,
    USB_TEST_COMMAND_ID_FLASH_CLEAR,
	USB_TEST_COMMAND_ID_ECG_STREAM,
    USB_TEST_COMMAND_ID_FLASH_TEST,
    USB_TEST_COMMAND_ID_SOFTWARE_RESET
}USBTestCommandID_t;

typedef enum
{
    USB_COMMAND_ID_RTC                          = 0x40,
    USB_COMMAND_ID_FIRMWARE_VERSION,
    USB_COMMAND_ID_HARDWARE_VERSION,
    USB_COMMAND_ID_DEVICE_NAME,
    USB_COMMAND_ID_DEVICE_NUMBER,
    USB_COMMAND_ID_SERIAL_NUMBER,
    USB_COMMAND_ID_MANUFACTORING_DATE,
    USB_COMMAND_ID_BATCH_NUMBER,
    USB_COMMAND_ID_LAST_SERVICE_DATE,
    USB_COMMAND_ID_PATIENT_NAME,
    USB_COMMAND_ID_BATTERY_DESIGN_CAPACITY_MAH,
    USB_COMMAND_ID_BATTERY_RSENSE_MOHM,
    USB_COMMAND_ID_DEVICE_TYPE,
    USB_COMMAND_ID_BLE_BAUDRATE,
    USB_COMMAND_ID_BLE_NAME,
    USB_COMMAND_ID_BLE_PHY_MODE,
    USB_COMMAND_ID_BLE_SERIAL_NUMBER,
    USB_COMMAND_ID_BLE_INDICATOR,
    USB_COMMAND_ID_DEVICE_SAMPLING_FREQUENCY,
    USB_COMMAND_ID_HPF_CUTOFF_FREQUENCY,
    USB_COMMAND_ID_LPF_CUTOFF_FREQUENCY,
    USB_COMMAND_ID_DEVICE_NOTCH_FREQUENCY,
	USB_COMMAND_ID_DEVICE_NOTCH_Q_VALUE,
    USB_COMMAND_ID_DEVICE_CHANGE_BAUDRATE,
    USB_COMMAND_ID_DEVICE_UPDATE_REQUEST
}USBConfigCommandID_t;

typedef enum
{
	USB_COMMAND_ID_LIST_RECORD_COUNT_AND_PAGE	= 0x60,	/* Flash içi sayfaları listeleme ve 16 ya kadar bilgiyi raporlama */
	USB_COMMAND_ID_RECORD_REQUEST,						/* */
	USB_COMMAND_ID_RECORD_STREAM_DATA,					/* */
	USB_COMMAND_ID_RECORD_FEEDBACK,						/* OK or NOK */
	USB_COMMAND_ID_RECORD_FINISH,
	USB_COMMAND_ID_RECORD_RECOVER,
    USB_COMMAND_ID_RECORD_CANCEL,
    USB_COMMAND_ID_FLASH_HEALTH_CONTROL
}USBRecordCommandID_t;

typedef struct
{
    USBTestCommandID_t 				USB_test_command_id;
    USBConfigCommandID_t 			USB_config_command_id;
    USBRecordCommandID_t 			USB_flash_command_id;
    USBFirmwareUpdateCommandID_t	USB_firmware_update_command_id;
}USBCommandID_t;

typedef enum
{
    USB_PACKET_PROCESS_TYPE_READ    = 0x01,
    USB_PACKET_PROCESS_TYPE_WRITE   = 0x02,
}USBPacketProcessType_t;

typedef enum
{
    USB_PACKET_FOOTER_1     = 0x55,
    USB_PACKET_FOOTER_2     = 0xAA,
}USBPacketFooter_t;

typedef enum
{
	MCU_BAUDRATE_9600,
	MCU_BAUDRATE_115200,
	MCU_BAUDRATE_230400,
	MCU_BAUDRATE_460800,
	MCU_BAUDRATE_NOTSET
}MCUBaudRate_t;
//******************************************************************//
//******************************************************************//

//***************************TRANSMIT*******************************//
//******************************************************************//
typedef struct
{
    uint8_t status_code;
}USBTxPacketStatusCode_t;


typedef struct 
{
    USBPacketPacketType_t packet_type;
    uint8_t command;
    USBTxPacketStatusCode_t status_code;
    uint16_t data_len;
    uint8_t data[USB_MAX_BUFFER_LEN];
    uint8_t checksum;

}USBTxPacketInfo_t;


typedef struct
{
    uint8_t usbTxBuf[USB_MAX_BUFFER_LEN];
    uint16_t usbTxBufLen;
    USBTxPacketInfo_t USB_Tx_packet_info;
    
}USBTxParameters_t;
//******************************************************************//
//******************************************************************//

//***************************RECEIVE*******************************//
//******************************************************************//
typedef enum
{
    USB_RX_WAIT_PACKET_STATE,
    USB_RX_HEADER_CONTROL_STATE,
    USB_RX_PACKET_TYPE_CONTROL_STATE,
    USB_RX_COMMAND_ID_CONTROL_STATE,
    USB_RX_PROCESS_TYPE_CONTROL_STATE,
    USB_RX_DATA_LENGTH_CONTROL_STATE,
    USB_RX_DATA_CONTROL_STATE,
    USB_RX_CHECKSUM_CONTROL_STATE,
    USB_RX_STOP_BIT_CONTROL_STATE,
    USB_RX_OPERATION_STATE,

    USB_RX_STATE_COUNT
} USBRxDeviceState_t;

typedef enum
{
    USB_PACKET_CORRECT                          = 0x0000,
    USB_PACKET_ERROR_HEADER                     = 1 << 1,
    USB_PACKET_ERROR_PACKET_TYPE                = 1 << 2,
    USB_PACKET_ERROR_INVALID_TEST_COMMAND_ID    = 1 << 3,
    USB_PACKET_ERROR_INVALID_CONFIG_COMMAND_ID  = 1 << 4,
    USB_PACKET_ERROR_INVALID_PROCESS_TYPE       = 1 << 5,
    USB_PACKET_ERROR_INVALID_DATA_LEN           = 1 << 6,
    USB_PACKET_ERROR_CHECKSUM                   = 1 << 7,
    USB_PACKET_ERROR_FOOTER                     = 1 << 8,

} USBPacketErrors_t;

typedef struct
{
    USBPacketPacketType_t packet_type;
    USBCommandID_t command;
    USBPacketProcessType_t process_type;
    uint16_t data_len;
    uint8_t data[USB_MAX_BUFFER_LEN];
    uint8_t checksum;

}USBRxPacketInfo_t;

typedef struct
{
    uint8_t 			usbRxBuf[USB_MAX_BUFFER_LEN];
    uint16_t 			usbRxBufLen;
    uint8_t 			usbRxFlag;
    USBRxDeviceState_t 	device_rx_state;
    USBRxPacketInfo_t 	USB_rx_packet_info;
    USBPacketErrors_t   USB_packet_error;
    uint16_t 			expectedFrameLen;   /* Header + payload + CRC + footer */
    uint8_t  			headerLocked;       /* AA55 bulundu mu? (0/1) */
}USBRxParameters_t;
//******************************************************************//
//******************************************************************//

typedef struct
{
    USBTxParameters_t   USB_tx_parameters;
    USBRxParameters_t   USB_rx_parameters;
}USBCommParameters_t;





#endif /* LW_USB_COMM_H_ */
