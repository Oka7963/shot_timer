/*
 * common.h
 *
 *  Created on: Jun 15, 2024
 *      Author: conan
 */

#ifndef SRC_COMMON_H_
#define SRC_COMMON_H_

/* Put common .h include here */
#include <stdbool.h>
#include <stdio.h>
#include "st7735.h"
#include "main.h" // pin defines and  #include "stm32f1xx_hal.h"


#define MAX_RECORD		256
#define MAX_SHOW_ON_SCREEN	8

typedef enum
{
	SYS_STATE_INITIALIZE=0,
	SYS_STATE_IDLE,
	SYS_STATE_START,
	SYS_STATE_COUNTING,
	SYS_STATE_END,
	SYS_STATE_NUM,
}SYS_STATE;

typedef struct
{
	SYS_STATE uSysState;
	bool bStartCount;			// start accept shot count
	bool bBuzzOn;				// start send buzz
	uint32_t uShotTickStart;	// shot count tick zero point
	uint8_t uShotCnt;			// lines of record
	uint32_t pShotRec[MAX_RECORD]; //all record
	uint32_t uCurrentTick;		// from start to present time

}scSystemState;

/* Common structure or type define for extern usage */
extern SPI_HandleTypeDef hspi1;
//extern TIM_HandleTypeDef htim4;
extern UART_HandleTypeDef huart2;
extern PCD_HandleTypeDef hpcd_USB_FS;
extern scSystemState SysState;


#endif /* SRC_COMMON_H_ */
