/*
 * at24c32_driver.c
 *
 *  Created on: Jul 30, 2025
 *      Author: Fatih
 */

#include "at24c32_driver.h"

HAL_StatusTypeDef AT24C32_Initialization(S_AT24C32_t *at24c32, I2C_HandleTypeDef *hi2c)
{
	HAL_GPIO_WritePin(IMU_VCC_ENABLE_GPIO_Port, 		IMU_VCC_ENABLE_Pin, 		GPIO_PIN_SET);
	HAL_GPIO_WritePin(IMU_CS_GPIO_Port, 				IMU_CS_Pin, 				GPIO_PIN_SET);
	HAL_GPIO_WritePin(MCU_EEPROM_VCC_ENABLE_GPIO_Port, 	MCU_EEPROM_VCC_ENABLE_Pin, 	GPIO_PIN_SET);

	at24c32->i2c_handle 	= hi2c;
	at24c32->device_address	= AT24C32_I2C_ADDRESS_WRITE;

	return AT24C32_IsDeviceReady(at24c32);
}

/**
 * @brief  Writes a sequence of bytes to the AT24C32 EEPROM, handling page boundaries.
 * @param  at24c32  Pointer to initialized EEPROM handle (I2C, device address, etc.).
 * @param  address  16-bit start address in EEPROM memory.
 * @param  data     Pointer to the data buffer to write.
 * @param  length   Number of bytes to write.
 * @param  timeout  Timeout in milliseconds for the entire write operation.
 * @retval HAL_OK       if all pages were written successfully.
 * @retval HAL_TIMEOUT  if the operation timed out before completion.
 * @retval HAL_ERROR    on any I2C or other hardware error.
 */
HAL_StatusTypeDef AT24C32_WriteData( S_AT24C32_t *at24c32,
                                     uint16_t address,
                                     uint8_t * data,
                                     uint16_t length)
{
    uint16_t write_size;
    uint32_t start_time = HAL_GetTick();
    uint32_t timeout = 1000;

    /* 1) Yazma korumayı devre dışı bırak */
    AT24C32_EEPROM_WP_DEACTIVE;

    /* 2) Chunk’lar halinde, sayfa sınırını aşmayarak yaz */
    while (length > 0U)
    {
        write_size = (uint16_t)(AT24C32_PAGE_SIZE_BYTES
                     - (uint16_t)(address % AT24C32_PAGE_SIZE_BYTES));
        if (write_size > length)
        {
            write_size = length;
        }

        /* 2.1) I2C üzerinden page-write */
        if (HAL_I2C_Mem_Write(at24c32->i2c_handle,
                              at24c32->device_address,
                              address,
                              I2C_MEMADD_SIZE_16BIT,
                              (uint8_t *)data,
                              write_size,
                              AT24C32_WRITE_TIMEOUT_MS) != HAL_OK)
        {
            AT24C32_EEPROM_WP_ACTIVE;
            return HAL_ERROR;
        }

        /* 2.2) İç yazma döngüsünün tamamlanmasını ACK-polling ile bekle */
        do
        {
            if (HAL_I2C_IsDeviceReady(at24c32->i2c_handle,
                                      at24c32->device_address,
                                      AT24C32_READY_TRIALS,
                                      AT24C32_READY_TIMEOUT_MS) == HAL_OK)
            {
                break;
            }
        }
        while ((HAL_GetTick() - start_time) < timeout);

        /* 2.3) Zaman aşıldıysa çık */
        if ((HAL_GetTick() - start_time) >= timeout)
        {
            AT24C32_EEPROM_WP_ACTIVE;
            return HAL_TIMEOUT;
        }

        /* 2.4) Offset’i güncelle */
        length  -= write_size;
        data   += write_size;
        address = (uint16_t)(address + write_size);
    }

    /* 3) Yazma korumayı yeniden etkinleştir */
    AT24C32_EEPROM_WP_ACTIVE;
    return HAL_OK;
}

