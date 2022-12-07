/*
 * rtc.c
 *	The rtc source file contains the configuration of the I2C with the RTC device
 *	it has its read and write functions
 *  Created on: Oct 12, 2022
 *      Author: Laura Garcia
 */

#include <rtc.h>
#include "fsl_port.h"
#include "config.h"
#include "i2c.h"

    /*
     * WARNING: INITIAL SHOWN CLOCK VALUES ON TERA TERM DEPENDS ON RTC's
     *  		REGISTER VALUES
     */
void rtc_init(void){
	static const i2c_device_config_t rtc_config = {
		.address = RTC_ADDRESS,
		.dataSize = ONE,
		.subaddressSize = ONE,
		.regAddress = CONTROL_ADDRESS

	};

	set_time(get_time());
    set_date(get_date());
    i2c_write_indv(rtc_config, ENABLE_SQW);
}

void set_time(clock_values_t time){
	static const i2c_device_config_t rtc_config = {
			.address = RTC_ADDRESS,
			.dataSize = I2C_CLOCK_DATASIZE,
			.regAddress = SECONDS_ADDRESS,
			.subaddressSize = ONE
	};

    i2c_write_three_values(rtc_config, time.seconds, time.minutes, time.hours);

}

void set_date(date_values_t date){
	static const i2c_device_config_t rtc_config = {
			.address = RTC_ADDRESS,
			.dataSize = I2C_CLOCK_DATASIZE,
			.regAddress = DATE_ADDRESS,
			.subaddressSize = ONE
	};

    i2c_write_three_values(rtc_config, date.day, date.month, date.year);
}

date_values_t get_date(void){
	date_values_t date;
	static const i2c_device_config_t rtc_config = {
			.address = RTC_ADDRESS,
			.dataSize = I2C_DATA_LENGTH - 1U,
			.regAddress = ZERO_VALUE,
			.subaddressSize = SUBADDRESS_SIZE
	};

	i2c_read(rtc_config);

	date.day = get_master_rxBuff()[DAY];
	date.month = get_master_rxBuff()[MONTH];
	date.year = get_master_rxBuff()[YEAR];

	return date;
}

clock_values_t get_time(void){
	clock_values_t time;
	static const i2c_device_config_t rtc_config = {
			.address = RTC_ADDRESS,
			.dataSize = I2C_DATA_LENGTH - 1U,
			.regAddress = ZERO_VALUE,
			.subaddressSize = SUBADDRESS_SIZE
	};

	i2c_read(rtc_config);

	time.seconds = get_master_rxBuff()[SECONDS];
	time.minutes = get_master_rxBuff()[MINUTES];
	time.hours = get_master_rxBuff()[HOURS];

	return time;
}

