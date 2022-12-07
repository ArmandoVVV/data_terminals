/*
 * AT24C32D.h
 *
 *  Created on: Nov 1, 2022
 *      Author: Laura García
 *      brief: this module has the functions to write and read all the required
 *      values on the memory device.
 */

#ifndef AT24C32D_H_
#define AT24C32D_H_

#include "rtc.h"
#include "i2c.h"
#include "dht20.h"

#define TOTAL_VALUES_TO_STORE	 8
#define UART4_ADDRESS_OFFSET	 0x320
#define ROM_DATASIZE	8
#define ROM_SUBADDRESS_SIZE 2

typedef struct{
	clock_values_t time;
	date_values_t date;
	uint8_t temperature;
	uint8_t humidity;
}rom_values_t;

typedef struct{
	uint8_t capture_time;
	uint8_t number_of_captures;
}log_values_t;

void set_ROM_uart0(void);
rom_values_t get_ROM_uart0(void);
void set_ROM_uart4(void);
rom_values_t get_ROM_uart4(void);


#endif /* AT24C32D_H_ */
