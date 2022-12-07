/*
 * config.h
 *
 *  	This is a source file for the configuration for the practice 2.
 * 		Device driver for Kinetis K64.
 * 		it contains all the definitions of the macros.
 *
 *  Created on: Oct 14, 2022
 *      Author: armando
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define TRUE						1
#define FALSE						0

#define SECONDS 					0
#define MINUTES 					1
#define HOURS 						2

#define DAY 						4
#define MONTH 						5
#define YEAR						6

#define ZERO_VALUE					0x00
#define ONE							1
typedef float float32_t;

#define MOVE_TO_UPPER( int8_var ) 	(int8_var << 4)
#define MOVE_TO_LOWER( int8_var ) 	(int8_var >> 4)
#define LOWER_MASK( int8_var )		(int8_var & 0x0F)

#define PORTD_IRQ		62
#define PRIORITY_3		3
#define NVIC_global_enable_interrupts __enable_irq()

/*
 * The idea is to use pointer arithmetic ((&(var)+1)) to determine the offset of
 * the variable, and then subtract the original address of the variable, yielding
 *  its size. For example, if you have an int16_t i variable located at 0x0002, you
 *   would be subtracting 0x0002 from 0x0006, thereby obtaining 0x4 or 4 bytes.
 */
#define SIZEOF_VAR( var ) ((size_t)(&(var)+1)-(size_t)(&(var)))

#endif /* CONFIG_H_ */
