/*
 * i2c.h
 *
 *  Created on: Oct 21, 2022
 *      Author: armando
 *      brief: this module has all the i2c possible communications, every i2c device
 *      passes by this module.
 */

#ifndef I2C_H_
#define I2C_H_

#include "stdint.h"
#include "fsl_i2c.h"

/* I2C source clock */
#define I2C_MASTER_CLK_SRC          I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ         CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_MASTER 					I2C0

#define I2C_BAUDRATE               	400000U
#define I2C_DATA_LENGTH             33U

#define I2C_CLOCK_DATASIZE			3
#define I2C_SQW_DATASIZE			1
#define SUBADDRESS_SIZE				1

#define FIRST_VALUE					0
#define SECOND_VALUE				1
#define THIRD_VALUE					2
#define FOURTH_VALUE				3
#define FIFTH_VALUE					4
#define SIXTH_VALUE					5
#define SEVENTH_VALUE				6
#define EIGTH_VALUE					7

typedef enum{
	RTC_ADDRESS = 0x68,		// general defined address
	SENSOR_ADDRESS = 0x38,	// specific defined address
	ROM_ADDRESS = 0x50		// general defined address
}slave_address_t;

typedef struct{
	uint8_t regAddress;
	uint8_t address;
	uint8_t subaddressSize;
	uint8_t dataSize;

}i2c_device_config_t;


uint8_t* get_master_rxBuff(void);

uint8_t* get_master_txBuff(void);

uint8_t get_error_flag(void);

void i2c_config(void);

void i2c_write_indv(i2c_device_config_t device, uint8_t value);
void i2c_read(i2c_device_config_t device);
void i2c_write_three_values(i2c_device_config_t device, uint8_t first_value, uint8_t second_value, uint8_t third_value);
void i2c_write_eight_values(i2c_device_config_t device, uint8_t value1, uint8_t value2, uint8_t value3, uint8_t value4, uint8_t value5, uint8_t value6, uint8_t value7, uint8_t value8);


#endif /* I2C_H_ */
