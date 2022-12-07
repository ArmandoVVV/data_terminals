/*
 * dth20.c
 *
 *  Created on: Oct 21, 2022
 *      Author: armando
 *      brief: this module has all the necessary functions to use the DHT20
 *      device.
 */
#include "dht20.h"
#include "i2c.h"
#include "fsl_common.h"
#include "fsl_clock.h"
#include "config.h"

void dht_init(void){
	i2c_device_config_t dht_config;

	// config dht values
	dht_config.address = SENSOR_ADDRESS;
	dht_config.subaddressSize = ONE;
	dht_config.dataSize = ONE;
	dht_config.regAddress = ZERO_VALUE;

    SDK_DelayAtLeastUs(TEN_MS, CORE_CLOCK);

    // send reset command 0xAB
    i2c_write_indv(dht_config, 0xAB);

    // wait 10ms
    SDK_DelayAtLeastUs(TEN_MS, CORE_CLOCK);

    dht_config.dataSize = THREE_DATA;
    dht_config.subaddressSize = ZERO_VALUE;

    // send calibration commands
    i2c_write_three_values(dht_config, 0xE1, 0x08, 0x00);

    // read status byte until calibration bit = 1
    do{
    	dht_read_statusByte();
    }while( UNCALIBRATED == ((get_master_rxBuff()[0] >> CALIBRATION_BIT) & ONE) );
}

void dht_read_statusByte(void){
	static i2c_device_config_t dht_config;

	dht_config.address = SENSOR_ADDRESS;
	dht_config.subaddressSize = ONE;
	dht_config.dataSize = ONE;
	dht_config.regAddress = STATUS_REGISTER;

	i2c_read(dht_config);
}

void dht_send_readCommands(void){
	static i2c_device_config_t dht_config;

	dht_config.address = SENSOR_ADDRESS;
	dht_config.dataSize = THREE_DATA;
	dht_config.regAddress = ZERO_VALUE;
	dht_config.subaddressSize = ONE;

	// send read commands
	i2c_write_three_values(dht_config, 0xAC, 0x33, 0x00);
}

uint8_t dht_is_busy(void){
	dht_read_statusByte();

	return (get_master_rxBuff()[0] >> STATUS_BIT);
}

void dht_read(void){
	static i2c_device_config_t dht_config;

	dht_config.address = SENSOR_ADDRESS;
	dht_config.regAddress = STATUS_REGISTER;
	dht_config.subaddressSize = ONE;
	dht_config.dataSize = SEVEN_DATA;

	i2c_read(dht_config);
}

uint32_t get_humidity(void){
	uint32_t humidity = ZERO_VALUE;
	float32_t f_humidity = ZERO_VALUE;

	dht_read();

	humidity = get_master_rxBuff()[1] << 12;
	humidity = humidity | get_master_rxBuff()[2] << 4;
	humidity = humidity | MOVE_TO_LOWER(get_master_rxBuff()[3]);

	// RH% = (SRH / 2^20) * 100%
	f_humidity =  (humidity * 100) / BITN20_VALUE ;
	humidity = f_humidity;

	return humidity;
}

uint32_t get_temperature(void){
	uint32_t temperature = ZERO_VALUE;
	float32_t f_temperature = ZERO_VALUE;

	dht_read();

	temperature = LOWER_MASK(get_master_rxBuff()[3]) << 16;
	temperature = temperature | (get_master_rxBuff()[4] << 8);
	temperature = temperature | (get_master_rxBuff()[5]);

	// T[C] = (ST/ 2^20) * 200 - 50 ( 100 )
	f_temperature = (((temperature * 200) / BITN20_VALUE) - 50) * 100;
	temperature = f_temperature;

	temperature = (temperature / 100);

	return temperature;
}

