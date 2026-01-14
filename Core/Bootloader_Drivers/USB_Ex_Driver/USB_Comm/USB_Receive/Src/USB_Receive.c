/*
 * USB_Receive.c
 *
 *  Created on: Agust 06, 2025
 *      Author: Inanc Mert Altun
 */

#include "USB_Receive.h"
#include "usbd_cdc_if.h"

extern USBCommParameters_t USB_Comm_Parameters;

void USB_Rx_Wait_Packet_Function(void);
void USB_Rx_Header_Control_Function(void);
void USB_Rx_Packet_Type_Control_Function(void);
void USB_Rx_Command_ID_Control_Function(void);
void USB_Rx_Process_Type_Control_Function(void);
void USB_Rx_Data_Length_Control_Function(void);
void USB_Rx_Data_Control_Function(void);
void USB_Rx_Checksum_Control_Function(void);
void USB_Rx_Stop_Bit_Control_Function(void);
void USB_Rx_Operation_Function(USBCommParameters_t *USB_Comm_ParametersLocal);
static void USB_Rx_Packet_Reset(void);


void System_USB_Communication_Receive_Function(USBCommParameters_t *USB_Comm_ParametersLocal)
{
    switch (USB_Comm_Parameters.USB_rx_parameters.device_rx_state)
    {
    case USB_RX_WAIT_PACKET_STATE:
        USB_Rx_Wait_Packet_Function();
        break;
    case USB_RX_HEADER_CONTROL_STATE:
        USB_Rx_Header_Control_Function();
        break;
    case USB_RX_PACKET_TYPE_CONTROL_STATE:
        USB_Rx_Packet_Type_Control_Function();
        break;
    case USB_RX_COMMAND_ID_CONTROL_STATE:
        USB_Rx_Command_ID_Control_Function();
        break;
    case USB_RX_PROCESS_TYPE_CONTROL_STATE:
        USB_Rx_Process_Type_Control_Function();
        break;
    case USB_RX_DATA_LENGTH_CONTROL_STATE:
        USB_Rx_Data_Length_Control_Function();
        break;
    case USB_RX_DATA_CONTROL_STATE:
        USB_Rx_Data_Control_Function();
        break;
    case USB_RX_CHECKSUM_CONTROL_STATE:
        USB_Rx_Checksum_Control_Function();
        break;
    case USB_RX_STOP_BIT_CONTROL_STATE:
        USB_Rx_Stop_Bit_Control_Function();
        break;
    case USB_RX_OPERATION_STATE:
        USB_Rx_Operation_Function(USB_Comm_ParametersLocal);
        break;

    default:
        break;
    }
}

void USB_Rx_Wait_Packet_Function(void)
{
    if (!(USB_Comm_Parameters.USB_rx_parameters.usbRxFlag) &&
        !(USB_Comm_Parameters.USB_rx_parameters.usbRxBufLen))
    {

        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    }
    else
    {
    	USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_HEADER_CONTROL_STATE;
    }
}

void USB_Rx_Header_Control_Function(void)
{
    if(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_1_HEADER_1] != USB_PACKET_HEADER_1 && 
       USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_2_HEADER_2] != USB_PACKET_HEADER_2)
    {
        USB_Rx_Packet_Reset();
        USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_HEADER;
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    }
    else
    {
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_PACKET_TYPE_CONTROL_STATE;
    }   
}

void USB_Rx_Packet_Type_Control_Function(void)
{
    
    if(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_3_PACKET_TYPE] != USB_PACKET_PACKET_TYPE_TEST &&
       USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_3_PACKET_TYPE] != USB_PACKET_PACKET_TYPE_CONFIG &&
	   USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_3_PACKET_TYPE] != USB_PACKET_PACKET_FLASH &&
	   USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_3_PACKET_TYPE] != USB_PACKET_FIRMWARE_UPDATE &&
	   USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_3_PACKET_TYPE] != USB_PACKET_PACKET_FLASH_DEBUG)
    {
        USB_Rx_Packet_Reset();
        USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_PACKET_TYPE;
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    }
    else
    {
        USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_3_PACKET_TYPE];
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_COMMAND_ID_CONTROL_STATE;
    }
}

