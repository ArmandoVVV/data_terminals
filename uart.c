/*
 * uart.c
 * 	Source file of the UART, it contains the display functions and the configuration
 * 	functions
 *
 *
 *  Created on: Oct 11, 2022
 *      Author: Armando Cabrales
 */
#include <rtc.h>
#include "fsl_port.h"
#include "uart.h"
#include "config.h"
#include "dht20.h"
#include "logger.h"


/*VT100 command for clearing the screen*/
static const uint8_t g_vt100_clear[5] = "\033[2J";

/** VT100 command for setting the background*/
static const uint8_t g_vt100_color_1[8] = "\033[0;43m";

/** VT100 command for setting the foreground*/
static const uint8_t g_vt100_color_2[6] = "\033[34m";

/** VT100 command for setting the background and foreground*/
static const uint8_t g_vt100_color_3[11] = "\033[6;33;34m";

/** VT100 command for positioning the cursor in x and y position*/
static const uint8_t g_vt100_xy_1[9] = "\033[10;20H";
static const uint8_t g_vt100_xy_2[9] = "\033[11;20H";
static const uint8_t g_vt100_xy_3[9] = "\033[12;20H";
static const uint8_t g_vt100_xy_4[9] = "\033[13;20H";
static const uint8_t g_vt100_xy_5[9] = "\033[14;20H";
static const uint8_t g_vt100_xy_6[9] = "\033[15;20H";
static const uint8_t g_vt100_xy_7[9] = "\033[16;20H";
static const uint8_t g_vt100_xy_8[9] = "\033[17;20H";
static const uint8_t g_vt100_xy_10[9] = "\033[19;20H";

static const uint8_t g_vt100_xy_esp[9] = "\033[11;0H";

static const uint8_t g_vt100_menu_1[] = "1) Establecer Hora\r";
static const uint8_t g_vt100_menu_2[] = "2) Establecer Fecha\r";
static const uint8_t g_vt100_menu_3[] = "3) Leer hora\r";
static const uint8_t g_vt100_menu_4[] = "4) Leer fecha\r";
static const uint8_t g_vt100_menu_5[] = "5) Leer Temperatura\r";
static const uint8_t g_vt100_menu_6[] = "6) Leer Humedad\r";
static const uint8_t g_vt100_menu_7[] = "7) Activar Log\r";
static const uint8_t g_vt100_menu_8[] = "8) Leer Log\r";

static const uint8_t g_vt100_set_time_1[/**/] =  "Escribir hora en hh:mm:ss";
static const uint8_t g_vt100_set_time_2[/**/] =  "La hora ha sido cambiada...";

static const uint8_t g_vt100_set_date_1[/**/] =  "Escribir fecha: en dd/mm/aa";
static const uint8_t g_vt100_set_date_2[] = "La fecha ha sido cambiada...";

static const uint8_t g_vt100_read_time_1[] = "La hora actual es:";

static const uint8_t g_vt100_read_date_1[] = "La fecha actual es:";

static const uint8_t g_vt100_read_temp_1[] = "La temperatura es:";

static const uint8_t g_vt100_read_wet_1[] = "La humedad relativa es:";

static const uint8_t g_vt100_activate_log_1[] = "Deseas activar la captura de datos): Si-->Y NO-->N";
static const uint8_t g_vt100_activate_log_2[] = "Introducir tiempo de captura";
static const uint8_t g_vt100_activate_log_5[] = "Introducir numero de capturas";
static const uint8_t g_vt100_read_log_1[] = "¿Deseas leer la captura de datos?: Si-->Y No-->N";

static const uint8_t g_vt100_twoDots[2] =  ":";
static const uint8_t g_vt100_slash[2] =  "/";
static const uint8_t g_vt100_C[2] =  " C";
static const uint8_t g_vt100_error[/**/] =  "ERROR: Sin comunicacion con el dispositivo I2C";
static const uint8_t g_vt100_percent[] = " %";
static const uint8_t g_vt100_block[] = "La otra terminal esta haciendo uso de este recurso";
static const uint8_t g_vt100_wait[] = "Espera 10 segundos para checar la conexion...";

static const uint8_t g_vt100_read_log_2[] = "Temperatura: ";
static const uint8_t g_vt100_read_log_3[] = "C, Humedad ";
static const uint8_t g_vt100_read_log_4[] = "%, ";
static const uint8_t g_vt100_read_log_5[] = " hrs ";
static const uint8_t g_vt100_read_log_6[] = "                            ";

// static functions
static void set_background(UART_Type* UART);
static void uart0_menu_state_machine(void);
static void uart4_menu_state_machine(void);
static void set_date_index(uint8_t value);
static void set_time_index(uint8_t value);
static uint8_t get_date_index(void);
static uint8_t get_time_index(void);
static void uart0_esc_pressed_check(void);
static void uart4_esc_pressed_check(void);

//global variables with non static getters and setters
static state_t g_UART0_current_state = MENU;
static state_t g_UART4_current_state = MENU;

