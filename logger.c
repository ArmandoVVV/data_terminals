/*
 * logger.c
 *
 *  Created on: Nov 4, 2022
 *      Author: armando
 *      brief: this module has all the necessary function to manage the logger
 *      program, it uses the memory to write and read and works with the time and
 *      number of captures arguments.
 */
#include "logger.h"
#include "config.h"

static uint8_t g_capture_time_uart0 = ZERO_VALUE;
static uint8_t g_number_of_captures_uart0 = ZERO_VALUE;
static device_status_t g_log_status_uart0 = DISABLE;

static uint8_t g_capture_time_uart4 = 0;
static uint8_t g_number_of_captures_uart4 = ZERO_VALUE;
static device_status_t g_log_status_uart4 = DISABLE;

void set_capture_time_uart0(uint8_t* capture_time_array, uint8_t digits){
	if(1 == digits){
		g_capture_time_uart0 = capture_time_array[0];
	}
	else if(2 == digits){
		g_capture_time_uart0 = capture_time_array[0] * 10;
		g_capture_time_uart0 = g_capture_time_uart0 + capture_time_array[1];
	}

}

uint8_t get_capture_time_uart0(void){
	return g_capture_time_uart0;
}

void set_number_of_captures_uart0(uint8_t* number_of_captures_array, uint8_t digits){
	switch (digits){
		case 2:
			g_number_of_captures_uart0 = number_of_captures_array[0] * 10;
			g_number_of_captures_uart0 = g_number_of_captures_uart0 + number_of_captures_array[1];
		break;

		case 3:
			g_number_of_captures_uart0 = 100;
		break;
	}
}

uint8_t get_number_of_captures_uart0(void){
	return g_number_of_captures_uart0;
}

void set_log_status_uart0(device_status_t status){
	g_log_status_uart0 = status;
}

device_status_t get_log_status_uart0(void){
	return g_log_status_uart0;
}


// UART4

void set_capture_time_uart4(uint8_t* capture_time_array, uint8_t digits){
	if(1 == digits){
		g_capture_time_uart4 = capture_time_array[0];
	}
	else if(2 == digits){
		g_capture_time_uart4 = capture_time_array[0] * 10;
		g_capture_time_uart4 = g_capture_time_uart4 + capture_time_array[1];
	}

}

uint8_t get_capture_time_uart4(void){
	return g_capture_time_uart4;
}

void set_number_of_captures_uart4(uint8_t* number_of_captures_array, uint8_t digits){
	switch (digits){
		case 2:
			g_number_of_captures_uart4 = number_of_captures_array[0] * 10;
			g_number_of_captures_uart4 = g_number_of_captures_uart4 + number_of_captures_array[1];
		break;

		case 3:
			g_number_of_captures_uart4 = 100;
		break;
	}
}

uint8_t get_number_of_captures_uart4(void){
	return g_number_of_captures_uart4;
}

void set_log_status_uart4(device_status_t status){
	g_log_status_uart4 = status;
}

device_status_t get_log_status_uart4(void){
	return g_log_status_uart4;
}

