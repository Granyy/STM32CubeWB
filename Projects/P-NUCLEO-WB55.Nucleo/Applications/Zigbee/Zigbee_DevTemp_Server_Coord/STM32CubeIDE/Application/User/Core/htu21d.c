/*
 * htu21d.c
 *
 *  Created on: Feb 23, 2021
 *      Author: leo
 */

#include "stm32wbxx_hal.h"


#define HTU21D_ADDRESS 0x40

#define HTU21D_WRITE ((HTU21D_ADDRESS << 1)|0x00)
#define HTU21D_READ  ((HTU21D_ADDRESS << 1)|0x01)

#define HTU21D_TRIGGER_TEMP_MEAS	 0xE3 // Trigger Temperature Measurement
#define HTU21D_TRIGGER_HUM_MEAS      0xE5 // Trigger Humidity Measurement

#define HTU21D_TRIGGER_TEMP_MEAS_NH	 0xF3 // Trigger Temperature Measurement no hold master
#define HTU21D_TRIGGER_HUM_MEAS_NH   0xF5 // Trigger Humidity Measurement no hold master

#define HTU21D_USER_REGISTER_WRITE   0xE6 // Write user register
#define HTU21D_USER_REGISTER_READ    0xE7 // Read  user register

#define HTU21D_SOFT_RESET            0xFE // Soft reset

#define HTU21D_TEMP_COEFFICIENT      -0.15     //temperature coefficient for RH compensation at range 0°C..80°C, for HTU21D & SHT21 only
#define HTU21D_CRC8_POLYNOMINAL      0x13100   //crc8 polynomial for 16bit value, CRC8 -> x^8 + x^5 + x^4 + 1

#define HTU21D_SOFT_RESET_DELAY      15        //in milliseconds

#define HTU21D_ERROR                 0xFF      //returns 255, if CRC8 or communication error is occurred


uint8_t buffer_read[3];

static uint8_t htu21d_check_crc(uint16_t data);

static uint8_t htu21d_check_crc(uint16_t data)
{
  for (uint8_t bit = 0; bit < 16; bit++)
  {
    if   (data & 0x8000) data = (data << 1) ^ HTU21D_CRC8_POLYNOMINAL;
    else data <<= 1;
  }
  return data >>= 8;
}


uint16_t htu21d_get_temp()
{
	uint8_t buffer[1] = { 0xE3 };
	uint32_t rawTemperature;
	//status = HAL_I2C_Master_Transmit(&i2c3,0x80, buffer, sizeof(buffer), 10000);
	//status = HAL_I2C_Master_Receive(&i2c3, 0x81, buffer_read, sizeof(buffer_read), 10000);


	if (htu21d_check_crc(buffer_read[0]<<8|buffer_read[1]))
	{
		rawTemperature = buffer_read[0]<<8|buffer_read[1];
		return ((rawTemperature*17572)/65536) - 4685;
	}
	else
	{
		return 0xFFFF;
	}
}

