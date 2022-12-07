
/*
 * gpio.c
 *
 *	\brief
 *		This is the source file for the GPIO device driver for Kinetis K64.
 *		It contains all the implementation for configuration functions and runtime functions.
 *		i.e., this is the application programming interface (API) for the GPIO peripheral.
 *
 *
 *  Created on: Oct 12, 2022
 *      Author: armando
 */

#include "fsl_port.h"
#include "fsl_gpio.h"
#include "gpio.h"

volatile static gpio_interrupt_flags_t g_intr_status_flag = {0};

static void (*gpio_D_callback)(uint32_t flags) = 0;

void GPIO_callback_init(gpio_name_t gpio, void (*handler)(uint32_t flags)){
	if(GPIO_D == gpio){
		gpio_D_callback = handler;
	}
}

void PORTD_IRQHandler(void){
	uint32_t irq_status = 0;

	irq_status = GPIO_PortGetInterruptFlags(GPIOD);

	if(gpio_D_callback)
	{
		gpio_D_callback(irq_status);
	}

	GPIO_PortClearInterruptFlags(GPIOD, irq_status);
}


void GPIO_clear_irq_status(gpio_name_t gpio){
	if(GPIO_D == gpio){
		g_intr_status_flag.flag_port_d = false;
	}
}

uint8_t GPIO_get_irq_status(gpio_name_t gpio){
	uint8_t status = 0;

	if(GPIO_D == gpio){
		status = g_intr_status_flag.flag_port_d;
	}

	return(status);
}


void gpio_init(void){
    /* Port B Clock Gate Control: Clock enabled */
    CLOCK_EnableClock(kCLOCK_PortB);
    CLOCK_EnableClock(kCLOCK_PortC);
    CLOCK_EnableClock(kCLOCK_PortD);

	const port_pin_config_t portb_pin2_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is enabled */
                                                    kPORT_OpenDrainEnable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as I2C0_SCL */
                                                    kPORT_MuxAlt2,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister
	};

    const port_pin_config_t portb_pin3_config = {/* Internal pull-up resistor is enabled */
                                                    kPORT_PullUp,
                                                    /* Fast slew rate is configured */
                                                    kPORT_FastSlewRate,
                                                    /* Passive filter is disabled */
                                                    kPORT_PassiveFilterDisable,
                                                    /* Open drain is enabled */
                                                    kPORT_OpenDrainEnable,
                                                    /* Low drive strength is configured */
                                                    kPORT_LowDriveStrength,
                                                    /* Pin is configured as I2C0_SDA */
                                                    kPORT_MuxAlt2,
                                                    /* Pin Control Register fields [15:0] are not locked */
                                                    kPORT_UnlockRegister
    };

    const port_pin_config_t portd_pin0_config = {
			kPORT_PullUp,
			kPORT_FastSlewRate,
			kPORT_PassiveFilterDisable,
			kPORT_OpenDrainDisable,
			kPORT_HighDriveStrength,
			kPORT_MuxAsGpio,
			kPORT_UnlockRegister
    };

	gpio_pin_config_t gpio_input_config = {
			        kGPIO_DigitalInput,
			        0,
	};

    // UART 0 PINS
    PORT_SetPinMux(UART0_RX_PORT, UART0_RX_PIN, kPORT_MuxAlt3);
    PORT_SetPinMux(UART0_TX_PORT, UART0_TX_PIN, kPORT_MuxAlt3);

    // UART 4 PINS
    PORT_SetPinMux(UART4_RX_PORT, UART4_RX_PIN, kPORT_MuxAlt3);
    PORT_SetPinMux(UART4_TX_PORT, UART4_TX_PIN, kPORT_MuxAlt3);

    // I2C0 PINS
    PORT_SetPinConfig(SCL_PORT, SCL_PIN, &portb_pin2_config);

    PORT_SetPinConfig(SDA_PORT, SDA_PIN, &portb_pin3_config);

    GPIO_PinInit(GPIOD, SQW_PIN, &gpio_input_config);
    PORT_SetPinConfig(PORTD, SQW_PIN, &portd_pin0_config);
    PORT_SetPinInterruptConfig(PORTD, SQW_PIN, kPORT_InterruptFallingEdge);
}
