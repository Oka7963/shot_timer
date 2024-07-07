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
	/* we only use ss:mmm */
	static char psHRMS[16];
	int ms = tick % 1000;
	int sec = tick / 1000;
	int min = sec / 60;
	sec = sec % 60;

//	sprintf(psHRMS,"%02u-%02u:%02u.%03u",num+1,min,sec,ms); // +1 for first index should show as 0
	sprintf(psHRMS,"%02u | +%02u.%03u",num+1,sec,ms); // +1 for first index should show as 0
	return (const char*)psHRMS;
}

const char *irq_TickToTimeUpdate(uint32_t tick)
{
	static char psHRMS[16];
	int ms = tick % 1000;
	int sec = tick / 1000;
	int min = sec / 60;
	sec = sec % 60;

	sprintf(psHRMS,"%02u:%02u.%03u",min,sec,ms); // +1 for first index should show as 0
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
	/* print and scroll the record */
	/* uShotCnt start form 0, so at print, +10 */

	/*
	 *  text 7x10
	 *  time 16*26
	 *
	 *      128
	 *  +-----------+
	 *
	 *    90
	 *    dynamic
	 *    line
	 *					160
	 *    26 empty
	 *    18 current time
	 *    26 empty
	 *  +------------+
	 *
	 *
	 *
	 *  print like this
	 *
	 *  rec n
	 *  rec n+1
	 *  rec n+2
	 *   ....
	 *
	 * */

	uint8_t printLine = 0;

	/* Max data can be shown */
	if(SysState.uShotCnt>=MAX_SHOW_ON_SCREEN){
		/* scroll all data upward */
		for(printLine=1;printLine<=MAX_SHOW_ON_SCREEN;printLine++){
			/* x, y
			 * rec string
			 * font, txt color, bg color
			 *  */
			/* Left column */
			ST7735_WriteString(0, 10*printLine,
						irq_TickToTimeAndCnt(SysState.pShotRec[printLine+SysState.uShotCnt-MAX_SHOW_ON_SCREEN],printLine+SysState.uShotCnt-MAX_SHOW_ON_SCREEN),
						Font_7x10, ST7735_WHITE, ST7735_BLACK);
//			/* Right column */
//			ST7735_WriteString(9*7, 10*printLine,
//						irq_TickToTimeAndCnt(SysState.pShotRec[printLine+SysState.uShotCnt-MAX_SHOW_ON_SCREEN+8],printLine+SysState.uShotCnt-MAX_SHOW_ON_SCREEN+8),
//						Font_7x10, ST7735_WHITE, ST7735_BLACK);

		}
	}
	else
	{
		/* print as normal */
		printLine = SysState.uShotCnt+1;
		ST7735_WriteString(0, 10*printLine, irq_TickToTimeAndCnt(SysState.pShotRec[SysState.uShotCnt],SysState.uShotCnt), Font_7x10, ST7735_WHITE, ST7735_BLACK);
	}

	/* print current timer with min */
	ST7735_WriteString(0, 90+26, irq_TickToTimeUpdate(SysState.uCurrentTick), Font_11x18, ST7735_WHITE, ST7735_BLACK);

}

void task_irq_ShotCount(void)
{
	/* record the shot and its time */
	static uint32_t uPrevShotTick;
	uint32_t uShotTick = HAL_GetTick();
	if(uShotTick-uPrevShotTick > SHOT_CONT_INTERVAL)
	{
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
			/* SysState.pShotRec[SysState.uShotCnt] => current tick - last tick
			 * SysState.pShotRec[SysState.uShotCnt-1]	=> last tick
			 * SysState.uCurrentTick				=> current tick - start tick
			 * uShotTick							=> current tick
			 * SysState.uShotTickStart				=> start tick */
			ST7735_WriteString(0, 0, "== Shot Count ==", Font_7x10, ST7735_WHITE, ST7735_BLACK);
			if(SysState.uShotCnt==0)
			{
				SysState.pShotRec[SysState.uShotCnt]=uShotTick-SysState.uShotTickStart;
				SysState.uCurrentTick = uShotTick-SysState.uShotTickStart;
			}
			else
			{
				SysState.pShotRec[SysState.uShotCnt]=uShotTick-uPrevShotTick;
				SysState.uCurrentTick = uShotTick-SysState.uShotTickStart;
			}

			printf("Event ShotCount %s - %s|",irq_TickToTimeAndCnt(SysState.pShotRec[SysState.uShotCnt],SysState.uShotCnt),irq_TickToTimeUpdate(SysState.pShotRec[SysState.uShotCnt-1]));
			printf("diff %s|",irq_TickToTimeUpdate(SysState.pShotRec[SysState.uShotCnt]));
			printf("current time %s\r\n",irq_TickToTimeUpdate(SysState.uCurrentTick));

			irq_ShowShotRecord();

			/* move to next index */
			SysState.uShotCnt++;
			/* debounce, put here so ShotCnt won't effect by multiple irq entry */
			uPrevShotTick = uShotTick;

			break;
		}


	}

}