void USB_Rx_Command_ID_Control_Function(void)
{
    switch (USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type)
    {
    case USB_PACKET_PACKET_TYPE_TEST:
        switch (USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID])
        {
            case USB_TEST_COMMAND_ID_LSM6DSOX:
            case USB_TEST_COMMAND_ID_ADS1192:
            case USB_TEST_COMMAND_ID_AT24C32:
            case USB_TEST_COMMAND_ID_MAX17303:
            case USB_TEST_COMMAND_ID_BLE:
            case USB_TEST_COMMAND_ID_LED:
            case USB_TEST_COMMAND_ID_VIBRATION:
            case USB_TEST_COMMAND_ID_BATTERY_AVG_VCELL_MV:
            case USB_TEST_COMMAND_ID_BATTERY_AVG_CURRENT_MA:
            case USB_TEST_COMMAND_ID_BATTERY_TEMP_C:
            case USB_TEST_COMMAND_ID_BATTERY_SOC_PERCENT:
            case USB_TEST_COMMAND_ID_BATTERY_REP_CAPACITY_MAH:
            case USB_TEST_COMMAND_ID_BATTERY_FULL_CAPACITY_MAH:
            case USB_TEST_COMMAND_ID_BATTERY_TTE_MINUTES:
            case USB_TEST_COMMAND_ID_BATTERY_TTF_MINUTES:
            case USB_TEST_COMMAND_ID_BATTERY_CYCLE_COUNT:
            case USB_TEST_COMMAND_ID_BATTERY_SOH_PERCENT:
            case USB_TEST_COMMAND_ID_FLASH_CLEAR:
            case USB_TEST_COMMAND_ID_ECG_STREAM:
            case USB_TEST_COMMAND_ID_FLASH_TEST:
            case USB_TEST_COMMAND_ID_SOFTWARE_RESET:


            	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.command.USB_test_command_id = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID];
                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_PROCESS_TYPE_CONTROL_STATE;
            break;
        
            default:
                USB_Rx_Packet_Reset();
                USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_INVALID_TEST_COMMAND_ID;
                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
            break;
        }
    break;

    case USB_PACKET_PACKET_TYPE_CONFIG:
        switch (USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID])
        {
            case USB_COMMAND_ID_RTC:
            case USB_COMMAND_ID_FIRMWARE_VERSION:
            case USB_COMMAND_ID_HARDWARE_VERSION:
            case USB_COMMAND_ID_DEVICE_NAME:
            case USB_COMMAND_ID_DEVICE_NUMBER:
            case USB_COMMAND_ID_SERIAL_NUMBER:
            case USB_COMMAND_ID_MANUFACTORING_DATE:
            case USB_COMMAND_ID_BATCH_NUMBER:
            case USB_COMMAND_ID_LAST_SERVICE_DATE:
            case USB_COMMAND_ID_PATIENT_NAME:
            case USB_COMMAND_ID_BATTERY_DESIGN_CAPACITY_MAH:
            case USB_COMMAND_ID_BATTERY_RSENSE_MOHM:
            case USB_COMMAND_ID_DEVICE_TYPE:
            case USB_COMMAND_ID_BLE_BAUDRATE:
            case USB_COMMAND_ID_BLE_NAME:
            case USB_COMMAND_ID_BLE_PHY_MODE:
            case USB_COMMAND_ID_BLE_SERIAL_NUMBER:
            case USB_COMMAND_ID_BLE_INDICATOR:
            case USB_COMMAND_ID_DEVICE_SAMPLING_FREQUENCY:
            case USB_COMMAND_ID_HPF_CUTOFF_FREQUENCY:
            case USB_COMMAND_ID_LPF_CUTOFF_FREQUENCY:
            case USB_COMMAND_ID_DEVICE_NOTCH_FREQUENCY:
            case USB_COMMAND_ID_DEVICE_NOTCH_Q_VALUE:
            case USB_COMMAND_ID_DEVICE_CHANGE_BAUDRATE:
            case USB_COMMAND_ID_DEVICE_UPDATE_REQUEST:

            	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.command.USB_config_command_id = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID];
                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_PROCESS_TYPE_CONTROL_STATE;

            break;

            default:

                USB_Rx_Packet_Reset();
                USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_INVALID_CONFIG_COMMAND_ID;
                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

            break;
        }
    break;
    case USB_PACKET_PACKET_FLASH:
    case USB_PACKET_PACKET_FLASH_DEBUG:
        switch (USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID])
        {
            case USB_COMMAND_ID_LIST_RECORD_COUNT_AND_PAGE:
            case USB_COMMAND_ID_RECORD_REQUEST:
            case USB_COMMAND_ID_RECORD_FEEDBACK:
            case USB_COMMAND_ID_RECORD_FINISH:
            case USB_COMMAND_ID_RECORD_RECOVER:
            case USB_COMMAND_ID_RECORD_CANCEL:
            case USB_COMMAND_ID_FLASH_HEALTH_CONTROL:

            	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.command.USB_flash_command_id = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID];
                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_PROCESS_TYPE_CONTROL_STATE;
            break;

            default:
                USB_Rx_Packet_Reset();
                USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_INVALID_CONFIG_COMMAND_ID;
                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
            break;
        }

        case USB_PACKET_FIRMWARE_UPDATE:
        	switch (USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID])
        	{
        		case USB_FIRMWARE_UPDATE_STATUS_REQ:
				case USB_FIRMWARE_UPDATE_READY:
				case USB_FIRMWARE_UPDATE_PACKET_INFO:
				case USB_FIRMWARE_UPDATE_SEND_PACKET:

	            	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.command.USB_firmware_update_command_id = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_4_COMMAND_ID];
	                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_PROCESS_TYPE_CONTROL_STATE;

					break;
				default:

	                USB_Rx_Packet_Reset();
	                USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_INVALID_CONFIG_COMMAND_ID;
	                USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

					break;
        	}
        	break;
    default:
        break;
    }
}