HAL_StatusTypeDef AT24C32_WriteU32(S_AT24C32_t *at24c32,
                                   uint16_t address,
                                   uint32_t value)
{
    HAL_StatusTypeDef status;
    uint8_t buf[4];

    /* 1) Parametre kontrolü */
    if (at24c32 == NULL)
    {
        return HAL_ERROR;
    }

#if defined(AT24C32_SIZE_BYTES)
    if ((uint32_t)address + 4u > (uint32_t)AT24C32_SIZE_BYTES)
    {
        return HAL_ERROR; /* sınır taşması */
    }
#endif

    /* 2) Little-endian olarak parçala */
    buf[0] = (uint8_t)(value & 0xFFu);
    buf[1] = (uint8_t)((value >> 8)  & 0xFFu);
    buf[2] = (uint8_t)((value >> 16) & 0xFFu);
    buf[3] = (uint8_t)((value >> 24) & 0xFFu);

    /* 3) EEPROM’a yaz */
    status = AT24C32_WriteData(at24c32, address, buf, 4u);

    return status;
}

/**
 * @brief  Write a contiguous block of bytes to an AT24C32 EEPROM.
 *
 * Splits the payload at page boundaries (AT24C32: 32 bytes/page) and performs
 * ACK polling (HAL_I2C_IsDeviceReady) after each page program until the device
 * becomes ready or the timeout expires.
 *
 * @param[in,out] at24c32  Pointer to AT24C32 driver context (uses i2c_handle and device_address).
 * @param[in]     address  16-bit internal EEPROM address (0x0000..0x0FFF).
 * @param[in]     data     Pointer to the buffer holding bytes to be written.
 * @param[in]     length   Number of bytes to write.
 *
 * @retval HAL_OK          All bytes written successfully.
 * @retval HAL_ERROR       Underlying HAL I2C error occurred.
 * @retval HAL_BUSY        I2C peripheral busy.
 * @retval HAL_TIMEOUT     Device did not become ready within the timeout.
 *
 * @pre  at24c32->device_address shall be the 7-bit device address left-shifted by 1
 *       (e.g. 0x50 << 1). Write-protect (WP) must be deasserted and VCC enabled.
 * @note This is a blocking call. The function does not validate address range;
 *       the caller must ensure (address + length) does not exceed device size.
 * @note Page size is taken from AT24C32_PAGE_SIZE_BYTES; using a wrong value
 *       may lead to page wrap and corrupted data.
 */
HAL_StatusTypeDef AT24C32_WriteMultipleData( S_AT24C32_t *at24c32,
											 uint16_t address,
											 uint8_t * data,
											 uint16_t length)
{
    HAL_StatusTypeDef ret 		= HAL_OK;
    uint32_t 		  timeout 	= 1000;

    while (length > 0)
    {
        // Bu sayfada kaç bayt yazabiliriz?
        uint8_t pageSpace = (uint8_t)(AT24C32_PAGE_SIZE_BYTES - (address % AT24C32_PAGE_SIZE_BYTES));
        uint8_t chunk     = (length < pageSpace) ? (uint8_t)length : pageSpace;

        // [memAddrHigh][memAddrLow][data...]
        uint8_t tx[2 + 32]; // AT24C32 için max 2 + 32 yeter; istersen dinamik ayır
        tx[0] = (uint8_t)((address >> 8) & 0xFF);
        tx[1] = (uint8_t)(address & 0xFF);
        for (uint8_t i = 0; i < chunk; i++)
            tx[2 + i] = data[i];

        ret = HAL_I2C_Master_Transmit(at24c32->i2c_handle,
        							  at24c32->device_address,
									  tx,
									  (uint16_t)(2 + chunk),
									  HAL_MAX_DELAY);
        if (ret != HAL_OK)
            return ret;

        // Yazma çevrimi (tWR) tamamlanana kadar ACK polling
        // (sabit gecikme yerine daha sağlıklı)
        uint32_t pollStart = HAL_GetTick();
        do {
            ret = HAL_I2C_IsDeviceReady(at24c32->i2c_handle,
					  	  	  	  	    at24c32->device_address,
										1,
										100);

            if (ret == HAL_OK)
            {
            	break;
            }

        } while ((HAL_GetTick() - pollStart) < timeout);

        if (ret != HAL_OK)
        {
        	return ret;
        }

        // İlerlet
        address += chunk;
        data    += chunk;
        length  -= chunk;
    }

    return HAL_OK;
}

