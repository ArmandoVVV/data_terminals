/**
 * 				    Equipo_3_P2
 * @authors Armando Cabrales & Laura Garcia
 * @brief   Application entry point.
 */
#include <dht20.h>
#include <rtc.h>
#include <stdio.h>
#include "uart.h"
#include "fsl_uart.h"
#include "gpio.h"
#include "config.h"
#include "i2c.h"
#include "callback_manager.h"
#include "watchdog.h"

int main(void) {
	NVIC_EnableIRQ(PORTD_IRQ);
	NVIC_SetPriority(PORTD_IRQ, PRIORITY_3);

	NVIC_global_enable_interrupts;
	GPIO_callback_init(GPIO_D, write_rtc_values);

	i2c_config();

	gpio_init();
	uart_init();

	rtc_init();
	dht_init();

	setup_watchdog();

    while (TRUE){
    	check_mail();
    	refresh_watchdog();
    }

    return 0 ;
}
