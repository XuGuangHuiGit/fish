#include "TIM.h"
#include "USART.h"


void delay_ms(u16 nms)  
{  
	u32 temp;
	SysTick ->LOAD = 9000*nms;
	SysTick ->VAL = 0x00;
	SysTick ->CTRL = 0x01;
	do{
		temp = SysTick ->CTRL;
	}while((temp&0x01)&&(!(temp&(1<<16))));
	SysTick ->CTRL = 0x00;
	SysTick ->VAL = 0x00;
   
} 






