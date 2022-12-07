/*
 * gpio.h
 *
 *  Created on: Oct 12, 2022
 *      Author: armando
 */

#ifndef GPIO_H_
#define GPIO_H_

// pines
#define SDA_PIN         (3u)
#define SCL_PIN         (2u)
#define SQW_PIN         (0u)

#define UART0_RX_PIN    (16u)
#define UART0_TX_PIN	(17u)

#define UART4_RX_PIN	(14u)
#define UART4_TX_PIN	(15u)

// ports
#define SDA_PORT        (PORTB)
#define SCL_PORT       	(PORTB)
#define SQW_PORT        (PORTD)

#define UART0_RX_PORT   (PORTB)
#define UART0_TX_PORT	(PORTB)

#define UART4_RX_PORT	(PORTC)
#define UART4_TX_PORT	(PORTC)

// masks
#define SDA_MASK		(1 << SDA_PIN)
#define SCL_MASK		(1 << SCL_PIN)
#define SQW_MASK		(1 << SQW_PIN)

#define UART0_RX_MASK	(1 << UART0_RX_PIN)
#define UART0_TX_MASK	(1 << UART0_TX_PIN)

#define UART4_RX_MASK	(1 << UART4_RX_PIN)
#define UART4_TX_MASK	(1 << UART4_TX_PIN)

// GPIOs
#define SDA_GPIO		(GPIOB)
#define SCL_GPIO		(GPIOB)
#define SQW_GPIO		(GPIOD)

#define UART0_RX_GPIO	(GPIOB)
#define UART0_TX_GPIO	(GPIOB)

#define UART4_RX_GPIO	(GPIOC)
#define UART4_TX_GPIO	(GPIOC)

/*! These constants are used to select an specific port in the different API functions*/
typedef enum{GPIO_A, /*!< Definition to select GPIO A */
			 GPIO_B, /*!< Definition to select GPIO B */
			 GPIO_C, /*!< Definition to select GPIO C */
			 GPIO_D, /*!< Definition to select GPIO D */
			 GPIO_E, /*!< Definition to select GPIO E */
			 GPIO_F  /*!< Definition to select GPIO F */
			} gpio_name_t;

typedef struct
{
	uint8_t flag_port_a : 1;
	uint8_t flag_port_b : 1;
	uint8_t flag_port_c : 1;
	uint8_t flag_port_d : 1;
	uint8_t flag_port_e : 1;
} gpio_interrupt_flags_t;


void gpio_init(void);
void GPIO_callback_init(gpio_name_t gpio, void (*handler)(uint32_t flags));
void PORTD_IRQHandler(void);


#endif /* GPIO_H_ */
