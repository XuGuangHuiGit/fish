#include "stepMotor.h"
#include "stm32f10x_tim.h"
#include "gpio.h"
#include "USART.h"


#define FORWARD(x) GPIO_SetBits(GPIOC , x);
#define REVERSE(x) GPIO_ResetBits(GPIOC , x);

u8 MotorSetAndDone_Flag = MOTOR_DONE;
u16 fishMotorRun_Flag = 0;

int motor_X_CNT = 0;
int motor_Y_CNT = 0;
int period = 0;
void PWM1_init(int pre);


int motorInit(int speed){
	PWM1_init(speed);
	//PC8 is used as dir ctrl of X-motor
	gpio_init(GPIOC , GPIO_Pin_8 , GPIO_Speed_50MHz , GPIO_Mode_Out_PP);
	
	//PC9 is used as dir ctrl of W-motor
	gpio_init(GPIOC , GPIO_Pin_9 , GPIO_Speed_50MHz , GPIO_Mode_Out_PP);
	
	return 1;
}


void setSpeed(u8 which , u16 speed){
	
}

void ctrlMotor(u8 which , s16 state){
	if(which == X_MOTOR){
		TIM_SetCompare1(TIM1,state*period/2);
	}
	
	if(which == Y_MOTOR){
		TIM_SetCompare4(TIM1,state*period/2);
	}
}


void setPos(int X_pos, int Y_pos){
	Y_pos *=-1;
	if(X_pos<0){
		FORWARD(GPIO_Pin_8);
		X_pos *= -1;
	}else{
		REVERSE(GPIO_Pin_8);
	}
	
	if(Y_pos<0){
		FORWARD(GPIO_Pin_9);
		Y_pos *= -1;
	}else {
		REVERSE(GPIO_Pin_9);
	}
	
	motor_X_CNT = X_pos;
	motor_Y_CNT = Y_pos;
	
	if(motor_X_CNT)
		ctrlMotor(X_MOTOR , RUN);
	
	if(motor_Y_CNT)
		ctrlMotor(Y_MOTOR , RUN);
}





void PWM1_init(int pre){	
	
	NVIC_InitTypeDef nvic_InitStructure;
	GPIO_InitTypeDef gpio_InitStructure;
	TIM_TimeBaseInitTypeDef tim_TimeBaseInitTypeDef;
	TIM_OCInitTypeDef  TIM_OCInitStructure;
	
	period = 	4000000 / pre;
	gpio_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	gpio_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	gpio_InitStructure.GPIO_Pin = GPIO_Pin_8|GPIO_Pin_11;
	GPIO_Init(GPIOA, &gpio_InitStructure);

	tim_TimeBaseInitTypeDef.TIM_Prescaler = 35; //4M
	tim_TimeBaseInitTypeDef.TIM_CounterMode = TIM_CounterMode_Up;
	tim_TimeBaseInitTypeDef.TIM_Period =period;           
	tim_TimeBaseInitTypeDef.TIM_ClockDivision = 0;
	tim_TimeBaseInitTypeDef.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(TIM1, &tim_TimeBaseInitTypeDef);
	
	
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM2;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;
	TIM_OCInitStructure.TIM_Pulse =0;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low;    //当为PWM模式2时，极性要反过来
	TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Set;
	TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCIdleState_Reset;
	
	TIM_OC1Init(TIM1, &TIM_OCInitStructure);
	TIM_OC4Init(TIM1, &TIM_OCInitStructure);

	TIM_ITConfig(TIM1 , TIM_IT_Update , ENABLE);
	
	nvic_InitStructure.NVIC_IRQChannel = TIM1_UP_IRQn;   
	nvic_InitStructure.NVIC_IRQChannelCmd = ENABLE;   
	nvic_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;  
	nvic_InitStructure.NVIC_IRQChannelSubPriority = 1;   
	NVIC_Init(&nvic_InitStructure);  
	
	
	TIM_Cmd(TIM1, ENABLE);
	TIM_CtrlPWMOutputs(TIM1, ENABLE);

}


void TIM1_UP_IRQHandler(){
	if(TIM_GetITStatus(TIM1,TIM_IT_Update) != RESET ){
		TIM_ClearITPendingBit(TIM1,TIM_FLAG_Update);//清中断标志位
		if(motor_X_CNT >0)
			--motor_X_CNT;
		else{
			ctrlMotor(X_MOTOR , STOP);
		}
		
		if(motor_Y_CNT >0)
			--motor_Y_CNT;
		else{
			ctrlMotor(Y_MOTOR , STOP);
		}
		
		/*mark that step-motor has done*/
		if(motor_X_CNT == 0 && motor_Y_CNT == 0 && MotorSetAndDone_Flag == MOTOR_SET){
			MotorSetAndDone_Flag = MOTOR_DONE;
		}
		
		/*
		*	check fish-motor and stop it
		*/
		if(fishMotorRun_Flag > 5){
			/*wait until motor has gone some time*/
			fishMotorRun_Flag --;
		}else if(fishMotorRun_Flag <= 5&&fishMotorRun_Flag >1){
			if(GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_5)){
				fishMotor(STOP);
				fishMotorRun_Flag = 0;
			}
		}
	}
}
