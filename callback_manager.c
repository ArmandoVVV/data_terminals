/*
 * callback_manager.c
 *
 *  Created on: Oct 21, 2022
 *      Author: armando
 *      brief: this module manages the callback of the system that occurs every
 *      second.
 */

#include "uart.h"
#include "rtc.h"
#include "dht20.h"
#include "AT24C32D.h"
#include "logger.h"

#define RESET_SECONDS 0

static void check_device_connection(UART_Type* UART);

// this function works every second
void write_rtc_values(uint32_t SQ){
	static clock_values_t time;
	static date_values_t date;
	static float32_t temperature;
	static float32_t humidity;
	static uint8_t toggle_read_dht = ZERO_VALUE;
	static uint8_t seconds_passed_uart0 = ZERO_VALUE;
	static uint8_t captures_uart0 = ZERO_VALUE;
	static uint8_t seconds_passed_uart4 = ZERO_VALUE;
	static uint8_t captures_uart4 = ZERO_VALUE;

	if(ENABLE == get_log_status_uart0()){

		// if the time between capture is completed
		if(get_capture_time_uart0() == seconds_passed_uart0){
			set_ROM_uart0();
			if(SET_NUMBER_OF_CAPTURES == get_UART0_current_state()){
				check_device_connection(UART0);
			}
			seconds_passed_uart0 = RESET_SECONDS;
			captures_uart0++;

			// if the device finished all the captures
			if(captures_uart0 >= get_number_of_captures_uart0()){
				set_log_status_uart0(DISABLE);
			}
		}
		seconds_passed_uart0++;
	}

	if(ENABLE == get_log_status_uart4()){
		// if the time between capture is completed
		if(get_capture_time_uart4() == seconds_passed_uart4){
			set_ROM_uart4();
			if(SET_NUMBER_OF_CAPTURES == get_UART4_current_state()){
				check_device_connection(UART4);
			}
			seconds_passed_uart4 = RESET_SECONDS;
			captures_uart4++;

			// if the device finished all the captures
			if(captures_uart4 >= get_number_of_captures_uart4()){
				set_log_status_uart4(DISABLE);
			}
		}
		seconds_passed_uart4++;
	}

	if (ZERO_VALUE == toggle_read_dht){
		dht_send_readCommands();
		toggle_read_dht = 1;
	}
	else{
		if( !(dht_is_busy()) ){
			temperature = get_temperature();
			humidity = get_humidity();

			toggle_read_dht = ZERO_VALUE;
		}
	}

	switch(get_UART0_current_state()){
		case READ_DATE:
			date = get_date();
			write_date(date, UART0);
			check_device_connection(UART0);
		break;

		case READ_TIME:
			time = get_time();
			write_clock(time, UART0);
			check_device_connection(UART0);
		break;

		case READ_TEMP:
			write_temperature(temperature, UART0);
			check_device_connection(UART0);
		break;

		case READ_WET:
			write_humidity(humidity, UART0);
			check_device_connection(UART0);
		break;

		default:

		break;
	}

	switch(get_UART4_current_state()){
		case READ_DATE:
			date = get_date();
			write_date(date, UART4);
			check_device_connection(UART4);
		break;

		case READ_TIME:
			time = get_time();
			write_clock(time, UART4);
			check_device_connection(UART4);
		break;

		case READ_TEMP:
			write_temperature(temperature, UART4);
			check_device_connection(UART4);
		break;

		case READ_WET:
			write_humidity(humidity, UART4);
			check_device_connection(UART4);
		break;

		default:

		break;
	}
}

static void check_device_connection(UART_Type* UART){
	if(TRUE == get_error_flag()){
		write_error_message(UART);
	}
}
