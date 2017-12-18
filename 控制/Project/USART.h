#ifndef __USART_H
#define __USART_H


#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_usart.h"
#include <stdio.h>
#include <stdarg.h>

#define CMD_USARTx USART1


void Usart_init(USART_TypeDef* USARTx , u32 baud);
void uprintf(USART_TypeDef* USARTx, char *fmt, ...);



#endif
