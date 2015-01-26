/*
 * @brief UART Comm port call back routines
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
#include <string.h>
#include "board.h"
#include "app_usbd_cfg.h"
#include "cdc_uart.h"

/*****************************************************************************
 * Private types/enumerations/variables
 ****************************************************************************/



/* Ring buffer size */
#define UCOM_BUF_SZ         64			/* The size should always be 2^n type.*/
#define UCOM_TX_CONNECTED   _BIT(8)		/* connection state is for both RX/Tx */
#define UCOM_TX_BUSY        _BIT(0)

#define UCOM_RX_UART_DIS    _BIT(0)
#define UCOM_RX_BUF_FULL    _BIT(1)
#define UCOM_RX_BUF_QUEUED  _BIT(2)
#define UCOM_RX_DB_QUEUED   _BIT(3)

/**
 * Structure containing Virtual Comm port control data
 */
typedef struct UCOM_DATA {
	USBD_HANDLE_T hUsb;		/*!< Handle to USB stack */
	USBD_HANDLE_T hCdc;		/*!< Handle to CDC class controller */

	uint8_t *rxBuf;			/*!< UART Rx buffer */
	uint8_t *txBuf;			/*!< UART Tx buffer */
	uint8_t txBuf_uartIndex;/*!< UART index in Tx buffer */
	int8_t txBuf_count;		/*!< Bytes present in Tx buffer */
	uint8_t rxBuf_uartIndex;/*!< UART index in Rx buffer */
	uint8_t rxBuf_usbIndex;	/*!< USB index in Rx buffer */

	volatile uint8_t usbTxBusy;		/*!< USB is busy sending previous packet */
	volatile uint8_t usbRxPending;	/*!< USB packet is pending in EP buffer */

	volatile uint16_t tx_flags;
	volatile uint16_t rx_flags;
	uint16_t rx_count;
	uint16_t rx_rd_count;

	LPC_USART_T *selected;
	uint8_t inEndpoint;
	uint8_t outEndpoint;

	 USB_EP_HANDLER_T pInHdlr;
	 USB_EP_HANDLER_T pOutHdlr;

} UCOM_DATA_T;

/** Virtual Comm port control data instance. */
static UCOM_DATA_T g_uCOM1;

static UCOM_DATA_T g_uCOM2;
static UCOM_DATA_T g_uCOM3;
static UCOM_DATA_T g_uCOM4;
static UCOM_DATA_T g_uCOM5;

// FIXME: This needs to be selected based on board type.
//
#define SELECTED_UART LPC_UART0

/*****************************************************************************
 * Public types/enumerations/variables
 ****************************************************************************/

/*****************************************************************************
 * Private functions
 ****************************************************************************/

