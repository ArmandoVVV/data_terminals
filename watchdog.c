/*
 * watchdog.c
 * brief:
 * 		this source configures the watchdog, setups it refresh time
 * 		and can tell how many times the system was restarted.
 *
 *  Created on: Oct 2, 2022
 *      Author: Laura García
 */
#include "fsl_wdog.h"
#include "fsl_rcm.h"
#include "watchdog.h"

#define WDOG_WCT_INSTRUCITON_COUNT (256U)
#define THREE_SECONDS 0xBB8

WDOG_Type *wdog_base = WDOG;
wdog_config_t config;
RCM_Type *rcm_base   = RCM;

static inline uint32_t GetTimerOutputValue(WDOG_Type *base);
static void WaitWctClose(WDOG_Type *base);

uint16_t get_reset_count(void){
	return WDOG_GetResetCount(wdog_base);
}

void setup_watchdog(void){
	if (!(RCM_GetPreviousResetSources(rcm_base) & kRCM_SourceWdog)){
		WDOG_ClearResetCount(wdog_base);
	}
	WDOG_GetDefaultConfig(&config);
	config.timeoutValue = THREE_SECONDS;
    WDOG_Init(wdog_base, &config);
    WaitWctClose(wdog_base);
}

void refresh_watchdog(void){
	WDOG_Refresh(wdog_base);
    while (GetTimerOutputValue(wdog_base) < (config.timeoutValue >> 3U)){ // esto va siempre del feed / refresh
    }
}


static void WaitWctClose(WDOG_Type *base)
{
    /* Accessing register by bus clock */
    for (uint32_t i = 0; i < WDOG_WCT_INSTRUCITON_COUNT; i++)
    {
        (void)base->RSTCNT;
    }
}

static inline uint32_t GetTimerOutputValue(WDOG_Type *base)
{
    return (uint32_t)((((uint32_t)base->TMROUTH) << 16U) | (base->TMROUTL));
}
