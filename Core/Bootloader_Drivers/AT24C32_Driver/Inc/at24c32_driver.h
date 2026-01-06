/*
 * at24c32_driver.h
 *
 *  Created on: Jul 30, 2025
 *      Author: Fatih
 */

#ifndef LW_DRIVERS_AT24C32_DRIVER_INC_AT24C32_DRIVER_H_
#define LW_DRIVERS_AT24C32_DRIVER_INC_AT24C32_DRIVER_H_

#include "main.h"
#include <stdbool.h>
#include "at24c32_address.h"

#define AT24C32_EEPROM_VCC_ENABLE			HAL_GPIO_WritePin(MCU_EEPROM_VCC_ENABLE_GPIO_Port,     MCU_EEPROM_VCC_ENABLE_Pin, 	GPIO_PIN_SET)
#define AT24C32_EEPROM_VCC_DISABLE			HAL_GPIO_WritePin(MCU_EEPROM_VCC_ENABLE_GPIO_Port,     MCU_EEPROM_VCC_ENABLE_Pin, 	GPIO_PIN_RESET)

#define AT24C32_EEPROM_WP_ACTIVE			HAL_GPIO_WritePin(MCU_EEPROM_WP_GPIO_Port,     			MCU_EEPROM_WP_Pin, 			GPIO_PIN_SET)
#define AT24C32_EEPROM_WP_DEACTIVE			HAL_GPIO_WritePin(MCU_EEPROM_WP_GPIO_Port,     			MCU_EEPROM_WP_Pin, 			GPIO_PIN_RESET)

#define AT24C32_I2C_ADDRESS_WRITE   		(0xA8U)		// AT24C32 EEPROM I2C yazma adresi
#define AT24C32_I2C_ADDRESS_READ    		(0xA9U)		// AT24C32 EEPROM I2C okuma adresi

#define AT24C32_TOTAL_SIZE_BYTES    		(4096U)		// AT24C32 EEPROM toplam kapasite 	(bayt cinsinden)
#define AT24C32_PAGE_SIZE_BYTES     		(32U)		// AT24C32 EEPROM sayfa boyutu 		(bayt cinsinden)

#define AT24C32_WRITE_TIMEOUT_MS    		(100U)      // I²C Mem_Write timeout */
#define AT24C32_READ_TIMEOUT_MS     		(100U)      // I²C Mem_Read  timeout */

#define AT24C32_READY_TRIALS            	(5U)        // HAL_I2C_IsDeviceReady deneme sayısı */
#define AT24C32_READY_TIMEOUT_MS        	(10U)       // Her deneme için timeout (ms) */
#define AT24C32_READY_OVERALL_TIMEOUT_MS 	(100U)      // Toplam polling için overall timeout (ms) */

#define AT24C32_WRITE_CYCLE_MS          	(10U)       // EEPROM page-write completion delay */

typedef struct
{
    I2C_HandleTypeDef 	*i2c_handle;
    uint8_t 			device_address;
} S_AT24C32_t;

HAL_StatusTypeDef AT24C32_Initialization(S_AT24C32_t *at24c32, I2C_HandleTypeDef *hi2c);
HAL_StatusTypeDef AT24C32_WriteData(S_AT24C32_t *at24c32, uint16_t address, uint8_t * data, uint16_t length);
HAL_StatusTypeDef AT24C32_WriteU32(S_AT24C32_t *at24c32, uint16_t address, uint32_t value);
HAL_StatusTypeDef AT24C32_WriteMultipleData(S_AT24C32_t *at24c32, uint16_t address, uint8_t * data, uint16_t length);
HAL_StatusTypeDef AT24C32_ReadData(S_AT24C32_t *at24c32, uint16_t address, uint8_t * data, uint16_t length);
HAL_StatusTypeDef AT24C32_ReadU32(S_AT24C32_t *at24c32, uint16_t address, uint32_t *value);
HAL_StatusTypeDef AT24C32_ReadMultipleData(S_AT24C32_t *at24c32, uint16_t address, uint8_t * data, uint16_t length);
HAL_StatusTypeDef AT24C32_EraseAll(S_AT24C32_t *at24c32);
HAL_StatusTypeDef AT24C32_IsDeviceReady(S_AT24C32_t *at24c32);

#endif /* LW_DRIVERS_AT24C32_DRIVER_INC_AT24C32_DRIVER_H_ */