/* UART port init routine */
static void UCOM_UartInit(int i)
{
	return;
	/* Pin-muxing done in board init. */
	//Init_UART_PinMux();

	Chip_UART_Init(SELECTED_UART);
	Chip_UART_SetBaud(SELECTED_UART, 115200);
	Chip_UART_ConfigData(SELECTED_UART, (UART_LCR_WLEN8 | UART_LCR_SBS_1BIT));
	Chip_UART_SetupFIFOS(SELECTED_UART, (UART_FCR_FIFO_EN | UART_FCR_TRG_LEV2));
	Chip_UART_TXEnable(SELECTED_UART);

	/* Enable receive data and line status interrupt */
	Chip_UART_IntEnable(SELECTED_UART, (UART_IER_RBRINT | UART_IER_RLSINT));

	/* Enable Interrupt for UART channel */
	/* Priority = 1 */
	if(i==0)
	{
		NVIC_SetPriority(UART0_IRQn, 1);
		/* Enable Interrupt for UART channel */
		NVIC_EnableIRQ(UART0_IRQn);
	}
	if(i==1)
	{
		NVIC_SetPriority(UART2_IRQn, 1);
		/* Enable Interrupt for UART channel */
		NVIC_EnableIRQ(UART2_IRQn);

	}
}
static ErrorCode_t UCOM_bulk_in_hdlr5(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{

	return LPC_OK;
}
/* UCOM bulk EP_IN and EP_OUT endpoints handler */
static ErrorCode_t UCOM_bulk_hdlr5(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return LPC_OK;
}
static ErrorCode_t UCOM_bulk_in_hdlr4(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{

	return LPC_OK;
}
/* UCOM bulk EP_IN and EP_OUT endpoints handler */
static ErrorCode_t UCOM_bulk_hdlr4(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return LPC_OK;
}

static ErrorCode_t UCOM_bulk_in_hdlr3(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{

	return LPC_OK;
}
/* UCOM bulk EP_IN and EP_OUT endpoints handler */
static ErrorCode_t UCOM_bulk_hdlr3(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return LPC_OK;
}

static ErrorCode_t UCOM_bulk_in_hdlr2(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{

	return LPC_OK;
}
/* UCOM bulk EP_IN and EP_OUT endpoints handler */
static ErrorCode_t UCOM_bulk_hdlr2(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	return LPC_OK;
}
static ErrorCode_t UCOM_bulk_in_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	UCOM_DATA_T *pUcom = (UCOM_DATA_T *) data;
	if (event == USB_EVT_IN) {
		pUcom->tx_flags &= ~VCOM_TX_BUSY;
	}
	return LPC_OK;
}
/* UCOM bulk EP_IN and EP_OUT endpoints handler */
static ErrorCode_t UCOM_bulk_hdlr(USBD_HANDLE_T hUsb, void *data, uint32_t event)
{
	UCOM_DATA_T *pUcom = (UCOM_DATA_T *) data;
	switch (event) {
	case USB_EVT_OUT:
		pUcom->rx_count = USBD_API->hw->ReadEP(hUsb, pUcom->outEndpoint, pUcom->rxBuf);
		if (pUcom->rx_flags & VCOM_RX_BUF_QUEUED) {
			pUcom->rx_flags &= ~VCOM_RX_BUF_QUEUED;
			if (pUcom->rx_count != 0) {
				pUcom->rx_flags |= VCOM_RX_BUF_FULL;
			}

		}
		else if (pUcom->rx_flags & VCOM_RX_DB_QUEUED) {
			pUcom->rx_flags &= ~VCOM_RX_DB_QUEUED;
			pUcom->rx_flags |= VCOM_RX_DONE;
		}
		break;

	case USB_EVT_OUT_NAK:
		/* queue free buffer for RX */
		if ((pUcom->rx_flags & (VCOM_RX_BUF_FULL | VCOM_RX_BUF_QUEUED)) == 0) {
			USBD_API->hw->ReadReqEP(hUsb, pUcom->outEndpoint, pUcom->rxBuf, VCOM_RX_BUF_SZ);
			pUcom->rx_flags |= VCOM_RX_BUF_QUEUED;
		}
		break;

	default:
		break;
	}
	return LPC_OK;
	//UCOM_DATA_T *pUcom = (UCOM_DATA_T *) data;
	uint32_t count = 0;

	switch (event) {
	/* A transfer from us to the USB host that we queued has completed. */
	case USB_EVT_IN:
		if ((g_uCOM1.usbTxBusy == 0) && USB_IsConfigured(g_uCOM1.hUsb)) {
			g_uCOM1.usbTxBusy = 1;
			count = USBD_API->hw->WriteEP(g_uCOM1.hUsb, g_uCOM1.inEndpoint,0, 5);
			g_uCOM1.rxBuf_usbIndex = 5 & (UCOM_BUF_SZ - 1);
		}
		if ((g_uCOM2.usbTxBusy == 0) && USB_IsConfigured(g_uCOM2.hUsb)) {
			g_uCOM2.usbTxBusy = 1;
			count = USBD_API->hw->WriteEP(g_uCOM2.hUsb, g_uCOM1.inEndpoint,0, 5);
			g_uCOM2.rxBuf_usbIndex = 5 & (UCOM_BUF_SZ - 1);
		}

		break;
		/* check if UART had more data to send */
		if (pUcom->rxBuf_uartIndex < pUcom->rxBuf_usbIndex) {
			count = UCOM_BUF_SZ - pUcom->rxBuf_usbIndex;
		}
		else {
			count = pUcom->rxBuf_uartIndex - pUcom->rxBuf_usbIndex;
		}
		if (count) {
			pUcom->usbTxBusy = 1;
			count = USBD_API->hw->WriteEP(pUcom->hUsb, pUcom->inEndpoint, &pUcom->rxBuf[pUcom->rxBuf_usbIndex], count);
			pUcom->rxBuf_usbIndex = (pUcom->rxBuf_usbIndex + count) & (UCOM_BUF_SZ - 1);
		}
		else {
			pUcom->usbTxBusy = 0;
		}
		break;

		/* We received a transfer from the USB host . */
	case USB_EVT_OUT:
		if ((Chip_UART_GetIntsEnabled(SELECTED_UART) & UART_IER_THREINT) == 0) {
			pUcom->txBuf_count = USBD_API->hw->ReadEP(hUsb, pUcom->outEndpoint, pUcom->txBuf);
			pUcom->txBuf_uartIndex = 0;
			/* kick start UART tranmission */
			pUcom->txBuf_uartIndex = Chip_UART_Send(SELECTED_UART,
					&pUcom->txBuf[pUcom->txBuf_uartIndex],
					pUcom->txBuf_count);
			pUcom->txBuf_count -= pUcom->txBuf_uartIndex;
			/* Enable UART transmit interrupt */
			Chip_UART_IntEnable(SELECTED_UART, UART_IER_THREINT);
		}
		else {
			pUcom->usbRxPending++;
		}
		break;

	default:
		break;
	}

	return LPC_OK;
}
static ErrorCode_t UCOM_SetLineCode1(USBD_HANDLE_T hCDC, CDC_LINE_CODING *line_coding)
{
	g_uCOM1.tx_flags= VCOM_TX_CONNECTED;
	return LPC_OK;
}
/* Set line coding call back routine */
static ErrorCode_t UCOM_SetLineCode(USBD_HANDLE_T hCDC, CDC_LINE_CODING *line_coding)
{
	uint32_t config_data = 0;


	g_uCOM2.tx_flags= VCOM_TX_CONNECTED;

	switch (line_coding->bDataBits) {
	case 5:
		config_data |= UART_LCR_WLEN5;
		break;

	case 6:
		config_data |= UART_LCR_WLEN6;
		break;

	case 7:
		config_data |= UART_LCR_WLEN7;
		break;

	case 8:
	default:
		config_data |= UART_LCR_WLEN8;
		break;
	}

	switch (line_coding->bCharFormat) {
	case 1:	/* 1.5 Stop Bits */
		/* In the UART hardware 1.5 stop bits is only supported when using 5
		 * data bits. If data bits is set to 5 and stop bits is set to 2 then
		 * 1.5 stop bits is assumed. Because of this 2 stop bits is not support
		 * when using 5 data bits.
		 */
		if (line_coding->bDataBits == 5) {
			config_data |= UART_LCR_SBS_2BIT;
		}
		else {
			return ERR_USBD_UNHANDLED;
		}
		break;

	case 2:	/* 2 Stop Bits */
		/* In the UART hardware if data bits is set to 5 and stop bits is set to 2 then
		 * 1.5 stop bits is assumed. Because of this 2 stop bits is
		 * not support when using 5 data bits.
		 */
		if (line_coding->bDataBits != 5) {
			config_data |= UART_LCR_SBS_2BIT;
		}
		else {
			return ERR_USBD_UNHANDLED;
		}
		break;

	default:
	case 0:	/* 1 Stop Bit */
		config_data |= UART_LCR_SBS_1BIT;
		break;
	}

	switch (line_coding->bParityType) {
	case 1:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_ODD);
		break;

	case 2:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_EVEN);
		break;

	case 3:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_F_1);
		break;

	case 4:
		config_data |= (UART_LCR_PARITY_EN | UART_LCR_PARITY_F_0);
		break;

	default:
	case 0:
		config_data |= UART_LCR_PARITY_DIS;
		break;
	}

	if (line_coding->dwDTERate < 3125000) {
		Chip_UART_SetBaud(SELECTED_UART, line_coding->dwDTERate);
	}
	Chip_UART_ConfigData(SELECTED_UART, config_data);

	return LPC_OK;
}

