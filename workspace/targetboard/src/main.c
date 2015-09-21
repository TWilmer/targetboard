/*
 * main.c
 *
 *  Created on: 20-Sep-2015
 *      Author: thorsten
 */

#include "board.h"
#define TICKRATE_HZ1 (1)       /* 10 ticks per second */

void SysTick_Handler(void)
{
        Board_LED_Toggle(0);
        Board_LED_Toggle(1);
        Board_LED_Toggle(2);
}
static void main2(void);
void main()
{
	main2();
}
__attribute__ ((section(".text")))
static void main2(void)
{
	 SystemCoreClockUpdate();
	        Board_Init();

	        /* Enable and setup SysTick Timer at a periodic rate */
	        SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	        /* LEDs toggle in interrupt handlers */
	        while (1) {
	                __WFI();
	        }

}
