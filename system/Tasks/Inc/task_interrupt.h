/*
 * task_interrupt.h
 *
 *  Created on: Jun 15, 2024
 *      Author: conan
 */

#ifndef TASK_INTERRUPT_H_
#define TASK_INTERRUPT_H_

#include "common.h"
#include "st7735.h"

/* Defines for constant */
#define SHOT_CONT_INTERVAL		50 //(ms)
#define BTN_DEBOUNCE			50 //ms

/* Callback Functions */
void task_irq_ShotCount(void);
void task_irq_BTN(void);




#endif /* TASK_INTERRUPT_H_ */