/*****************************************************************************
 * Public functions
 ****************************************************************************/

/**
 * @brief	UART interrupt handler sub-routine
 * @return	Nothing
 */
void UART0_IRQHandler(void)
{
	return ;

	uint32_t count = 0;
	/* Handle transmit interrupt if enabled */
	if (g_uCOM1.selected->IER & UART_IER_THREINT) {
		if (g_uCOM1.txBuf_count > 0) {
			count = Chip_UART_Send(g_uCOM1.selected, &g_uCOM1.txBuf[g_uCOM1.txBuf_uartIndex], g_uCOM1.txBuf_count);
			g_uCOM1.txBuf_count -= count;
			g_uCOM1.txBuf_uartIndex += count;
		}
		/* If  usbRxBuf empty check if any packet pending USB EP RAM */
		if (g_uCOM1.txBuf_count < 1) {
			if ((g_uCOM1.usbRxPending > 0) && USB_IsConfigured(g_uCOM1.hUsb)) {
				g_uCOM1.usbRxPending--;
				g_uCOM1.txBuf_count = USBD_API->hw->ReadEP(g_uCOM1.hUsb, g_uCOM1.outEndpoint, g_uCOM1.txBuf);
				g_uCOM1.txBuf_uartIndex = 0;

			}
			else {
				/* all data transmitted on UART disable UART_IER_THREINT */
				Chip_UART_IntDisable(g_uCOM1.selected, UART_IER_THREINT);
			}
		}
	}

	/* Handle receive interrupt */
	count = Chip_UART_Read(g_uCOM1.selected, &g_uCOM1.rxBuf[g_uCOM1.rxBuf_uartIndex], UCOM_BUF_SZ - g_uCOM1.rxBuf_uartIndex);

	if (count) {
		/* Note, following logic works if UCOM_BUF_SZ is 2^n size only. */
		g_uCOM1.rxBuf_uartIndex = (g_uCOM1.rxBuf_uartIndex + count) & (UCOM_BUF_SZ - 1);
		/* If USB Tx is not busy kick start USB Tx */
		if ((g_uCOM1.usbTxBusy == 0) && USB_IsConfigured(g_uCOM1.hUsb)) {
			g_uCOM1.usbTxBusy = 1;
			count = USBD_API->hw->WriteEP(g_uCOM1.hUsb, g_uCOM1.inEndpoint, &g_uCOM1.rxBuf[g_uCOM1.rxBuf_usbIndex], count);
			g_uCOM1.rxBuf_usbIndex = (g_uCOM1.rxBuf_usbIndex + count) & (UCOM_BUF_SZ - 1);
		}
	}

}

