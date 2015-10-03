/*
 * target_can.h
 *
 *  Created on: 15-Feb-2015
 *      Author: thorsten
 */

#ifndef INC_TARGET_CAN_H_
#define INC_TARGET_CAN_H_
#include "board.h"

typedef enum
{
	CAN_MSG_RECEIVED,
	CAN_STRING_OUT,
	CAN_GET_BAUDRATE
} eCallBackTypes;
typedef void* CAN_HANDLE_T;
typedef ErrorCode_t (*CAN_CALLBACK)(CAN_HANDLE_T hCan, void* data, eCallBackTypes event,  void* bassBye);
typedef struct {
  LPC_CAN_T *canbus;
  CAN_CALLBACK callback;
  void *callbackData;

} TARGET_CAN_T;

void setBaudRate(CAN_HANDLE_T canbus , int rate);

void sendCanMsg(CAN_HANDLE_T canbus, CAN_MSG_T *);
void setCallBack(CAN_HANDLE_T canbus, CAN_CALLBACK cb, void *data  );

extern void target_can_init();

TARGET_CAN_T gCAN1;
TARGET_CAN_T gCAN2;


#endif /* INC_TARGET_CAN_H_ */