//global variables with static getters and setters
static uart_mail_box_t g_mail_box_uart_0 = {0, 0};
static uart_mail_box_t g_mail_box_uart_4 = {0, 0};
static uint8_t g_date_index = ZERO_VALUE;
static uint8_t g_time_index = ZERO_VALUE;

// setters and getters
static void set_date_index(uint8_t value){
	g_date_index = value;
}

static void set_time_index(uint8_t value){
	g_time_index = value;
}

static uint8_t get_date_index(void){
	return g_date_index;
}

static uint8_t get_time_index(void){
	return g_time_index;
}

void set_UART0_current_state(state_t state){
	g_UART0_current_state = state;
}

state_t get_UART0_current_state(void){
	return g_UART0_current_state;
}

void set_UART4_current_state(state_t state){
	g_UART4_current_state = state;
}

state_t get_UART4_current_state(void){
	return g_UART4_current_state;
}

// functions

void UART0_RX_TX_IRQHandler(void){
    /* If new data arrived. */
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART0))
    {
    	g_mail_box_uart_0.mail_box = UART_ReadByte(UART0);
    	g_mail_box_uart_0.flag = TRUE;

    }
    SDK_ISR_EXIT_BARRIER;
}

void UART4_RX_TX_IRQHandler(void){
    /* If new data arrived. */
    if ((kUART_RxDataRegFullFlag | kUART_RxOverrunFlag) & UART_GetStatusFlags(UART4))
    {
    	g_mail_box_uart_4.mail_box = UART_ReadByte(UART4);
    	g_mail_box_uart_4.flag = TRUE;

    }
    SDK_ISR_EXIT_BARRIER;
}

void uart_init(void){
    uart_config_t config;
    uint32_t uart0_clock;
    uint32_t uart4_clock;

    UART_GetDefaultConfig(&config);
    config.baudRate_Bps = UART_BAUDRATE;
    config.enableTx     = TRUE;
    config.enableRx     = TRUE;

    uart0_clock = CLOCK_GetFreq(UART0_CLK_SRC);
    uart4_clock = CLOCK_GetFreq(UART4_CLK_SRC);

    UART_Init(UART0, &config, uart0_clock);
    UART_Init(UART4, &config, uart4_clock);

    show_menu(UART0);
    show_menu(UART4);
    /* Enable RX interrupt. */
    UART_EnableInterrupts(UART0, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);
    UART_EnableInterrupts(UART4, kUART_RxDataRegFullInterruptEnable | kUART_RxOverrunInterruptEnable);

    /* Enable UART0 interrupt. */
	NVIC_EnableIRQ(UART0_IRQ);
	NVIC_SetPriority(UART0_IRQ, PRIORITY_10);

	/* Enable UART4 interrupt. */
	NVIC_EnableIRQ(UART4_IRQ);
	NVIC_SetPriority(UART4_IRQ, PRIORITY_10);

	NVIC_global_enable_interrupts;
}

uint8_t ascii_to_decimal(uint8_t value){
	return value - ASCII_OFFSET;
}

void check_mail(void){
	if(g_mail_box_uart_0.flag){
		/**Sends to the PCA the received data in the mailbox*/
		UART_WriteBlocking(UART0, &g_mail_box_uart_0.mail_box, 1);

		uart0_state_machine();

		/**clear the reception flag*/
		g_mail_box_uart_0.flag = 0;
	}

	if(g_mail_box_uart_4.flag){
		/**Sends to the PCA the received data in the mailbox*/
		UART_WriteBlocking(UART4, &g_mail_box_uart_4.mail_box, 1);

		uart4_state_machine();

		/**clear the reception flag*/
		g_mail_box_uart_4.flag = 0;
	}
}

