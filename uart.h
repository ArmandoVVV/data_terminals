/*
 * uart.h
 *
 *  Created on: Oct 11, 2022
 *      Author: Armando Cabrales
 */

#ifndef UART_H_
#define UART_H_

#include <stdint.h>
#include "rtc.h"
#include "fsl_uart.h"
#include "AT24C32D.h"

#define ESC 					'\e'
#define ENTER 					'\r'
#define ASCII_VALUE_OF_ONE 		'1'
#define ASCII_VALUE_OF_TWO 		'2'
#define ASCII_VALUE_OF_THREE 	'3'
#define ASCII_VALUE_OF_FOUR 	'4'
#define ASCII_VALUE_OF_FIVE 	'5'
#define ASCII_VALUE_OF_SIX 		'6'
#define ASCII_VALUE_OF_SEVEN 	'7'
#define ASCII_VALUE_OF_EIGHT 	'8'
#define Y						'y'
#define N						'n'

/* UART instance and clock */

#define UART0_CLK_FREQ   		CLOCK_GetFreq(UART0_CLK_SRC)
#define UART4_CLK_FREQ   		CLOCK_GetFreq(UART4_CLK_SRC)
#define UART_BAUDRATE 			9600u
#define PRIORITY_10 			10
#define UART0_IRQ 				31
#define UART4_IRQ 				66
#define NVIC_global_enable_interrupts  __enable_irq()
#define NVIC_global_disable_interrupts __disable_irq()
#define ASCII_OFFSET			48
#define RETURN_INDEX_TO_0 		0

#define DAY_TEN						0
#define DAY_UNIT					1
#define MONTH_TEN					3
#define MONTH_UNIT					4
#define YEAR_TEN					6
#define YEAR_UNIT					7

void uart_init(void);

void uart0_state_machine(void);
void uart4_state_machine(void);

void show_menu(UART_Type* UART);
void show_read_time(UART_Type* UART);
void show_set_time(UART_Type* UART);
void show_set_date(UART_Type* UART);
void show_read_date(UART_Type* UART);
void show_read_temp(UART_Type* UART);
void show_read_wet(UART_Type* UART);
void show_activate_log(UART_Type* UART);
void show_read_log(UART_Type* UART);

void write_date(date_values_t date, UART_Type* UART);
void write_clock(clock_values_t time, UART_Type* UART);
void write_temperature(uint32_t temp, UART_Type* UART);
void write_humidity(uint32_t hum, UART_Type* UART);
void write_rom(rom_values_t rom, UART_Type* UART);

void write_error_message(UART_Type* UART);


void check_mail(void);

typedef struct{
	uint8_t flag; /** Flag to indicate that there is new data*/
	uint8_t mail_box; /** it contains the received data*/
} uart_mail_box_t;

typedef enum{
	MENU,
	SET_TIME,
	SET_DATE,
	READ_TIME,
	READ_DATE,
	READ_TEMP,
	READ_WET,
	ACTIVATE_LOG,
	SET_CAPTURE_TIME,
	SET_NUMBER_OF_CAPTURES,
	READ_LOG
}state_t;

void set_UART0_current_state(state_t state);
state_t get_UART0_current_state(void);

void set_UART4_current_state(state_t state);
state_t get_UART4_current_state(void);

#endif /* UART_H_ */
