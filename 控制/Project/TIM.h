#ifndef __TIM__H
#define __TIM__H

#include "stm32f10x_tim.h"

void TIM2_init(int Time_ms);
void delay_ms(u16 nms);

#endif