ErrorCode_t  initCom(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T *pDesc,USBD_API_INIT_PARAM_T *pUsbParam, UCOM_DATA_T *com, int o)
{
	ErrorCode_t ret = LPC_OK;
	USBD_CDC_INIT_PARAM_T cdc_param2;

	com->hUsb = hUsb;
	/* Initi CDC params */
	memset((void *) &cdc_param2, 0, sizeof(USBD_CDC_INIT_PARAM_T));
	cdc_param2.mem_base = pUsbParam->mem_base;
	cdc_param2.mem_size = pUsbParam->mem_size;
	cdc_param2.cif_intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, CDC_COMMUNICATION_INTERFACE_CLASS,o);
	cdc_param2.dif_intf_desc = (uint8_t *) find_IntfDesc(pDesc->high_speed_desc, CDC_DATA_INTERFACE_CLASS,o);
	cdc_param2.SetLineCode = UCOM_SetLineCode;

	if(o==0)
	cdc_param2.SetLineCode = UCOM_SetLineCode1;

	ret = USBD_API->cdc->init(hUsb, &cdc_param2, &com->hCdc);
	if (ret == LPC_OK) {
		/* allocate transfer buffers */
		com->txBuf = (uint8_t *) cdc_param2.mem_base;
		cdc_param2.mem_base += UCOM_BUF_SZ;
		cdc_param2.mem_size -= UCOM_BUF_SZ;
		com->rxBuf = (uint8_t *) cdc_param2.mem_base;
		cdc_param2.mem_base += UCOM_BUF_SZ;
		cdc_param2.mem_size -= UCOM_BUF_SZ;

		/* register endpoint interrupt handler */
		uint32_t	ep_indx = (((	com->inEndpoint & 0x0F) << 1) +1);

		ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx, com->pInHdlr, com);

		if (ret == LPC_OK) {
			/* register endpoint interrupt handler */

			ep_indx = ((	com->outEndpoint & 0x0F) << 1);

			ret = USBD_API->core->RegisterEpHandler(hUsb, ep_indx,  com->pOutHdlr, com);
			/* Init UART port for bridging */
			//	UCOM_UartInit(1);
			/* Set the line coding values as per UART Settings */
			USB_CDC_CTRL_T * pCDC = (USB_CDC_CTRL_T *) com->hCdc;
			pCDC->line_coding.dwDTERate = 115200;
			pCDC->line_coding.bDataBits = 8;
		}

		/* update mem_base and size variables for cascading calls. */
		pUsbParam->mem_base = cdc_param2.mem_base;
		pUsbParam->mem_size = cdc_param2.mem_size;
	}
	return ret;
}
/* UART to USB com port init routine */
ErrorCode_t UCOM_init(USBD_HANDLE_T hUsb, USB_CORE_DESCS_T *pDesc, USBD_API_INIT_PARAM_T *pUsbParam)
{

	ErrorCode_t ret = LPC_OK;



	/* Store USB stack handle for future use. */

	g_uCOM1.selected=LPC_UART0;
	g_uCOM2.selected=LPC_UART2;
	g_uCOM3.selected=0;
	g_uCOM4.selected=0;
	g_uCOM5.selected=0;

	g_uCOM1.inEndpoint=USB_CDC1_IN_EP;
	g_uCOM1.outEndpoint=USB_CDC1_OUT_EP;
	g_uCOM1.pInHdlr = &UCOM_bulk_in_hdlr;
	g_uCOM1.pOutHdlr = &UCOM_bulk_hdlr;
	if (ret == LPC_OK) {
		ret= initCom(hUsb, pDesc, pUsbParam, &g_uCOM1, 0);
	}


	g_uCOM2.inEndpoint=USB_CDC2_IN_EP;
	g_uCOM2.outEndpoint=USB_CDC2_OUT_EP;
	g_uCOM2.pInHdlr = &UCOM_bulk_in_hdlr2;
	g_uCOM2.pOutHdlr = &UCOM_bulk_hdlr2;
	if (ret == LPC_OK) {
		ret=initCom(hUsb, pDesc, pUsbParam, &g_uCOM2, 1);
	}

	g_uCOM3.inEndpoint=USB_CDC3_IN_EP;
	g_uCOM3.outEndpoint=USB_CDC3_OUT_EP;
	g_uCOM3.pInHdlr = &UCOM_bulk_in_hdlr3;
	g_uCOM3.pOutHdlr = &UCOM_bulk_hdlr3;
	if (ret == LPC_OK) {
		ret= initCom(hUsb, pDesc, pUsbParam, &g_uCOM3, 2);
	}
	g_uCOM4.inEndpoint=USB_CDC4_IN_EP;
	g_uCOM4.outEndpoint=USB_CDC4_OUT_EP;
	g_uCOM4.pInHdlr = &UCOM_bulk_in_hdlr4;
	g_uCOM4.pOutHdlr = &UCOM_bulk_hdlr4;
	if (ret == LPC_OK) {
		ret=initCom(hUsb, pDesc, pUsbParam, &g_uCOM4, 3);
	}
	g_uCOM5.inEndpoint=USB_CDC5_IN_EP;
	g_uCOM5.outEndpoint=USB_CDC5_OUT_EP;
	g_uCOM5.pInHdlr = &UCOM_bulk_in_hdlr5;
	g_uCOM5.pOutHdlr = &UCOM_bulk_hdlr5;
	if (ret == LPC_OK) {
		ret=initCom(hUsb, pDesc, pUsbParam, &g_uCOM5, 4);
	}

	return ret;
}





