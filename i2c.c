/*
 * i2c.c
 *
 *  Created on: Oct 21, 2022
 *      Author: armando
 *      brief: this module has all the i2c possible communications, every i2c device
 *      passes by this module.
 */
#include "i2c.h"
#include "config.h"

static void set_error_flag(uint8_t value);

uint8_t g_master_txBuff[I2C_DATA_LENGTH];
uint8_t g_master_rxBuff[I2C_DATA_LENGTH];

i2c_master_transfer_t masterXfer;

static uint8_t g_error_flag = FALSE;

uint8_t get_error_flag(void){
	return g_error_flag;
}

static void set_error_flag(uint8_t value){
	g_error_flag = value;
}

uint8_t* get_master_rxBuff(void){
	return g_master_rxBuff;
}

uint8_t* get_master_txBuff(void){
	return g_master_txBuff;
}

void i2c_config(void){
	i2c_master_config_t masterConfig;
    uint32_t sourceClock;

    I2C_MasterGetDefaultConfig(&masterConfig);
    masterConfig.baudRate_Bps = I2C_BAUDRATE;

    sourceClock = I2C_MASTER_CLK_FREQ;
    I2C_MasterInit(I2C_MASTER, &masterConfig, sourceClock);
}

void i2c_write_indv(i2c_device_config_t device, uint8_t value){
	get_master_txBuff()[0] = value;

    masterXfer.slaveAddress   = device.address;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = (uint32_t)device.regAddress; //reg interno que se afecta
    masterXfer.subaddressSize = device.subaddressSize;
    masterXfer.data           = get_master_txBuff(); //direccion de arreglo
    masterXfer.dataSize       = device.dataSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    // si no la interacción con el dispositivo i2c no fue exitosa
    if( I2C_MasterTransferBlocking(I2C_MASTER, &masterXfer) ){
    	set_error_flag(TRUE);
    }
    else{
    	set_error_flag(FALSE);
    }
}

void i2c_read(i2c_device_config_t device){
	uint8_t registerAddress   = device.regAddress;

    masterXfer.slaveAddress   = device.address;
    masterXfer.direction      = kI2C_Read;
    masterXfer.subaddress     = (uint32_t)registerAddress;
    masterXfer.subaddressSize = device.subaddressSize;
    masterXfer.data           = get_master_rxBuff();
    masterXfer.dataSize       = device.dataSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    // si no la interacción con el dispositivo i2c no fue exitosa
    if(I2C_MasterTransferBlocking(I2C_MASTER, &masterXfer)){
    	set_error_flag(TRUE);
    }
    else{
    	set_error_flag(FALSE);
    }
}

void i2c_write_three_values(i2c_device_config_t device, uint8_t first_value, uint8_t second_value, uint8_t third_value){
	get_master_txBuff()[FIRST_VALUE] = first_value;
	get_master_txBuff()[SECOND_VALUE] = second_value;
	get_master_txBuff()[THIRD_VALUE] = third_value;

    masterXfer.slaveAddress   = device.address; //1101 000
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = (uint32_t)device.regAddress; //reg interno que se afecta
    masterXfer.subaddressSize = device.subaddressSize; //bytes del tamaño,  mem sería 2 bytes
    masterXfer.data           = get_master_txBuff(); //direccion de arreglo
    masterXfer.dataSize       = device.dataSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    // si no la interacción con el dispositivo i2c no fue exitosa
    if(I2C_MasterTransferBlocking(I2C_MASTER, &masterXfer)){
    	set_error_flag(TRUE);
    }
    else{
    	set_error_flag(FALSE);
    }

}

void i2c_write_eight_values(i2c_device_config_t device, uint8_t value1, uint8_t value2, uint8_t value3, uint8_t value4, uint8_t value5, uint8_t value6, uint8_t value7, uint8_t value8){
	get_master_txBuff()[FIRST_VALUE] = value1;
	get_master_txBuff()[SECOND_VALUE] = value2;
	get_master_txBuff()[THIRD_VALUE] = value3;
	get_master_txBuff()[FOURTH_VALUE] = value4;
	get_master_txBuff()[FIFTH_VALUE] = value5;
	get_master_txBuff()[SIXTH_VALUE] = value6;
	get_master_txBuff()[SEVENTH_VALUE] = value7;
	get_master_txBuff()[EIGTH_VALUE] = value8;

    masterXfer.slaveAddress   = device.address;
    masterXfer.direction      = kI2C_Write;
    masterXfer.subaddress     = (uint32_t)device.regAddress; //reg interno que se afecta
    masterXfer.subaddressSize = device.subaddressSize; //bytes del tamaño
    masterXfer.data           = get_master_txBuff(); //direccion de arreglo
    masterXfer.dataSize       = device.dataSize;
    masterXfer.flags          = kI2C_TransferDefaultFlag;

    // si no la interacción con el dispositivo i2c no fue exitosa
    if(I2C_MasterTransferBlocking(I2C_MASTER, &masterXfer)){
    	set_error_flag(TRUE);
    }
    else{
    	set_error_flag(FALSE);
    }
}


