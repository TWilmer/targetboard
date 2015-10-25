/*
 * @brief NXP LPC1769 LPCXpresso Sysinit file
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2013
 * All rights reserved.
 *
 * @par
 * Software that is described herein is for illustrative purposes only
 * which provides customers with programming information regarding the
 * LPC products.  This software is supplied "AS IS" without any warranties of
 * any kind, and NXP Semiconductors and its licensor disclaim any and
 * all warranties, express or implied, including all implied warranties of
 * merchantability, fitness for a particular purpose and non-infringement of
 * intellectual property rights.  NXP Semiconductors assumes no responsibility
 * or liability for the use of the software, conveys no license or rights under any
 * patent, copyright, mask work right, or any other intellectual property rights in
 * or to any products. NXP Semiconductors reserves the right to make changes
 * in the software without notification. NXP Semiconductors also makes no
 * representation or warranty that such application will be suitable for the
 * specified use without further testing or modification.
 *
 * @par
 * Permission to use, copy, modify, and distribute this software and its
 * documentation is hereby granted, under NXP Semiconductors' and its
 * licensor's relevant copyrights in the software, without fee, provided that it
 * is used in conjunction with NXP Semiconductors microcontrollers.  This
 * copyright, permission, and disclaimer notice must appear in all copies of
 * this code.
 */

#include "board.h"

/* The System initialization code is called prior to the application and
   initializes the board for run-time operation. Board initialization
   includes clock setup and default pin muxing configuration. */

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/* Pin muxing configuration */
STATIC const PINMUX_GRP_T pinmuxing[] = {
		{0,  25,  IOCON_MODE_INACT | IOCON_FUNC3},	/* TXD3 */
		{0,  26,  IOCON_MODE_INACT | IOCON_FUNC3},	/* RXD3 */

		{0,  23,   IOCON_MODE_INACT | IOCON_FUNC0},	/* LED1 */
		{3,  25,   IOCON_MODE_INACT | IOCON_FUNC0},	/* LED2 */
		{3,  26,   IOCON_MODE_INACT | IOCON_FUNC0},	/* LED3 */

		{2,  10,   IOCON_MODE_INACT | IOCON_FUNC0},	/* BUTTON */

		{2, 9, IOCON_MODE_INACT| IOCON_FUNC0}, /* Pullup for USB*/


		{1, 18, IOCON_MODE_INACT| IOCON_FUNC0}, /* IR LED driver*/


		{0, 24, IOCON_MODE_PULLUP| IOCON_FUNC0}, /* IR INPUT*/

		{0, 25, IOCON_MODE_INACT| IOCON_FUNC3}, /* TX3*/
		{0, 26, IOCON_MODE_INACT| IOCON_FUNC3}, /* RX3*/

		{0,  6,   IOCON_MODE_INACT | IOCON_FUNC0},	/* ESP_RST */
		{0,  4,   IOCON_MODE_INACT | IOCON_FUNC0},	/* ESP_0 */
		{0,  5,   IOCON_MODE_INACT | IOCON_FUNC0},	/* ESP_2 */


		{0,  2,   IOCON_MODE_INACT | IOCON_FUNC1},	/* TX0 */
	    {0,  3,   IOCON_MODE_INACT | IOCON_FUNC1},	/* RX0 */


		{2,  4,   IOCON_MODE_INACT | IOCON_FUNC0},	/* PWM1 */
		{1,  9,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM1A */
		{1,  8,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM1B */


		{2,  5,   IOCON_MODE_INACT | IOCON_FUNC0},	/* PWM2 */
		{1,  1,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM2A */
		{1,  0,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM2B */


		{1,  21,   IOCON_MODE_INACT | IOCON_FUNC0},	/* PWM3 */
		{0,  9,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM3A */
		{1,  10,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM3B */

		{1,  23,   IOCON_MODE_INACT | IOCON_FUNC0},	/* PWM4 */
		{1,  14,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM4A */
		{0,  8,   IOCON_MODE_INACT | IOCON_FUNC0},	/* CM4B */

		{1,  4,   IOCON_MODE_INACT | IOCON_FUNC0},	/* STBY */


};

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/* Sets up system pin muxing */
void Board_SetupMuxing(void)
{
	Chip_IOCON_SetPinMuxing(LPC_IOCON, pinmuxing, sizeof(pinmuxing) / sizeof(PINMUX_GRP_T));
}

/* Setup system clocking */
void Board_SetupClocking(void)
{
	Chip_SetupXtalClocking();

	/* Setup FLASH access to 4 clocks (100MHz clock) */
	Chip_SYSCTL_SetFLASHAccess(FLASHTIM_100MHZ_CPU);
}

/* Set up and initialize hardware prior to call to main */
void Board_SystemInit(void)
{
	Board_SetupMuxing();
	Board_SetupClocking();
}
