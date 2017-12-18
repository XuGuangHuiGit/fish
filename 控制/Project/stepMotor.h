#ifndef __STEPMOTOR_H
#define __STEPMOTOR_H

#include "stm32f10x.h"

#define RUN 				1
#define STOP				0

#define X_MOTOR 		0
#define Y_MOTOR 		1

#define MOTOR_SET 	0
#define MOTOR_DONE  1


extern int motor_1_CNT ;
extern int motor_2_CNT ;
extern u8 MotorSetAndDone_Flag;
extern u16 fishMotorRun_Flag;

int motorInit(int speed);
void setPos(int X_pos, int Y_pos);


#endif

