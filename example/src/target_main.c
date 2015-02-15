/*
 * @brief USB to UART bridge example
 *
 * @note
 * Copyright(C) NXP Semiconductors, 2012
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
#include <stdio.h>
#include <string.h>
#include "app_usbd_cfg.h"
#include "cdc_uart.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

static USBD_HANDLE_T g_hUsb;

extern const  USBD_HW_API_T hw_api;
extern const  USBD_CORE_API_T core_api;
extern const  USBD_CDC_API_T cdc_api;
/* Since this example only uses CDC class link functions for that clas only */
static const  USBD_API_T g_usbApi = {
	&hw_api,
	&core_api,
	0,
	0,
	0,
	&cdc_api,
	0,
	0x02221101,
};

const  USBD_API_T *g_pUsbApi = &g_usbApi;

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* Initialize pin and clocks for USB0/USB1 port */
static void usb_pin_clk_init(void)
{
	/* enable USB PLL and clocks */
	Chip_USB_Init();
	/* enable USB 1 port on the board */
	Board_USBD_Init(1);
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	Handle interrupt from USB0
 * @return	Nothing
 */
void USB_IRQHandler(void)
{
	USBD_API->hw->ISR(g_hUsb);
}

/* Find the address of interface descriptor for given class type. */
USB_INTERFACE_DESCRIPTOR *find_IntfDesc(const uint8_t *pDesc, uint32_t intfClass, int occ)
{
	USB_COMMON_DESCRIPTOR *pD;
	USB_INTERFACE_DESCRIPTOR *pIntfDesc = 0;
	uint32_t next_desc_adr;

	pD = (USB_COMMON_DESCRIPTOR *) pDesc;
	next_desc_adr = (uint32_t) pDesc;

	while (pD->bLength) {
		/* is it interface descriptor */
		if (pD->bDescriptorType == USB_INTERFACE_DESCRIPTOR_TYPE) {

			pIntfDesc = (USB_INTERFACE_DESCRIPTOR *) pD;
			/* did we find the right interface descriptor */
			if (pIntfDesc->bInterfaceClass == intfClass ) {
				if( occ==0)
			  	   break;
				else
					occ--;
			}
		}
		pIntfDesc = 0;
		next_desc_adr = (uint32_t) pD + pD->bLength;
		pD = (USB_COMMON_DESCRIPTOR *) next_desc_adr;
	}

	return pIntfDesc;
}

/**
 * @brief	main routine for blinky example
 * @return	Function should not exit.
 */
int main(void)
{
	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;

	/* Initialize board and chip */
	SystemCoreClockUpdate();
	Board_Init();

	  Chip_IOCON_PinMux(LPC_IOCON, 2, 9,  IOCON_MODE_INACT, IOCON_FUNC0);
	    Chip_GPIO_SetPinDIROutput(LPC_GPIO,2,9);
	    Chip_GPIO_SetPinOutLow(LPC_GPIO,2,9);//Pull up usb port as a slave.
	    //Settings for uart0
	    Chip_IOCON_PinMux(LPC_IOCON, 0, 2,  IOCON_MODE_INACT, IOCON_FUNC1);
	    Chip_GPIO_SetPinDIROutput(LPC_GPIO,0,2);//TXD0

	    Chip_IOCON_PinMux(LPC_IOCON, 0, 3,  IOCON_MODE_INACT, IOCON_FUNC1);
	    Chip_GPIO_SetPinDIRInput(LPC_GPIO,0,3);//RXD0



		Chip_UART_SetBaud(LPC_UART0, 115200);

		Chip_UART_ConfigData(LPC_UART0, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
		Chip_UART_TXEnable(LPC_UART0);


		Chip_Clock_EnablePeriphClock(SYSCTL_CLOCK_UART2);
	    //Settings for uart2 P0.10/P0.11
	    Chip_IOCON_PinMux(LPC_IOCON, 0, 10,  IOCON_MODE_INACT, IOCON_FUNC1);
	    Chip_GPIO_SetPinDIROutput(LPC_GPIO,0,10);//TXD2

	    Chip_IOCON_PinMux(LPC_IOCON, 0, 11,  IOCON_MODE_INACT, IOCON_FUNC1);
	    Chip_GPIO_SetPinDIRInput(LPC_GPIO,0,11);//RXD0



		Chip_UART_SetBaud(LPC_UART2, 115200);

		Chip_UART_ConfigData(LPC_UART2, UART_LCR_WLEN8 | UART_LCR_SBS_1BIT | UART_LCR_PARITY_DIS);
		Chip_UART_TXEnable(LPC_UART2);


	/* enable clocks and pinmux */
	usb_pin_clk_init();

	/* initialize call back structures */
	memset((void *) &usb_param, 0, sizeof(USBD_API_INIT_PARAM_T));
	usb_param.usb_reg_base = LPC_USB_BASE + 0x200;
	usb_param.max_num_ep =16;
	usb_param.mem_base = USB_STACK_MEM_BASE;
	usb_param.mem_size = USB_STACK_MEM_SIZE;

	/* Set the USB descriptors */
	desc.device_desc = (uint8_t *) &USB_DeviceDescriptor[0];
	desc.string_desc = (uint8_t *) &USB_StringDescriptor[0];
	/* Note, to pass USBCV test full-speed only devices should have both
	   descriptor arrays point to same location and device_qualifier set to 0.
	 */
	desc.high_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.full_speed_desc = (uint8_t *) &USB_FsConfigDescriptor[0];
	desc.device_qualifier = 0;

	/* USB Initialization */
	ret = USBD_API->hw->Init(&g_hUsb, &desc, &usb_param);
	if (ret == LPC_OK) {


		/* Init UCOM - USB to UART bridge interface */
		ret = UCOM_init(g_hUsb, &desc, &usb_param);
		if (ret == LPC_OK) {
			/* Make sure USB and UART IRQ priorities are same for this example */
			NVIC_SetPriority(USB_IRQn, 1);
			/*  enable USB interrupts */
			NVIC_EnableIRQ(USB_IRQn);
			NVIC_EnableIRQ(UART0_IRQn);
			NVIC_EnableIRQ(UART2_IRQn);
			/* now connect */
			USBD_API->hw->Connect(g_hUsb, 1);
		}
	}
	uint8_t g_rxBuff[256];
	uint32_t prompt = 0, rdCnt = 0;
	while (1) {
		if ((vcom_connected() != 0) && (prompt == 0)) {
					vcom_write("Hello World!!\r\n", 15);
					prompt = 1;
				}
				/* If VCOM port is opened echo whatever we receive back to host. */
				if (prompt) {
					rdCnt = vcom_bread(&g_rxBuff[0], 256);

					if (rdCnt) {

						vcom_write(&g_rxBuff[0], rdCnt);
					}
				}

		/* Sleep until next IRQ happens */
		__WFI();
	}
}