/* Virtual com port buffered read routine */
uint32_t vcom_bread(uint8_t *pBuf, uint32_t buf_len)
{
	UCOM_DATA_T *pVcom = &g_uCOM1;
	uint16_t cnt = 0;
	/* read from the default buffer if any data present */
	if (pVcom->rx_count) {
		cnt = (pVcom->rx_count < buf_len) ? pVcom->rx_count : buf_len;
		memcpy(pBuf, pVcom->rxBuf, cnt);
		pVcom->rx_rd_count += cnt;

		/* enter critical section */
		NVIC_DisableIRQ(USB_IRQn);
		if (pVcom->rx_rd_count >= pVcom->rx_count) {
			pVcom->rx_flags &= ~VCOM_RX_BUF_FULL;
			pVcom->rx_rd_count = pVcom->rx_count = 0;
		}
		/* exit critical section */
		NVIC_EnableIRQ(USB_IRQn);
	}
	return cnt;

}

/* Virtual com port read routine */
ErrorCode_t vcom_read_req(uint8_t *pBuf, uint32_t len)
{
	UCOM_DATA_T *pVcom = &g_uCOM1;

	/* check if we queued Rx buffer */
	if (pVcom->rx_flags & (VCOM_RX_BUF_QUEUED | VCOM_RX_DB_QUEUED)) {
		return ERR_BUSY;
	}
	/* enter critical section */
	NVIC_DisableIRQ(USB_IRQn);
	/* if not queue the request and return 0 bytes */
	USBD_API->hw->ReadReqEP(pVcom->hUsb, pVcom->outEndpoint, pBuf, len);
	/* exit critical section */
	NVIC_EnableIRQ(USB_IRQn);
	pVcom->rx_flags |= VCOM_RX_DB_QUEUED;

	return LPC_OK;
}

/* Gets current read count. */
uint32_t vcom_read_cnt(void)
{
	UCOM_DATA_T *pVcom = &g_uCOM1;
	uint32_t ret = 0;

	if (pVcom->rx_flags & VCOM_RX_DONE) {
		ret = pVcom->rx_count;
		pVcom->rx_count = 0;
	}

	return ret;
}

/* Virtual com port write routine*/
uint32_t vcom_write(uint8_t *pBuf, uint32_t len)
{
	UCOM_DATA_T *pVcom = &g_uCOM1;
	uint32_t ret = 0;

	if ( (pVcom->tx_flags & VCOM_TX_CONNECTED) && ((pVcom->tx_flags & VCOM_TX_BUSY) == 0) ) {
		pVcom->tx_flags |= VCOM_TX_BUSY;

		/* enter critical section */
		NVIC_DisableIRQ(USB_IRQn);
		ret = USBD_API->hw->WriteEP(pVcom->hUsb, pVcom->inEndpoint, pBuf, len);
		/* exit critical section */
		NVIC_EnableIRQ(USB_IRQn);
	}

	return ret;
}
uint32_t vcom_connected(void) {
	return (g_uCOM1.tx_flags & VCOM_TX_CONNECTED);
}
