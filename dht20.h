/*
 * dht20.h
 *
 *  Created on: Oct 21, 2022
 *      Author: armando
 *      brief: this module has all the necessary functions to use the DHT20
 *      device.
 */

#ifndef DHT20_H_
#define DHT20_H_

#include "stdint.h"
#include "config.h"

#define UNCALIBRATED 		0
#define CALIBRATED 			1
#define CALIBRATION_BIT 	3

#define NOT_BUSY			0
#define BUSY				1
#define STATUS_BIT			7

#define BITN20_VALUE		(1 << 20)

#define CORE_CLOCK 21000000u
#define TEN_MS	   100000u

#define STATUS_REGISTER		0x71
#define THREE_DATA			3
#define SEVEN_DATA			7

void dht_init(void);

uint8_t dht_is_busy(void);
void dht_read_statusByte(void);
void dht_send_readCommands(void);

void dht_read(void);

uint32_t get_humidity(void);
uint32_t get_temperature(void);

#endif /* DHT20_H_ */



