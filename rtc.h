/*
 * rtc.h
 *
 *  Created on: Oct 12, 2022
 *      Author: Laura Garcia
 */

#ifndef RTC_H_
#define RTC_H_

#include "stdint.h"

#define SQ_ADDRESS					0x07
#define ENABLE_SQW					0x10

#define SQW							0

#define HOUR_TEN					0
#define HOUR_UNIT					1
#define MIN_TEN						3
#define MIN_UNIT					4
#define SEC_TEN						6
#define SEC_UNIT					7

typedef struct{
	uint8_t seconds;
	uint8_t minutes;
	uint8_t hours;
}clock_values_t;


typedef struct{
	uint8_t day;
	uint8_t month;
	uint8_t year;
}date_values_t;


typedef enum{
	SECONDS_ADDRESS = 0x00,
	MINUTES_ADDRESS = 0x01,
	HOURS_ADDRESS = 0x02,
	DAY_ADDRESS = 0x03,
	DATE_ADDRESS = 0x04,
	MONTH_ADDRESS = 0x05,
	YEAR_ADDRESS = 0x06,
	CONTROL_ADDRESS = 0x07
}rtc_address_t;


void rtc_init(void);

void set_time(clock_values_t time);
void set_date(date_values_t date);

date_values_t get_date(void);
clock_values_t get_time(void);

#endif /* RTC_H_ */
