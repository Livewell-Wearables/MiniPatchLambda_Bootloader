/*
 * crc.c
 *
 *  Created on: Jan 9, 2026
 *      Author: Fatih
 */


#include "crc.h"

uint32_t CRC32_Calculate(const uint8_t *data, uint32_t length)
{
    uint32_t crc = 0xFFFFFFFFu;

    for (uint32_t i = 0; i < length; i++)
    {
        crc ^= data[i];

        for (uint8_t j = 0; j < 8; j++)
        {
            if (crc & 1u)
            {
                crc = (crc >> 1) ^ 0xEDB88320u;
            }
            else
            {
                crc >>= 1;
            }
        }
    }

    return crc ^ 0xFFFFFFFFu;
}

uint8_t CRC32_Verify(const uint8_t *data,
                     uint32_t data_len,
                     uint32_t received_crc)
{
    uint32_t calculated_crc;

    if ((data == NULL) || (data_len == 0u))
    {
        return 0u;
    }

    calculated_crc = CRC32_Calculate(data, data_len);

    if (calculated_crc == received_crc)
    {
        return 1u;   /* CRC OK */
    }
    else
    {
        return 0u;   /* CRC FAIL */
    }
}