void USB_Rx_Process_Type_Control_Function(void)
{
    if(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_5_PROCESS_TYPE] != USB_PACKET_PROCESS_TYPE_READ &&
        USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_5_PROCESS_TYPE] != USB_PACKET_PROCESS_TYPE_WRITE)
    {
            USB_Rx_Packet_Reset();
            USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_INVALID_PROCESS_TYPE;
            USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    }
    else
    {
    	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.process_type = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_5_PROCESS_TYPE];
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_DATA_LENGTH_CONTROL_STATE;
    }
}

void USB_Rx_Data_Length_Control_Function(void)
{
    if(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_6_DATA_LEN_MSB] == 0 && USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_7_DATA_LEN_LSB] == 0)
    {
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_CHECKSUM_CONTROL_STATE;
    }
    else
    {
    	if((USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_6_DATA_LEN_MSB] + 9) > USB_MAX_BUFFER_LEN)
    	{
            USB_Rx_Packet_Reset();
            USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_INVALID_DATA_LEN;
            USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    	}
    	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data_len = (uint16_t)(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_6_DATA_LEN_MSB] << 8) | USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_7_DATA_LEN_LSB];
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_DATA_CONTROL_STATE;
    }
}

void USB_Rx_Data_Control_Function(void)
{
    for(uint16_t i = 0; i < USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data_len; i++)
    {
    	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data[i] = USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_DATA_START + i];
    }

    USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_CHECKSUM_CONTROL_STATE;
}

void USB_Rx_Checksum_Control_Function(void)
{
    for(uint16_t i = USB_INDEX_3_PACKET_TYPE; i < (USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data_len + USB_INDEX_DATA_START); i++)
    {
    	USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.checksum = USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.checksum ^ USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[i];
    }

    if(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[USB_INDEX_DATA_START + USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data_len] != USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.checksum)
    {
        USB_Rx_Packet_Reset();
        USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_CHECKSUM;
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    }
    else
    {
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_STOP_BIT_CONTROL_STATE;
    }
}

void USB_Rx_Stop_Bit_Control_Function(void)
{
    if(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[6 + USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data_len + 2] != USB_PACKET_FOOTER_1 &&
       USB_Comm_Parameters.USB_rx_parameters.usbRxBuf[6 + USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.data_len + 3] != USB_PACKET_FOOTER_2)
    {
        USB_Rx_Packet_Reset();
        USB_Comm_Parameters.USB_rx_parameters.USB_packet_error |= USB_PACKET_ERROR_FOOTER;
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;
    }
    else
    {
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_OPERATION_STATE;
    }
}

