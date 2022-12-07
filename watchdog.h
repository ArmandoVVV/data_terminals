/*
 * watchdog.h
 * brief:
 * 		this source configures the watchdog, setups it refresh time
 * 		and can tell how many times the system was restarted.
 *
 *  Created on: Oct 2, 2022
 *      Author: Laura García
 */

#ifndef WATCHDOG_H_
#define WATCHDOG_H_

void setup_watchdog(void);
void refresh_watchdog(void);
uint16_t get_reset_count(void);

#endif /* WATCHDOG_H_ */
