/*
 * target_can.c
 *
 *  Created on: 15-Feb-2015
 *      Author: thorsten
 */

#include "target_can.h"

static void PrintCANErrorInfo(TARGET_CAN_T *can, uint32_t Status)
{
	if (Status & CAN_ICR_EI) {
		can->callback(can, "Error Warning!", CAN_STRING_OUT , can->callbackData);
	}
	if (Status & CAN_ICR_DOI) {
		can->callback(can, "Data overrun!", CAN_STRING_OUT , can->callbackData);
	}
	if (Status & CAN_ICR_EPI) {
		can->callback(can, "Error Passive!", CAN_STRING_OUT , can->callbackData);
	}
	if (Status & CAN_ICR_ALI) {
		// DEBUGOUT("Arbitration lost in the bit: %d(th)\r\n", CAN_ICR_ALCBIT_VAL(Status));
		can->callback(can, "Arbitration lost in the bit:", CAN_STRING_OUT , can->callbackData);
	}
	if (Status & CAN_ICR_BEI) {

		if (Status & CAN_ICR_ERRDIR_RECEIVE) {
			can->callback(can, "Buss error! Error Direction: Transmiting", CAN_STRING_OUT , can->callbackData);

		}
		else {
			can->callback(can, "Buss error! Error Direction: Receiving", CAN_STRING_OUT , can->callbackData);
			DEBUGOUT("\t Error Direction: Receiving\r\n");
		}

		//DEBUGOUT("\t Error Location: 0x%2x\r\n", CAN_ICR_ERRBIT_VAL(Status));
		//DEBUGOUT("\t Error Type: 0x%1x\r\n", CAN_ICR_ERRC_VAL(Status));
	}
}


void CAN_IRQHandler(void)
{

	uint32_t IntStatus;
	CAN_MSG_T RcvMsgBuf;
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN1);
	PrintCANErrorInfo(&gCAN1, IntStatus);
	if (IntStatus & CAN_ICR_RI) {
		Chip_CAN_Receive(LPC_CAN1, &RcvMsgBuf);
		gCAN1.callback(&gCAN1, &RcvMsgBuf, CAN_MSG_RECEIVED , gCAN1.callbackData);
		PrintCANErrorInfo(&gCAN1, IntStatus);
	}
	IntStatus = Chip_CAN_GetIntStatus(LPC_CAN2);
	PrintCANErrorInfo(&gCAN2, IntStatus);
	if (IntStatus & CAN_ICR_RI) {

		Chip_CAN_Receive(LPC_CAN2, &RcvMsgBuf);
		gCAN2.callback(&gCAN2, &RcvMsgBuf, CAN_MSG_RECEIVED , gCAN2.callbackData);
	}


}

void sendCanMsg(CAN_HANDLE_T h, CAN_MSG_T *SendMsgBuf)
{
	CAN_BUFFER_ID_T   TxBuf;
	TARGET_CAN_T *canbus=(TARGET_CAN_T *)h;
	TxBuf = Chip_CAN_GetFreeTxBuf(canbus->canbus);
	Chip_CAN_Send(canbus->canbus, TxBuf, SendMsgBuf);
	while ((Chip_CAN_GetStatus(canbus->canbus) & CAN_SR_TCS(TxBuf)) == 0) {} // this should be an interrupt?!

}

void target_can_initInternal(TARGET_CAN_T can, LPC_CAN_T *canbus)
{

	Chip_CAN_Init(canbus, LPC_CANAF, LPC_CANAF_RAM);
	Chip_CAN_SetBitRate(canbus, 500000);
	Chip_CAN_EnableInt(canbus, CAN_IER_BITMASK);
	can.canbus=LPC_CAN1;
}
void setBaudRate(CAN_HANDLE_T can , int rate)
{
	TARGET_CAN_T *canbus=(TARGET_CAN_T *)can;
	Chip_CAN_SetBitRate(canbus->canbus, rate);
}


void target_can_init()
{
	target_can_initInternal(gCAN1, LPC_CAN1);
	target_can_initInternal(gCAN2, LPC_CAN2);

	NVIC_EnableIRQ(CAN_IRQn);

}


