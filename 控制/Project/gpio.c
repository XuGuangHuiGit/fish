#include "gpio.h"



void gpio_init(GPIO_TypeDef* GPIOx , uint16_t GPIO_Pin , GPIOSpeed_TypeDef GPIO_Speed , GPIOMode_TypeDef GPIO_Mode){
	GPIO_InitTypeDef GPIO_initstruct;
	GPIO_initstruct.GPIO_Pin = GPIO_Pin;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed;
	GPIO_initstruct.GPIO_Mode = GPIO_Mode;
	
	GPIO_Init(GPIOx , &GPIO_initstruct);
}


void GPIO_confi(void){
	//set PA3 as X-touch-home-check input
	gpio_init(GPIOA , GPIO_Pin_3 , GPIO_Speed_50MHz , GPIO_Mode_IPD);
	
	//set PA4 as Y-touch-home-check input
	gpio_init(GPIOA , GPIO_Pin_4 , GPIO_Speed_50MHz , GPIO_Mode_IPD);
	
	//set PA5 as fish-check input
	gpio_init(GPIOA , GPIO_Pin_5 , GPIO_Speed_50MHz , GPIO_Mode_IPD);
	
	//set PA6 as salt-check input
	gpio_init(GPIOA , GPIO_Pin_6 , GPIO_Speed_50MHz , GPIO_Mode_IPD);
	
	//set PB3 as fish-motor ctrl
	gpio_init(GPIOC , GPIO_Pin_10 , GPIO_Speed_50MHz , GPIO_Mode_Out_PP);
	
	//set PB4 as salt-motor ctrl
	gpio_init(GPIOC , GPIO_Pin_11 , GPIO_Speed_50MHz , GPIO_Mode_Out_PP);
	
	//set PB5 as mixup-motor ctrl
	gpio_init(GPIOB , GPIO_Pin_5 , GPIO_Speed_50MHz , GPIO_Mode_Out_PP);
	
	//set PB6 as Add-water-motor ctrl
	gpio_init(GPIOB , GPIO_Pin_6 , GPIO_Speed_50MHz , GPIO_Mode_Out_PP);
	
}


void saltMotor(u8 state){
	state?GPIO_SetBits(GPIOC , GPIO_Pin_11):GPIO_ResetBits(GPIOC , GPIO_Pin_11);
}

void fishMotor(u8 state){
	state?GPIO_SetBits(GPIOC , GPIO_Pin_10):GPIO_ResetBits(GPIOC , GPIO_Pin_10);
}

void mixUpMotor(u8 state){
	state?GPIO_SetBits(GPIOB , GPIO_Pin_5):GPIO_ResetBits(GPIOB , GPIO_Pin_5);
}

void addWaterMotor(u8 state){
	state?GPIO_SetBits(GPIOB , GPIO_Pin_6):GPIO_ResetBits(GPIOB , GPIO_Pin_6);
}