/**
 * @brief  Reads a sequence of bytes from the AT24C32 EEPROM, waiting first for any prior
 *         write cycle to complete before issuing the read.
 * @param[in]  at24c32  Pointer to initialized EEPROM handle (I2C handle & 7-bit device address).
 * @param[in]  address  16-bit start address in EEPROM memory.
 * @param[out] data     Pointer to buffer where read data will be stored.
 * @param[in]  length   Number of bytes to read.
 * @retval HAL_OK       if the read operation completed successfully.
 * @retval HAL_ERROR    if a transmission error occurred or parameters are invalid.
 * @retval HAL_TIMEOUT  if the device did not become ready before the polling timeout.
 */
HAL_StatusTypeDef AT24C32_ReadData( S_AT24C32_t * at24c32,
                                    uint16_t address,
                                    uint8_t * data,
                                    uint16_t length )
{
    HAL_StatusTypeDef status = HAL_ERROR;
    uint32_t start = HAL_GetTick();

    /* 1) Parametre kontrolü */
    if ((at24c32 == NULL) || (data == NULL))
    {
        return HAL_ERROR;
    }

    /* 2) Önce varsa devam eden yazma döngüsünün tamamlanmasını bekle */
    do
    {
        status = HAL_I2C_IsDeviceReady( at24c32->i2c_handle,
                                        (uint16_t)(at24c32->device_address),
                                        AT24C32_READY_TRIALS,
                                        AT24C32_READY_TIMEOUT_MS );
        if (status == HAL_OK)
        {
            break;
        }
    }
    while ((HAL_GetTick() - start) < AT24C32_READY_OVERALL_TIMEOUT_MS);

    if (status != HAL_OK)
    {
        return HAL_TIMEOUT;
    }

    /* 3) EEPROM’dan veri oku */
    status = HAL_I2C_Mem_Read( at24c32->i2c_handle,
                               (uint16_t)(at24c32->device_address),
                               address,
                               I2C_MEMADD_SIZE_16BIT,
                               data,
                               length,
                               AT24C32_READ_TIMEOUT_MS );
    if (status != HAL_OK)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef AT24C32_ReadU32(S_AT24C32_t *at24c32,
                                  uint16_t address,
                                  uint32_t *value)
{
    HAL_StatusTypeDef status;
    uint8_t buf[4];

    /* 1) Parametre kontrolü */
    if ((at24c32 == NULL) || (value == NULL))
    {
        return HAL_ERROR;
    }

#if defined(AT24C32_SIZE_BYTES)
    if ((uint32_t)address + 4u > (uint32_t)AT24C32_SIZE_BYTES)
    {
        return HAL_ERROR; /* sınır taşması */
    }
#endif

    /* 2) 4 byte oku */
    status = AT24C32_ReadData(at24c32, address, buf, 4u);
    if (status != HAL_OK)
    {
        return status;
    }

    /* 3) Little-endian formatında birleştir */
    *value  = (uint32_t)buf[0];
    *value |= ((uint32_t)buf[1] << 8);
    *value |= ((uint32_t)buf[2] << 16);
    *value |= ((uint32_t)buf[3] << 24);

    return HAL_OK;
}

/**
 * @brief  Read a contiguous block of bytes from an AT24C32 EEPROM.
 *
 * Sends the 16-bit internal address and performs a sequential read. The payload
 * is fetched in chunks (<= 255 bytes) for robustness; AT24C32 supports seamless
 * sequential reads across page boundaries.
 *
 * @param[in,out] at24c32  Pointer to AT24C32 driver context (uses i2c_handle and device_address).
 * @param[in]     address  16-bit internal EEPROM address (0x0000..0x0FFF).
 * @param[out]    data     Destination buffer to store the bytes read.
 * @param[in]     length   Number of bytes to read.
 *
 * @retval HAL_OK          Data read successfully.
 * @retval HAL_ERROR       Underlying HAL I2C error occurred.
 * @retval HAL_BUSY        I2C peripheral is busy.
 * @retval HAL_TIMEOUT     I2C transaction timed out.
 *
 * @pre  at24c32->device_address shall be the 7-bit device address left-shifted by 1
 *       (e.g., 0x50 << 1). I2C peripheral shall be initialized.
 * @pre  If this read follows a write, ensure the write cycle has completed
 *       (e.g., via ACK polling) before calling this function.
 *
 * @note This is a blocking call. The function does not validate address bounds;
 *       the caller shall ensure (address + length) is within device capacity.
 * @note WP state is irrelevant for reads and is not modified by this function.
 */
HAL_StatusTypeDef AT24C32_ReadMultipleData( S_AT24C32_t *at24c32,
											 uint16_t address,
											 uint8_t * data,
											 uint16_t length)
{
    HAL_StatusTypeDef ret;
    uint8_t addr[2];

    while (length > 0)
    {
        uint16_t chunk = (length > 255U) ? 255U : length;

        // İç adresi gönder (Repeated-Start ile devamında okuma yapılacak)
        addr[0] = (uint8_t)((address >> 8) & 0xFF);
        addr[1] = (uint8_t)(address & 0xFF);

        ret = HAL_I2C_Master_Transmit(at24c32->i2c_handle, at24c32->device_address, addr, 2, HAL_MAX_DELAY);
        if (ret != HAL_OK)
        {
        	return ret;
        }

        ret = HAL_I2C_Master_Receive(at24c32->i2c_handle, at24c32->device_address, data, chunk, HAL_MAX_DELAY);
        if (ret != HAL_OK)
        {
        	return ret;
        }

        address += chunk;
        data    += chunk;
        length  -= chunk;
    }

    return HAL_OK;
}

/**
 * @brief  EEPROM belleğinin tüm içeriğini 0xFF ile siler (sayfa bazlı page‐write).
 * @param[in]  at24c32   Başlatılmış AT24C32 sürücü yapısı (I2C handle & device_address).
 * @retval     HAL_OK    Tüm sayfalar başarıyla silindi.
 * @retval     HAL_ERROR Parametre hatası veya I²C iletişim hatası.
 * @retval     HAL_TIMEOUT  Herhangi bir sayfa yazma sırasında timeout oluştu.
 */
HAL_StatusTypeDef AT24C32_EraseAll(S_AT24C32_t *at24c32)
{
    HAL_StatusTypeDef status;
    uint16_t           pageCount = AT24C32_TOTAL_SIZE_BYTES / AT24C32_PAGE_SIZE_BYTES;
    uint8_t            buffer[AT24C32_PAGE_SIZE_BYTES];
    uint16_t           pageIdx;

    /* Parametre kontrolü */
    if (at24c32 == NULL)
    {
        return HAL_ERROR;
    }

    /* 0xFF ile dolu bir page tamponu hazırla */
    for (uint16_t i = 0U; i < AT24C32_PAGE_SIZE_BYTES; i++)
    {
        buffer[i] = 0xFFU;
    }

    /* Her bir sayfayı ayrı ayrı page‐write ile sil */
    for (pageIdx = 0U; pageIdx < pageCount; pageIdx++)
    {
        uint16_t addr = (uint16_t)(pageIdx * AT24C32_PAGE_SIZE_BYTES);

        status = AT24C32_WriteData(at24c32,
                                   addr,
								   buffer,
								   AT24C32_PAGE_SIZE_BYTES);

        HAL_Delay(10);

        if (status != HAL_OK)
        {
            return status;
        }
    }

    return HAL_OK;
}

/**
 * @brief  Checks whether the EEPROM device is ready for communication.
 * @param  handle         Pointer to the AT24C32 EEPROM handle structure.
 * @retval HAL status code.
 */
HAL_StatusTypeDef AT24C32_IsDeviceReady(S_AT24C32_t *at24c32)
{
	HAL_StatusTypeDef status;
	AT24C32_EEPROM_WP_ACTIVE;

	status = HAL_I2C_IsDeviceReady(at24c32->i2c_handle, at24c32->device_address, 2, 100);

    return status;
}