void uart0_state_machine(void){
	static uint8_t time_array[9];
	static clock_values_t time;

	static uint8_t date_array[9];
	static date_values_t date;

	static uint8_t number_of_captures[3];
	static uint8_t ncaptures_index = 0;

	static uint8_t capture_time[2];
	static uint8_t capture_time_index = 0;

	static uint8_t previous_mailbox;

	static uint8_t capture = 0;

	switch(get_UART0_current_state()){
		case MENU:
			ncaptures_index = 0;
			capture_time_index = 0;
			uart0_menu_state_machine();
		break;

		case READ_TIME:
			uart0_esc_pressed_check();
		break;

		case SET_TIME:
			uart0_esc_pressed_check();

			if(ENTER == g_mail_box_uart_0.mail_box){
				time.hours = MOVE_TO_UPPER(time_array[HOUR_TEN]) + time_array[HOUR_UNIT];
				time.minutes = MOVE_TO_UPPER(time_array[MIN_TEN]) + time_array[MIN_UNIT];
				time.seconds = MOVE_TO_UPPER(time_array[SEC_TEN]) + time_array[SEC_UNIT];

				set_time(time);
				UART_WriteBlocking(UART0, g_vt100_xy_4, SIZEOF_VAR(g_vt100_xy_4));
			    UART_WriteBlocking(UART0, g_vt100_set_time_2, SIZEOF_VAR(g_vt100_set_time_2));
			}
			time_array[get_time_index()] = ascii_to_decimal(g_mail_box_uart_0.mail_box);
			set_time_index(get_time_index() + 1);
		break;

		case SET_DATE:
			uart0_esc_pressed_check();

			if(ENTER == g_mail_box_uart_0.mail_box){
				date.year = MOVE_TO_UPPER(date_array[YEAR_TEN]) + date_array[YEAR_UNIT];
				date.month = MOVE_TO_UPPER(date_array[MONTH_TEN]) + date_array[MONTH_UNIT];
				date.day = MOVE_TO_UPPER(date_array[DAY_TEN]) + date_array[DAY_UNIT];

				set_date(date);
				UART_WriteBlocking(UART0, g_vt100_xy_4, SIZEOF_VAR(g_vt100_xy_4));
			    UART_WriteBlocking(UART0, g_vt100_set_date_2, SIZEOF_VAR(g_vt100_set_date_2));
			}
			date_array[get_date_index()] = ascii_to_decimal(g_mail_box_uart_0.mail_box);
			set_date_index(get_date_index() + 1);

		break;

		case READ_DATE:
			uart0_esc_pressed_check();
		break;

		case READ_TEMP:
			uart0_esc_pressed_check();
		break;

		case READ_WET:
			uart0_esc_pressed_check();
		break;

		case ACTIVATE_LOG:
			uart0_esc_pressed_check();

			if(ENTER == g_mail_box_uart_0.mail_box){
				if(Y == previous_mailbox){
					set_UART0_current_state(SET_CAPTURE_TIME);

				    UART_WriteBlocking(UART0, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
				    UART_WriteBlocking(UART0, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
					UART_WriteBlocking(UART0, g_vt100_activate_log_2, SIZEOF_VAR(g_vt100_activate_log_2));

				    UART_WriteBlocking(UART0, g_vt100_xy_3, SIZEOF_VAR(g_vt100_xy_3));
				}
			}
			else{
				previous_mailbox = g_mail_box_uart_0.mail_box;
			}

		break;

		case SET_CAPTURE_TIME:
			uart0_esc_pressed_check();

			if(ENTER == g_mail_box_uart_0.mail_box){

				set_UART0_current_state(SET_NUMBER_OF_CAPTURES);
				UART_WriteBlocking(UART0, g_vt100_xy_5, SIZEOF_VAR(g_vt100_xy_5));
				UART_WriteBlocking(UART0, g_vt100_activate_log_5, SIZEOF_VAR(g_vt100_activate_log_5));

			    UART_WriteBlocking(UART0, g_vt100_xy_6, SIZEOF_VAR(g_vt100_xy_6));

			}
			else{
				capture_time[capture_time_index] = ascii_to_decimal(g_mail_box_uart_0.mail_box);
				capture_time_index++;
			}


		break;

		case SET_NUMBER_OF_CAPTURES:

			uart0_esc_pressed_check();

			if(ENTER == g_mail_box_uart_0.mail_box){
				set_capture_time_uart0(capture_time, capture_time_index);
				set_number_of_captures_uart0(number_of_captures, ncaptures_index);

				set_log_status_uart0(ENABLE);

				UART_WriteBlocking(UART0, g_vt100_xy_7, SIZEOF_VAR(g_vt100_xy_7));
				UART_WriteBlocking(UART0, g_vt100_wait, SIZEOF_VAR(g_vt100_wait));
			}
			else{
				number_of_captures[ncaptures_index] = ascii_to_decimal(g_mail_box_uart_0.mail_box);
				ncaptures_index++;
			}

		break;

		case READ_LOG:
			uart0_esc_pressed_check();

			if(ENTER == g_mail_box_uart_0.mail_box){
				if(Y == previous_mailbox){
					UART_WriteBlocking(UART0, g_vt100_xy_esp, SIZEOF_VAR(g_vt100_xy_esp));
					for(capture = 0; capture < get_number_of_captures_uart0(); capture++){
						write_rom(get_ROM_uart0(), UART0);
					}
					if(TRUE == get_error_flag()){
						UART_WriteBlocking(UART0, g_vt100_error, SIZEOF_VAR(g_vt100_error));
					}
				}
			}
			else{
				previous_mailbox = g_mail_box_uart_0.mail_box;
			}


		break;
	}
}

void uart4_state_machine(void){
	static uint8_t time_array[9];
	static clock_values_t time;

	static uint8_t date_array[9];
	static date_values_t date;

	static uint8_t number_of_captures[3];
	static uint8_t ncaptures_index = 0;

	static uint8_t capture_time[2];
	static uint8_t capture_time_index = 0;

	static uint8_t previous_mailbox;

	static uint8_t capture = 0;

	switch(get_UART4_current_state()){
		case MENU:
			ncaptures_index = 0;
			capture_time_index = 0;
			uart4_menu_state_machine();
		break;

		case READ_TIME:
			uart4_esc_pressed_check();
		break;

		case SET_TIME:
			uart4_esc_pressed_check();

			if(ENTER == g_mail_box_uart_4.mail_box){
				time.hours = MOVE_TO_UPPER(time_array[HOUR_TEN]) + time_array[HOUR_UNIT];
				time.minutes = MOVE_TO_UPPER(time_array[MIN_TEN]) + time_array[MIN_UNIT];
				time.seconds = MOVE_TO_UPPER(time_array[SEC_TEN]) + time_array[SEC_UNIT];

				set_time(time);

				// text that indicates that the hour has been changed
				UART_WriteBlocking(UART4, g_vt100_xy_4, SIZEOF_VAR(g_vt100_xy_4));
			    UART_WriteBlocking(UART4, g_vt100_set_time_2, SIZEOF_VAR(g_vt100_set_time_2));
			}
			time_array[get_time_index()] = ascii_to_decimal(g_mail_box_uart_4.mail_box);
			set_time_index(get_time_index() + 1);
		break;

		case SET_DATE:
			uart4_esc_pressed_check();

			if(ENTER == g_mail_box_uart_4.mail_box){
				date.year = MOVE_TO_UPPER(date_array[YEAR_TEN]) + date_array[YEAR_UNIT];
				date.month = MOVE_TO_UPPER(date_array[MONTH_TEN]) + date_array[MONTH_UNIT];
				date.day = MOVE_TO_UPPER(date_array[DAY_TEN]) + date_array[DAY_UNIT];

				set_date(date);

				// text that indicates that the date has been changed
				UART_WriteBlocking(UART4, g_vt100_xy_4, SIZEOF_VAR(g_vt100_xy_4));
			    UART_WriteBlocking(UART4, g_vt100_set_date_2, SIZEOF_VAR(g_vt100_set_date_2));
			}
			date_array[get_date_index()] = ascii_to_decimal(g_mail_box_uart_4.mail_box);
			set_date_index(get_date_index() + 1);

		break;

		case READ_DATE:
			uart4_esc_pressed_check();
		break;

		case READ_TEMP:
			uart4_esc_pressed_check();
		break;

		case READ_WET:
			uart4_esc_pressed_check();
		break;

		case ACTIVATE_LOG:
			uart4_esc_pressed_check();

			if(ENTER == g_mail_box_uart_4.mail_box){
				if(Y == previous_mailbox){
					set_UART4_current_state(SET_CAPTURE_TIME);

				    UART_WriteBlocking(UART4, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
				    UART_WriteBlocking(UART4, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
					UART_WriteBlocking(UART4, g_vt100_activate_log_2, SIZEOF_VAR(g_vt100_activate_log_2));

				    UART_WriteBlocking(UART4, g_vt100_xy_3, SIZEOF_VAR(g_vt100_xy_3));
				}
			}
			else{
				previous_mailbox = g_mail_box_uart_4.mail_box;
			}

		break;

		case SET_CAPTURE_TIME:
			uart4_esc_pressed_check();

			if(ENTER == g_mail_box_uart_4.mail_box){
				set_UART4_current_state(SET_NUMBER_OF_CAPTURES);
				UART_WriteBlocking(UART4, g_vt100_xy_5, SIZEOF_VAR(g_vt100_xy_5));
				UART_WriteBlocking(UART4, g_vt100_activate_log_5, SIZEOF_VAR(g_vt100_activate_log_5));
			    UART_WriteBlocking(UART4, g_vt100_xy_6, SIZEOF_VAR(g_vt100_xy_6));
			}
			else{
				capture_time[capture_time_index] = ascii_to_decimal(g_mail_box_uart_4.mail_box);
				capture_time_index++;
			}


		break;

		case SET_NUMBER_OF_CAPTURES:

			uart4_esc_pressed_check();

			if(ENTER == g_mail_box_uart_4.mail_box){
				set_capture_time_uart4(capture_time, capture_time_index);
				set_number_of_captures_uart4(number_of_captures, ncaptures_index);

				set_log_status_uart4(ENABLE);

				UART_WriteBlocking(UART4, g_vt100_xy_7, SIZEOF_VAR(g_vt100_xy_7));
				UART_WriteBlocking(UART4, g_vt100_wait, SIZEOF_VAR(g_vt100_wait));
			}
			else{
				number_of_captures[ncaptures_index] = ascii_to_decimal(g_mail_box_uart_4.mail_box);
				ncaptures_index++;
			}

		break;

		case READ_LOG:
			uart4_esc_pressed_check();

			if(ENTER == g_mail_box_uart_4.mail_box){
				if(Y == previous_mailbox){
					UART_WriteBlocking(UART4, g_vt100_xy_esp, SIZEOF_VAR(g_vt100_xy_esp));
					for(capture = 0; capture < get_number_of_captures_uart4(); capture++){
						write_rom(get_ROM_uart4(), UART4);
					}
					if(TRUE == get_error_flag()){
						UART_WriteBlocking(UART4, g_vt100_error, SIZEOF_VAR(g_vt100_error));
					}
				}
			}
			else{
				previous_mailbox = g_mail_box_uart_4.mail_box;
			}


		break;
	}
}

static void uart0_esc_pressed_check(void){
	if(ESC == g_mail_box_uart_0.mail_box){
		show_menu(UART0);
		set_UART0_current_state(MENU);
	}
}

static void uart4_esc_pressed_check(void){
	if(ESC == g_mail_box_uart_4.mail_box){
		show_menu(UART4);
		set_UART4_current_state(MENU);
	}
}

static void uart0_menu_state_machine(void){
	set_time_index(RETURN_INDEX_TO_0);
	set_date_index(RETURN_INDEX_TO_0);

	switch(g_mail_box_uart_0.mail_box){
		case ASCII_VALUE_OF_ONE:
			if( SET_TIME  != get_UART4_current_state()){
				show_set_time(UART0);
				set_UART0_current_state(SET_TIME);
			}
			else{
			    UART_WriteBlocking(UART0, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART0, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}
		break;

		case ASCII_VALUE_OF_TWO:
			if(SET_DATE  != get_UART4_current_state()){
				show_set_date(UART0);
				set_UART0_current_state(SET_DATE);
			}
			else{
			    UART_WriteBlocking(UART0, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART0, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}

		break;

		case ASCII_VALUE_OF_THREE:
			if(READ_TIME  != get_UART4_current_state()){
				show_read_time(UART0);
				set_UART0_current_state(READ_TIME);
			}
			else{
			    UART_WriteBlocking(UART0, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART0, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}

		break;

		case ASCII_VALUE_OF_FOUR:
			if(READ_DATE  != get_UART4_current_state()){
				show_read_date(UART0);
				set_UART0_current_state(READ_DATE);
			}
			else{
			    UART_WriteBlocking(UART0, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART0, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}
		break;

		case ASCII_VALUE_OF_FIVE:
			if(READ_TEMP  != get_UART4_current_state()){
				show_read_temp(UART0);
				set_UART0_current_state(READ_TEMP);
			}
			else{
			    UART_WriteBlocking(UART0, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART0, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}

		break;

		case ASCII_VALUE_OF_SIX:
			if(READ_WET  != get_UART4_current_state()){
				show_read_wet(UART0);
				set_UART0_current_state(READ_WET);
			}
			else{
			    UART_WriteBlocking(UART0, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART0, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}

		break;

		case ASCII_VALUE_OF_SEVEN:
			show_activate_log(UART0);
			set_UART0_current_state(ACTIVATE_LOG);
		break;

		case ASCII_VALUE_OF_EIGHT:
			show_read_log(UART0);
			set_UART0_current_state(READ_LOG);
		break;

		default:
			// ignore
		break;
	}
}

static void uart4_menu_state_machine(void){
	set_time_index(RETURN_INDEX_TO_0);
	set_date_index(RETURN_INDEX_TO_0);

	switch(g_mail_box_uart_4.mail_box){
		case ASCII_VALUE_OF_ONE:
			if( SET_TIME  != get_UART0_current_state()){
				show_set_time(UART4);
				set_UART4_current_state(SET_TIME);
			}
			else{
			    UART_WriteBlocking(UART4, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART4, g_vt100_block, SIZEOF_VAR(g_vt100_block));

			}
		break;

		case ASCII_VALUE_OF_TWO:
			if(SET_DATE  != get_UART0_current_state()){
				show_set_date(UART4);
				set_UART4_current_state(SET_DATE);
			}
			else{
			    UART_WriteBlocking(UART4, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART4, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}
		break;

		case ASCII_VALUE_OF_THREE:
			if(READ_TIME != get_UART0_current_state()){
				show_read_time(UART4);
				set_UART4_current_state(READ_TIME);
			}
			else{
			    UART_WriteBlocking(UART4, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART4, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}
		break;

		case ASCII_VALUE_OF_FOUR:
			if(READ_DATE != get_UART0_current_state()){
				show_read_date(UART4);
				set_UART4_current_state(READ_DATE);
			}
			else{
			    UART_WriteBlocking(UART4, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART4, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}

		break;

		case ASCII_VALUE_OF_FIVE:
			if(READ_TEMP != get_UART0_current_state()){
				show_read_temp(UART4);
				set_UART4_current_state(READ_TEMP);
			}
			else{
			    UART_WriteBlocking(UART4, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART4, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}
		break;

		case ASCII_VALUE_OF_SIX:
			if(READ_WET != get_UART0_current_state()){
				show_read_wet(UART4);
				set_UART4_current_state(READ_WET);
			}
			else{
			    UART_WriteBlocking(UART4, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
			    UART_WriteBlocking(UART4, g_vt100_block, SIZEOF_VAR(g_vt100_block));
			}
		break;

		case ASCII_VALUE_OF_SEVEN:
			show_activate_log(UART4);
			set_UART4_current_state(ACTIVATE_LOG);
		break;

		case ASCII_VALUE_OF_EIGHT:
			show_read_log(UART4);
			set_UART4_current_state(READ_LOG);
		break;

		default:
			// ignore
		break;
	}
}

// show functions write on Tera Term the display of each state
void show_menu(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // firts line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_menu_1, SIZEOF_VAR(g_vt100_menu_1));

    // second line of text
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_menu_2, SIZEOF_VAR(g_vt100_menu_2));

    // third line
    UART_WriteBlocking(UART, g_vt100_xy_3, SIZEOF_VAR(g_vt100_xy_3));
    UART_WriteBlocking(UART, g_vt100_menu_3, SIZEOF_VAR(g_vt100_menu_3));

    // fourth line
    UART_WriteBlocking(UART, g_vt100_xy_4, SIZEOF_VAR(g_vt100_xy_4));
    UART_WriteBlocking(UART, g_vt100_menu_4, SIZEOF_VAR(g_vt100_menu_4));

    // fifth line
    UART_WriteBlocking(UART, g_vt100_xy_5, SIZEOF_VAR(g_vt100_xy_5));
    UART_WriteBlocking(UART, g_vt100_menu_5, SIZEOF_VAR(g_vt100_menu_5));

    // sixth line
    UART_WriteBlocking(UART, g_vt100_xy_6, SIZEOF_VAR(g_vt100_xy_6));
    UART_WriteBlocking(UART, g_vt100_menu_6, SIZEOF_VAR(g_vt100_menu_6));

    // seventh line
    UART_WriteBlocking(UART, g_vt100_xy_7, SIZEOF_VAR(g_vt100_xy_7));
    UART_WriteBlocking(UART, g_vt100_menu_7, SIZEOF_VAR(g_vt100_menu_7));

    // eighth line
    UART_WriteBlocking(UART, g_vt100_xy_8, SIZEOF_VAR(g_vt100_xy_8));
    UART_WriteBlocking(UART, g_vt100_menu_8, SIZEOF_VAR(g_vt100_menu_8));
}

void show_read_time(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_read_time_1, SIZEOF_VAR(g_vt100_read_time_1));

    // setups second line where input is shown
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_3));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_set_time(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_set_time_1, SIZEOF_VAR(g_vt100_set_time_1));

    // setups second line where input is shown
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_set_date(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_set_date_1, SIZEOF_VAR(g_vt100_set_date_1));

    // setups second line where input is shown
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_read_date(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_read_date_1, SIZEOF_VAR(g_vt100_read_date_1));

    // setups second line
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_read_temp(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_read_temp_1, SIZEOF_VAR(g_vt100_read_temp_1));

    // setups second line
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_read_wet(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_read_wet_1, SIZEOF_VAR(g_vt100_read_wet_1));

    // setups second line
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_activate_log(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_activate_log_1, SIZEOF_VAR(g_vt100_activate_log_1));

    // setups second line
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

void show_read_log(UART_Type* UART){
	// clear, set background and start position
	set_background(UART);

    // first line of text
    UART_WriteBlocking(UART, g_vt100_color_2, SIZEOF_VAR(g_vt100_color_2));
    UART_WriteBlocking(UART, g_vt100_read_log_1, SIZEOF_VAR(g_vt100_read_log_1));

    // setups second line
    UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));
    UART_WriteBlocking(UART, g_vt100_color_3, SIZEOF_VAR(g_vt100_color_3));
}

// clear, set background and start position
void set_background(UART_Type* UART){
    UART_WriteBlocking(UART, g_vt100_clear, SIZEOF_VAR(g_vt100_clear));
    UART_WriteBlocking(UART, g_vt100_xy_1, SIZEOF_VAR(g_vt100_xy_1));
    UART_WriteBlocking(UART, g_vt100_color_1, SIZEOF_VAR(g_vt100_color_1));
    UART_WriteBlocking(UART, g_vt100_clear, SIZEOF_VAR(g_vt100_clear));
}


// this function writes the current RTC´s date in format dd/mm/yy,
// it only writes when current state is READ_DATE
void write_date(date_values_t date, UART_Type* UART){
	static uint8_t day_0 = ZERO_VALUE;
	static uint8_t day_1 = ZERO_VALUE;
	static uint8_t month_0 = ZERO_VALUE;
	static uint8_t month_1 = ZERO_VALUE;
	static uint8_t year_0 = ZERO_VALUE;
	static uint8_t year_1 = ZERO_VALUE;

	day_0 = LOWER_MASK(date.day) + ASCII_OFFSET;
	day_1 = MOVE_TO_LOWER(date.day) + ASCII_OFFSET;

	month_0 = LOWER_MASK(date.month) + ASCII_OFFSET;
	month_1 = MOVE_TO_LOWER(date.month) + ASCII_OFFSET;

	year_0 = LOWER_MASK(date.year) + ASCII_OFFSET;
	year_1 = MOVE_TO_LOWER(date.year) + ASCII_OFFSET;

	UART_WriteBlocking(UART, g_vt100_xy_4, SIZEOF_VAR(g_vt100_xy_4));

	// WRITE_DAY
	UART_WriteBlocking(UART, &day_1, SIZEOF_VAR(day_1));
	UART_WriteBlocking(UART, &day_0, SIZEOF_VAR(day_0));

	UART_WriteBlocking(UART, g_vt100_slash, SIZEOF_VAR(g_vt100_slash));

	// WRITE_MONTH
	UART_WriteBlocking(UART, &month_1, SIZEOF_VAR(month_1));
	UART_WriteBlocking(UART, &month_0, SIZEOF_VAR(month_0));

	UART_WriteBlocking(UART, g_vt100_slash, SIZEOF_VAR(g_vt100_slash));

	// WRITE YEAR
	UART_WriteBlocking(UART, &year_1, SIZEOF_VAR(year_1));
	UART_WriteBlocking(UART, &year_0, SIZEOF_VAR(year_0));
}

void write_clock(clock_values_t time, UART_Type* UART){
	static uint8_t sec_0 = ZERO_VALUE;
	static uint8_t sec_1 = ZERO_VALUE;
	static uint8_t min_0 = ZERO_VALUE;
	static uint8_t min_1 = ZERO_VALUE;
	static uint8_t hour_0 = ZERO_VALUE;
	static uint8_t hour_1 = ZERO_VALUE;

	sec_0 = LOWER_MASK(time.seconds) + ASCII_OFFSET;
	sec_1 = MOVE_TO_LOWER(time.seconds) + ASCII_OFFSET;

	min_0 = LOWER_MASK(time.minutes) + ASCII_OFFSET;
	min_1 = MOVE_TO_LOWER(time.minutes) + ASCII_OFFSET;

	hour_0 = LOWER_MASK(time.hours) + ASCII_OFFSET;
	hour_1 = MOVE_TO_LOWER(time.hours) + ASCII_OFFSET;

	UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));

	// WRITE HOURS
	UART_WriteBlocking(UART, &hour_1, SIZEOF_VAR(hour_1));
	UART_WriteBlocking(UART, &hour_0, SIZEOF_VAR(hour_0));

	UART_WriteBlocking(UART, g_vt100_twoDots, SIZEOF_VAR(g_vt100_twoDots));

	// WRITE_MINUTES
	UART_WriteBlocking(UART, &min_1, SIZEOF_VAR(min_1));
	UART_WriteBlocking(UART, &min_0, SIZEOF_VAR(min_0));

	UART_WriteBlocking(UART, g_vt100_twoDots, SIZEOF_VAR(g_vt100_twoDots));

	// WRITE_SECONDS
	UART_WriteBlocking(UART, &sec_1, SIZEOF_VAR(sec_1));
	UART_WriteBlocking(UART, &sec_0, SIZEOF_VAR(sec_0));
}

void write_temperature(uint32_t temp, UART_Type* UART){
	static uint8_t temp_0 = ZERO_VALUE;
	static uint8_t temp_1 = ZERO_VALUE;

	temp_0 = (temp % 10) + ASCII_OFFSET;
	temp_1 = (temp / 10) + ASCII_OFFSET;

	UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));

	// WRITE TEMPERATURE
	UART_WriteBlocking(UART, &temp_1, SIZEOF_VAR(temp_1));
	UART_WriteBlocking(UART, &temp_0, SIZEOF_VAR(temp_0));

	UART_WriteBlocking(UART, g_vt100_C, SIZEOF_VAR(g_vt100_C));

}

void write_humidity(uint32_t hum, UART_Type* UART){
	static uint8_t hum_0 = ZERO_VALUE;
	static uint8_t hum_1 = ZERO_VALUE;

	hum_0 = (hum % 10) + ASCII_OFFSET;
	hum_1 = (hum / 10) + ASCII_OFFSET;

	UART_WriteBlocking(UART, g_vt100_xy_2, SIZEOF_VAR(g_vt100_xy_2));

	// WRITE HUMIDITY
	UART_WriteBlocking(UART, &hum_1, SIZEOF_VAR(hum_1));
	UART_WriteBlocking(UART, &hum_0, SIZEOF_VAR(hum_0));

	UART_WriteBlocking(UART, g_vt100_percent, SIZEOF_VAR(g_vt100_percent));

}

void write_error_message(UART_Type* UART){
	UART_WriteBlocking(UART, g_vt100_xy_10, SIZEOF_VAR(g_vt100_xy_10));
	UART_WriteBlocking(UART, g_vt100_error, SIZEOF_VAR(g_vt100_error));
}

void write_rom(rom_values_t rom, UART_Type* UART){
	static uint8_t temp_0 = ZERO_VALUE;
	static uint8_t temp_1 = ZERO_VALUE;

	static uint8_t hum_0 = ZERO_VALUE;
	static uint8_t hum_1 = ZERO_VALUE;

	static uint8_t sec_0 = ZERO_VALUE;
	static uint8_t sec_1 = ZERO_VALUE;
	static uint8_t min_0 = ZERO_VALUE;
	static uint8_t min_1 = ZERO_VALUE;
	static uint8_t hour_0 = ZERO_VALUE;
	static uint8_t hour_1 = ZERO_VALUE;

	static uint8_t day_0 = ZERO_VALUE;
	static uint8_t day_1 = ZERO_VALUE;
	static uint8_t month_0 = ZERO_VALUE;
	static uint8_t month_1 = ZERO_VALUE;
	static uint8_t year_0 = ZERO_VALUE;
	static uint8_t year_1 = ZERO_VALUE;

	temp_0 = (rom.temperature % 10) + ASCII_OFFSET;
	temp_1 = (rom.temperature / 10) + ASCII_OFFSET;

	hum_0 = (rom.humidity % 10) + ASCII_OFFSET;
	hum_1 = (rom.humidity / 10) + ASCII_OFFSET;

	sec_0 = LOWER_MASK(rom.time.seconds) + ASCII_OFFSET;
	sec_1 = MOVE_TO_LOWER(rom.time.seconds) + ASCII_OFFSET;

	min_0 = LOWER_MASK(rom.time.minutes) + ASCII_OFFSET;
	min_1 = MOVE_TO_LOWER(rom.time.minutes) + ASCII_OFFSET;

	hour_0 = LOWER_MASK(rom.time.hours) + ASCII_OFFSET;
	hour_1 = MOVE_TO_LOWER(rom.time.hours) + ASCII_OFFSET;

	day_0 = LOWER_MASK(rom.date.day) + ASCII_OFFSET;
	day_1 = MOVE_TO_LOWER(rom.date.day) + ASCII_OFFSET;

	month_0 = LOWER_MASK(rom.date.month) + ASCII_OFFSET;
	month_1 = MOVE_TO_LOWER(rom.date.month) + ASCII_OFFSET;

	year_0 = LOWER_MASK(rom.date.year) + ASCII_OFFSET;
	year_1 = MOVE_TO_LOWER(rom.date.year) + ASCII_OFFSET;

	UART_WriteBlocking(UART, g_vt100_read_log_2, SIZEOF_VAR(g_vt100_read_log_2));

	// WRITE TEMPERATURE
	UART_WriteBlocking(UART, &temp_1, SIZEOF_VAR(temp_1));
	UART_WriteBlocking(UART, &temp_0, SIZEOF_VAR(temp_0));

	UART_WriteBlocking(UART, g_vt100_read_log_3, SIZEOF_VAR(g_vt100_read_log_3));

	// WRITE HUMIDITY
	UART_WriteBlocking(UART, &hum_1, SIZEOF_VAR(hum_1));
	UART_WriteBlocking(UART, &hum_0, SIZEOF_VAR(hum_0));

	UART_WriteBlocking(UART, g_vt100_read_log_4, SIZEOF_VAR(g_vt100_read_log_4));

	// WRITE HOURS
	UART_WriteBlocking(UART, &hour_1, SIZEOF_VAR(hour_1));
	UART_WriteBlocking(UART, &hour_0, SIZEOF_VAR(hour_0));

	UART_WriteBlocking(UART, g_vt100_twoDots, SIZEOF_VAR(g_vt100_twoDots));

	// WRITE_MINUTES
	UART_WriteBlocking(UART, &min_1, SIZEOF_VAR(min_1));
	UART_WriteBlocking(UART, &min_0, SIZEOF_VAR(min_0));

	UART_WriteBlocking(UART, g_vt100_twoDots, SIZEOF_VAR(g_vt100_twoDots));

	// WRITE_SECONDS
	UART_WriteBlocking(UART, &sec_1, SIZEOF_VAR(sec_1));
	UART_WriteBlocking(UART, &sec_0, SIZEOF_VAR(sec_0));

	UART_WriteBlocking(UART, g_vt100_read_log_5, SIZEOF_VAR(g_vt100_read_log_5));

	// WRITE_DAY
	UART_WriteBlocking(UART, &day_1, SIZEOF_VAR(day_1));
	UART_WriteBlocking(UART, &day_0, SIZEOF_VAR(day_0));

	UART_WriteBlocking(UART, g_vt100_slash, SIZEOF_VAR(g_vt100_slash));

	// WRITE_MONTH
	UART_WriteBlocking(UART, &month_1, SIZEOF_VAR(month_1));
	UART_WriteBlocking(UART, &month_0, SIZEOF_VAR(month_0));

	UART_WriteBlocking(UART, g_vt100_slash, SIZEOF_VAR(g_vt100_slash));

	// WRITE YEAR
	UART_WriteBlocking(UART, &year_1, SIZEOF_VAR(year_1));
	UART_WriteBlocking(UART, &year_0, SIZEOF_VAR(year_0));

	UART_WriteBlocking(UART, g_vt100_read_log_6, SIZEOF_VAR(g_vt100_read_log_6));
}


