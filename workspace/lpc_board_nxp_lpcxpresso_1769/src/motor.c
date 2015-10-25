/*
 * motor.c
 *
 *  Created on: 25-Oct-2015
 *      Author: thorsten
 */
#include "motor.h"
#include "board.h"
typedef struct {
   int port;
   int bit;

} tPin;
typedef struct
{
	tPin pwm;
	tPin a;
	tPin b;
} tMotorDef;

#define MOTOR_STBY_PORT_NUM 1
#define MOTOR_STBY_BIT_NUM 4

#define NUM_MOTOR 4
const tMotorDef MOTOR[NUM_MOTOR]=
{
		{
		{2,4},
		{1,9},
		{1,8},
		},

		{
				{2,5},
				{1,1},
				{1,0},
		},

		{
				{1,21},
				{0,9},
				{1,10},
		},

		{
				{1,23},
				{1,14},
				{0,8},
		},
};

void Motor_Init(void)
{
	Chip_GPIO_WriteDirBit(LPC_GPIO, MOTOR_STBY_PORT_NUM, MOTOR_STBY_BIT_NUM, true);

	int i;
	for(int i=0;i<NUM_MOTOR;i++)
	{
		Chip_GPIO_WriteDirBit(LPC_GPIO, MOTOR[i].pwm.port, MOTOR[i].pwm.bit, true);
		Chip_GPIO_WriteDirBit(LPC_GPIO, MOTOR[i].a.port, MOTOR[i].a.bit, true);
		Chip_GPIO_WriteDirBit(LPC_GPIO, MOTOR[i].b.port, MOTOR[i].b.bit, true);
	}

	Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR_STBY_PORT_NUM, MOTOR_STBY_BIT_NUM, true);
}


void Moror_Do(int i, eDirection dir)
{
	if(i<NUM_MOTOR)
	{
		if(dir==MOTOR_OFF)
		{
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].pwm.port, MOTOR[i].pwm.bit, false);
		}else{
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].pwm.port, MOTOR[i].pwm.bit, true);
		}
		switch(dir)
		{
		case MOTOR_STOP:
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].a.port, MOTOR[i].a.bit, false);
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].b.port, MOTOR[i].b.bit, false);
		break;
		case MOTOR_LEFT:
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].a.port, MOTOR[i].a.bit, false);
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].b.port, MOTOR[i].b.bit, true);
		break;
		case MOTOR_RIGHT:
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].a.port, MOTOR[i].a.bit, true);
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].b.port, MOTOR[i].b.bit, false);
		break;
		case MOTOR_OFF:
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].a.port, MOTOR[i].a.bit, true);
			Chip_GPIO_WritePortBit(LPC_GPIO, MOTOR[i].b.port, MOTOR[i].b.bit, true);
			break;
		}
	}
}
void Motor_All(eDirection dir)
{
	int i;
	for(int i=0;i<NUM_MOTOR;i++)
	{
		Moror_Do(i,dir);
	}

}
