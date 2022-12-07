/*
 * AT24C32D.c
 *
 *  Created on: Nov 1, 2022
 *      Author: Laura García
 *      brief: this module has the functions to write and read all the required
 *      values on the memory device.
 *
 */

#include "AT24C32D.h"

void set_ROM_uart0(void){
	static rom_values_t rom;
	static uint32_t address_offset = ZERO_VALUE;

	rom.time = get_time();
	rom.date = get_date();
	rom.humidity = get_humidity();
	rom.temperature = get_temperature();

	static i2c_device_config_t rom_config = {
			.address        = ROM_ADDRESS,
			.dataSize       = ROM_DATASIZE,
			.subaddressSize = ROM_SUBADDRESS_SIZE
	};

	rom_config.regAddress = address_offset;

	i2c_write_eight_values(rom_config, rom.temperature, rom.humidity, rom.time.hours, rom.time.minutes, rom.time.seconds, rom.date.day, rom.date.month, rom.date.year);

	address_offset += ROM_DATASIZE;
}


rom_values_t get_ROM_uart0(void){
	static rom_values_t rom;
	static uint32_t address_offset = ZERO_VALUE;

	static i2c_device_config_t rom_config = {
			.address        = ROM_ADDRESS,
			.dataSize       = ROM_DATASIZE,
			.subaddressSize = ROM_SUBADDRESS_SIZE,
	};
	rom_config.regAddress = address_offset;

	i2c_read(rom_config);

	rom.temperature  = get_master_rxBuff()[0];
	rom.humidity     = get_master_rxBuff()[1];
	rom.time.hours   = get_master_rxBuff()[2];
	rom.time.minutes = get_master_rxBuff()[3];
	rom.time.seconds = get_master_rxBuff()[4];
	rom.date.day     = get_master_rxBuff()[5];
	rom.date.month   = get_master_rxBuff()[6];
	rom.date.year    = get_master_rxBuff()[7];

	address_offset += ROM_DATASIZE;

	return rom;
}

// uart4 uses an address offset to separate its memory from uart0
rom_values_t get_ROM_uart4(void){
	static rom_values_t rom;
	static uint32_t address_offset = UART4_ADDRESS_OFFSET;

	static i2c_device_config_t rom_config = {
			.address        = ROM_ADDRESS,
			.dataSize       = ROM_DATASIZE,
			.subaddressSize = ROM_SUBADDRESS_SIZE,
	};
	rom_config.regAddress = address_offset;

	// disable interrupts to protect overwriting on master_rxBuff
	NVIC_DisableIRQ(PORTD_IRQ);

	i2c_read(rom_config);

	rom.temperature  = get_master_rxBuff()[0];
	rom.humidity     = get_master_rxBuff()[1];
	rom.time.hours   = get_master_rxBuff()[2];
	rom.time.minutes = get_master_rxBuff()[3];
	rom.time.seconds = get_master_rxBuff()[4];
	rom.date.day     = get_master_rxBuff()[5];
	rom.date.month   = get_master_rxBuff()[6];
	rom.date.year    = get_master_rxBuff()[7];

	NVIC_EnableIRQ(PORTD_IRQ);

	address_offset += ROM_DATASIZE;

	return rom;
}

// uart4 uses an address offset to separate its memory from uart0
void set_ROM_uart4(void){
	static rom_values_t rom;
	static uint32_t address_offset = UART4_ADDRESS_OFFSET;

	rom.time = get_time();
	rom.date = get_date();
	rom.humidity = get_humidity();
	rom.temperature = get_temperature();

	static i2c_device_config_t rom_config = {
			.address        = ROM_ADDRESS,
			.dataSize       = ROM_DATASIZE,
			.subaddressSize = ROM_SUBADDRESS_SIZE
	};

	rom_config.regAddress = address_offset;

	i2c_write_eight_values(rom_config, rom.temperature, rom.humidity, rom.time.hours, rom.time.minutes, rom.time.seconds, rom.date.day, rom.date.month, rom.date.year);

	address_offset += ROM_DATASIZE;
}


