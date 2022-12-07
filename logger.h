/*
 * logger.h
 *
 *  Created on: Nov 4, 2022
 *      Author: armando
 *      brief: this module has all the necessary function to manage the logger
 *      program, it uses the memory to write and read and works with the time and
 *      number of captures arguments.
 */

#ifndef LOGGER_H_
#define LOGGER_H_

#include "stdint.h"

typedef enum{
	DISABLE,
	ENABLE
}device_status_t;

void set_capture_time_uart0(uint8_t* capture_time_array, uint8_t digits);
void set_number_of_captures_uart0(uint8_t* number_of_captures_array, uint8_t digits);
uint8_t get_number_of_captures_uart0(void);
uint8_t get_capture_time_uart0(void);
void set_log_status_uart0(device_status_t status);
device_status_t get_log_status_uart0(void);

void set_capture_time_uart4(uint8_t* capture_time_array, uint8_t digits);
void set_number_of_captures_uart4(uint8_t* number_of_captures_array, uint8_t digits);
uint8_t get_number_of_captures_uart4(void);
uint8_t get_capture_time_uart4(void);
void set_log_status_uart4(device_status_t status);
device_status_t get_log_status_uart4(void);


#endif /* LOGGER_H_ */
