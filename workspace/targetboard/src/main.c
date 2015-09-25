/*
 * main.c
 *
 *  Created on: 20-Sep-2015
 *      Author: thorsten
 */

#include "board.h"
#define TICKRATE_HZ1 (6)       /* 10 ticks per second */

void SysTick_Handler(void)
{
        Board_LED_Toggle(0);
        Board_LED_Toggle(1);
        Board_LED_Toggle(2);
}
static int main2(void);
int  main()
{
	return main2();
}
__attribute__ ((section(".text")))
static int main2(void)
{

	Board_SetupClocking();
		Chip_USB_Init();
	        Board_Init();
	        SystemCoreClockUpdate();

	        /* Enable and setup SysTick Timer at a periodic rate */
	        SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	        /* LEDs toggle in interrupt handlers */
	        while (1) {
	                __WFI();
	        }


	        return 1;

}
