/*
 * task_interrupt.c
 *
 *  Created on: Jun 15, 2024
 *      Author: conan
 */


#include "task_interrupt.h"

/* Global variable */
scSystemState SysState = {0};

/* local function */
const char *irq_TickToTimeAndCnt(uint32_t tick, uint8_t num)
{
	static char psHRMS[16];
	int ms = tick % 1000;
	int sec = tick / 1000;
	int min = sec / 60;
	sec = sec % 60;

	sprintf(psHRMS,"%02u-%02u:%02u.%03u",num+1,min,sec,ms); // +1 for first index should show as 0
	return (const char*)psHRMS;

}


void task_irq_BTN(void)
{
	static uint32_t uPrevShotBtn;
	uint32_t uBTNCnt = HAL_GetTick();

	if(uBTNCnt - uPrevShotBtn > BTN_DEBOUNCE)
	{
		/* debounce */
		printf("Event BTN\r\n");
		uPrevShotBtn = uBTNCnt;

		switch(SysState.uSysState)
		{
		default:
		case SYS_STATE_INITIALIZE:
		case SYS_STATE_END:
			/* do nothing state */
			printf("error -> I\r\n");
			SysState.uSysState = SYS_STATE_IDLE;
			break;
		case SYS_STATE_IDLE:
			printf("I->S\r\n");
			SysState.uSysState = SYS_STATE_START;
			SysState.bStartCount = true;
			SysState.bBuzzOn = true;
			break;
		case SYS_STATE_START:
		case SYS_STATE_COUNTING:
			printf("Cancel counting \r\n");
			SysState.uSysState = SYS_STATE_IDLE;
			break;
		}

		/* initialize screen and shot counters */
		SysState.uShotTickStart = uBTNCnt;
	    ST7735_FillScreen(ST7735_BLACK);
	    ST7735_WriteString(0, 0, "== Event BTN ==", Font_7x10, ST7735_WHITE, ST7735_BLACK);
	}

}

void irq_ShowShotRecord(void)
{
	/* uShotCnt start form 0, so at print, +10 */
	uint8_t printLine = 0;

	/* Max 15 data can be show */
	if(SysState.uShotCnt>MAX_SHOW_ON_SCREEN){
		/* scroll all data upward */
		for(printLine=1;printLine<=MAX_SHOW_ON_SCREEN;printLine++){
			ST7735_WriteString(0, 10*printLine, irq_TickToTimeAndCnt(SysState.pShotRec[printLine+SysState.uShotCnt-MAX_SHOW_ON_SCREEN],printLine+SysState.uShotCnt-MAX_SHOW_ON_SCREEN), Font_7x10, ST7735_WHITE, ST7735_BLACK);
		}
	}
	else
	{
		/* print as normal */
		printLine = SysState.uShotCnt+1;
		ST7735_WriteString(0, 10*printLine, irq_TickToTimeAndCnt(SysState.pShotRec[SysState.uShotCnt],SysState.uShotCnt), Font_7x10, ST7735_WHITE, ST7735_BLACK);
	}
	SysState.uShotCnt++;

}

void task_irq_ShotCount(void)
{
	static uint32_t uPrevShotTick;
	uint32_t uShotTick = HAL_GetTick();
	if(uShotTick-uPrevShotTick > SHOT_CONT_INTERVAL)
	{
		/* debounce */
		uPrevShotTick = uShotTick;

		switch(SysState.uSysState)
		{
		default:
		case SYS_STATE_INITIALIZE:
		case SYS_STATE_END:
		case SYS_STATE_IDLE:
		case SYS_STATE_START:
			/* do nothing state */
			break;
		case SYS_STATE_COUNTING:
			printf("Event ShotCount %s\r\n",irq_TickToTimeAndCnt(uShotTick-SysState.uShotTickStart,SysState.uShotCnt));
			ST7735_WriteString(0, 0, "== Shot Count ==", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			SysState.pShotRec[SysState.uShotCnt]=uShotTick-SysState.uShotTickStart;
			irq_ShowShotRecord();
			break;
		}


	}

}
