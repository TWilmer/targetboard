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

	//Board_LED_Toggle(3);
	//Board_LED_Toggle(1);
	//Board_LED_Toggle(2);
}


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

uint32_t values[500]={0};
uint32_t values_low[500]={0};
uint32_t values_high[500]={0};
uint32_t valuepos=0;
uint32_t valuelowpos=0;
uint32_t valuehighpos=0;
uint32_t lowMin=0xFFFFFFFF;
uint32_t lowMax=0;
uint32_t highMin=0xFFFFFFFF;
uint32_t highMax=0;
uint32_t start=0;
uint32_t received=0;
uint32_t receivedNow=0;
uint32_t receivedValues[32];
uint32_t receivedValuesPos=0;
uint32_t bitsrecevied[500]={0};
uint32_t bitsreceviedPos=0;
uint32_t bitCount;
void EINT3_IRQHandler(void)
{
	uint32_t curValue=0;

	if(Chip_GPIOINT_IsIntPending(LPC_GPIOINT, 0))
	{

		int falling=(Chip_GPIOINT_GetStatusFalling(LPC_GPIOINT,0) & (1<<24));
		int rising=(Chip_GPIOINT_GetStatusRising(LPC_GPIOINT,0) & (1<<24));
		if(  falling &&
				rising
		)
		{

		}else {
			curValue=Chip_TIMER_ReadCount(LPC_TIMER0);
			Chip_TIMER_Reset(LPC_TIMER0);
			Chip_TIMER_Enable(LPC_TIMER0);
			if(falling)
			{

				Chip_GPIOINT_SetIntRising(LPC_GPIOINT,GPIOINT_PORT0,1<<24);

				if(start==1)
				{
					received <<=1;
					bitCount++;
					if(curValue > 14722)
					{
						received|=1;
					}
					values_low[valuelowpos]=curValue;
					valuelowpos++;
					if(valuelowpos>=500)
						valuelowpos=499;
				}

			}else if(rising)
			{
				if(start==1)
				{
					received <<=1;
					bitCount++;
					if(curValue > 14722)
					{
						received|=1;
					}
					values_low[valuelowpos]=curValue;
					valuelowpos++;
					if(valuelowpos>=500)
						valuelowpos=499;
				}

				// LG remoteif(curValue>208468 && curValue<900000)
				if(curValue>60433 && curValue<900000)
				{
					received=0;
					start=1;
					bitCount=0;

			    // 	Board_LED_Set(1, 1);
				}



			}
			if(  bitCount==33 || bitCount==35)
			{
				receivedNow=received;
				receivedValues[receivedValuesPos]=received;
				if(receivedValuesPos<31)
					receivedValuesPos++;

			}

			if(start==1)
			{
				values[valuepos]=curValue;
								if(valuepos<499)
									valuepos++;

			}
		}


		Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 0,1<<24);


	}
	NVIC_ClearPendingIRQ(EINT3_IRQn);

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


int main(void)
{

	USBD_API_INIT_PARAM_T usb_param;
	USB_CORE_DESCS_T desc;
	ErrorCode_t ret = LPC_OK;
	Board_Init();
	Board_SetupClocking();
	Chip_USB_Init();

	SystemCoreClockUpdate();

	/* Enable and setup SysTick Timer at a periodic rate */
	SysTick_Config(SystemCoreClock / TICKRATE_HZ1);

	/* enable clocks and pinmux */
	usb_pin_clk_init();




	DEBUGSTR("Test");
	while(1);

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




	Chip_GPIO_SetPinDIROutput(LPC_GPIO,2,9);
	Chip_GPIO_SetPinOutLow(LPC_GPIO,2,9);//Pull up usb port as a slave.
	Chip_GPIO_SetPinDIROutput(LPC_GPIO,1,18);



	// Stuff for IR, enable Pin I/O Interrupt
	Chip_GPIOINT_Init(LPC_GPIOINT);
	NVIC_SetPriority(EINT3_IRQn, 1);
	Chip_GPIOINT_SetIntFalling(LPC_GPIOINT,GPIOINT_PORT0,1<<24);
	//Chip_GPIOINT_SetIntRising(LPC_GPIOINT,GPIOINT_PORT0,1<<24);
	Chip_GPIOINT_ClearIntStatus(LPC_GPIOINT, 0,1<<24);
	NVIC_ClearPendingIRQ(EINT3_IRQn);
	 NVIC_EnableIRQ(EINT3_IRQn);
	 Chip_TIMER_Init(LPC_TIMER0);

	 for(int i=0;i<500;i++)
	 {
	 values[i]=0;
	  values_low[i]=0;
	 values_high[i]=0;
	 }
	 start=0;

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
	Board_UARTPutSTR("ABC\n");
	uint8_t g_rxBuff[256];
	uint32_t prompt = 0, rdCnt = 0;


	//ESP into reset
	Chip_GPIO_SetPinState(	LPC_GPIO, 0 ,6 ,0);
	__enable_irq();
	int count=0;
	bitsreceviedPos=0;
	while (1) {

		switch(receivedNow)
		{
		case 0x418030E7:
		case 0x4300C39D:
			count=1000000;
			Board_LED_Set(0, 1);
			break;
		case 1124123539:
		case 100713363:
			Board_LED_Set(1, 1);
			count=1000000;
			break;
		case 1124123545:
		case 100713369:
			Board_LED_Set(2, 1);
			count=1000000;
			break;
		}
		receivedNow=0;
		if(count>0)
		{
			count--;
		}else{
			Board_LED_Set(0, 0);
			Board_LED_Set(1, 0);
			Board_LED_Set(2, 0);
		}

	}
}