void USB_Rx_Operation_Function(USBCommParameters_t *USB_Comm_ParametersLocal)
{
    if(USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_PACKET_TYPE_TEST)
    {
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

        USB_Rx_Packet_Reset();
    }
    else if(USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_PACKET_TYPE_CONFIG)
    {
    	USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

    	USB_Rx_Packet_Reset();
    }
    else if(USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_PACKET_FLASH)
    {
    	USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

    	USB_Rx_Packet_Reset();
    }
    else if(USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_PACKET_FLASH_DEBUG)
    {
        USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

    	USB_Rx_Packet_Reset();        
    }
    else if(USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info.packet_type == USB_PACKET_FIRMWARE_UPDATE)
    {
    	memcpy((void *)USB_Comm_ParametersLocal, (const void *)&USB_Comm_Parameters, sizeof(USB_Comm_Parameters));

    	USB_Comm_Parameters.USB_rx_parameters.device_rx_state = USB_RX_WAIT_PACKET_STATE;

    	USB_Rx_Packet_Reset();
    }
}


static void USB_Rx_Packet_Reset(void)
{
    memset(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf, 0 , sizeof(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf));
    memset(&USB_Comm_Parameters.USB_rx_parameters.USB_rx_packet_info, 0 , sizeof(USBRxPacketInfo_t));
    USB_Comm_Parameters.USB_rx_parameters.usbRxFlag = 0;
    USB_Comm_Parameters.USB_rx_parameters.usbRxBufLen = 0;
}

static int32_t find_header_index(const uint8_t *buf, uint32_t len)
{
    if (len < 2u) return -1;

    for (uint32_t i = 0; i < (len - 1u); i++)
    {
        if ((buf[i] == USB_PACKET_HEADER_1) && (buf[i + 1u] == USB_PACKET_HEADER_2))
        {
            return (int32_t)i;
        }
    }
    return -1;
}

volatile USB_RxDebug_t g_usb_rx_debug = {0};


void USB_RXCallback(uint8_t *buf, uint32_t *len)
{
	/*
	 * ESKİ HALİ
	 */
	/*
    memcpy(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf, buf, *len);
    USB_Comm_Parameters.USB_rx_parameters.usbRxBufLen = *len;
    USB_Comm_Parameters.USB_rx_parameters.usbRxFlag = 1;
    */
	uint32_t rxLen 	= *len;
	static uint16_t msgLen = 0;

	if((buf == NULL) || len == NULL)
		return;

	g_usb_rx_debug.rx_callback_count += 1;
	g_usb_rx_debug.last_rx_len		  = rxLen;

	memcpy(&g_usb_rx_debug.raw_bytes[g_usb_rx_debug.expected_frame_len], buf, rxLen);
	g_usb_rx_debug.expected_frame_len += g_usb_rx_debug.last_rx_len;

	if(g_usb_rx_debug.frame_in_progress == 0)
	{
		if(find_header_index(g_usb_rx_debug.raw_bytes, rxLen) != -1)
		{
			// Header'lı veri
			g_usb_rx_debug.frame_in_progress = 1;

			msgLen = (g_usb_rx_debug.raw_bytes[5] << 8) | (g_usb_rx_debug.raw_bytes[6]);
		}
	}

	if((msgLen + 10) == g_usb_rx_debug.expected_frame_len)
	{
		// Verinin tamamı gelmiştir işlenebilir.
		g_usb_rx_debug.total_received_bytes = g_usb_rx_debug.expected_frame_len;
		g_usb_rx_debug.frame_in_progress 	= 0;
		g_usb_rx_debug.frame_completed 	 	= 1;
	}
	else if((msgLen + 10) < g_usb_rx_debug.expected_frame_len)
	{
		g_usb_rx_debug.frame_completed 		= 0;
		g_usb_rx_debug.rx_callback_count 	= 0;
		g_usb_rx_debug.expected_frame_len 	= 0;
		g_usb_rx_debug.frame_in_progress    = 0;
		g_usb_rx_debug.last_rx_len			= 0;
	}

	if(g_usb_rx_debug.frame_completed)
	{
		memcpy(USB_Comm_Parameters.USB_rx_parameters.usbRxBuf,
			   g_usb_rx_debug.raw_bytes,
			   g_usb_rx_debug.total_received_bytes);

	    USB_Comm_Parameters.USB_rx_parameters.usbRxBufLen 	= g_usb_rx_debug.total_received_bytes;
	    USB_Comm_Parameters.USB_rx_parameters.usbRxFlag 	= 1;

		g_usb_rx_debug.frame_completed 		= 0;
		g_usb_rx_debug.rx_callback_count 	= 0;
		g_usb_rx_debug.expected_frame_len 	= 0;
		g_usb_rx_debug.frame_in_progress    = 0;
	}
}






