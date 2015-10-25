/*
 * motor.h
 *
 *  Created on: 25-Oct-2015
 *      Author: thorsten
 */

#ifndef MOTOR_H_
#define MOTOR_H_

void Motor_Init(void);

typedef enum {
	MOTOR_LEFT,
	MOTOR_RIGHT,
	MOTOR_STOP,
	MOTOR_OFF,
} eDirection;
void Motor_All(eDirection dir);
void Motor_Do(int motor, eDirection dir);

#endif /* MOTOR_H_ */
