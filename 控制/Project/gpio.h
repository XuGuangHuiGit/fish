#include "stm32f10x_gpio.h"

void gpio_init(GPIO_TypeDef* GPIOx , uint16_t GPIO_Pin , GPIOSpeed_TypeDef GPIO_Speed , GPIOMode_TypeDef GPIO_Mode);
void GPIO_confi(void);
void saltMotor(u8 state);

void fishMotor(u8 state);

void mixUpMotor(u8 state);

void addWaterMotor(u8 state);

