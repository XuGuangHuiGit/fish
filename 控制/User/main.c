/**
  ******************************************************************************
  * 
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "USART.h"
#include "rcc.h"
#include "TIM.h"
#include "main.h"
#include "stdlib.h"
#include "cmd.h"
#include "gpio.h"
#include "outputdata.h"
#include "ADC.h"
#include "stepMotor.h"

#define MOTOR_SPEED 1000

#define X_MAX 		2000
#define Y_MAX 		1700

#define AREA_NUMBER 4

int area[AREA_NUMBER][2] = {
	{500,400},
	{1500,400},
	{1500,1300},
	{500,1300},
};

typedef struct POS{
	int X;
	int Y;
}Pos_NowStruct;


Pos_NowStruct Pos_Now;// save the absolute position
Pos_NowStruct Pos_Next;//save the relative position which will be transmited to Motor-ctrl

u8 state = 0 ;//the state of now
u8 checkDone_Flag = 0;
u8 Run_Flag = 0;

float SALT_MAX , SALT_MIN;
float TEMP_MAX , TEMP_MIN;

u8 CheckWater(void);
void GoToNextPos(void);

int main(){
	int X_home_cnt = 0;
	int Y_home_cnt = 0;
	rcc_init();
	Usart_init(USART1 , 115200);//init USART1
	uprintf(USART1 , "system initing...\n");
	
	cmd_init();
	saltAndTempInit();
	
	TEMP_MIN = *(float*)(0x08015000);
	TEMP_MAX = *(float*)(0x08015004);
	SALT_MIN = *(float*)(0x08015008);
	SALT_MAX = *(float*)(0x0801500C);
	
	uprintf(USART1 , "param %f %f %f %f\n" , SALT_MAX , SALT_MIN , TEMP_MAX , TEMP_MIN);
	
	motorInit(MOTOR_SPEED);
	GPIO_confi();
	fishMotor(STOP);
	saltMotor(STOP);
	mixUpMotor(STOP);
	addWaterMotor(STOP);
	
//	while(1)
//		uprintf(USART1 , "%f\n" , get_salt());
	/*go home*/
Home:
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3))
		X_home_cnt = -20000;
	if(!GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4))
		Y_home_cnt = -20000;
	Run_Flag = 0;
	setPos(X_home_cnt, Y_home_cnt);
	state = 0;
	while(state != 0x03){
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){ /*get X state*/
			delay_ms(1);
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)){
				if(!(state&0x01)){/*first treat X*/
					if((state>>1)&0x01){/*all done*/
						setPos(0 , 0);
					}else {
						setPos(0 , -10000);/*Y is not done*/
					}
					state |= 0x01;/* stop x_motor and mark it*/
				}
			}
		}
		
		if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)){ /*get Y state*/
			delay_ms(1);
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_4)){	
				if(!((state>>1)&0x01)){//first treat Y
					if(state&0x01){/*All done*/
						setPos(0 , 0);
					}else {
						setPos(-10000 , 0);/*X is not done*/
					}
					state |= 0x02;/* stop Y_motor and mark it */
				}
			}
		}	
	}
	//setPos(0 , 0);
	Pos_Now.X = 0;
	Pos_Now.Y = 0;//All in home
	uprintf(USART1 , "system init done!\nplease push ÔËÐÐ to start system\n");
	 
	while(1){
		if(Run_Flag){
			state = 0;
			setPos(area[state][0], area[state][1]);
			MotorSetAndDone_Flag = MOTOR_SET;
			break;
		}
	}
	while(1){
		if(Run_Flag == 1){
			delay_ms(20);
			
			//uprintf(USART1 , "pro %d %f %f\n" , state , get_temp() , salt);
			
			/*work when arrive*/
			if(MotorSetAndDone_Flag == MOTOR_DONE){
				CheckWater();
				/** Ok now we can go to next pos **/
				delay_ms(20000);
				GoToNextPos();
			}
			
			/**back information about what are you doing**/
			if(doingWhat == 1){
				uprintf(USART1, "go to the next pos\n");
				doingWhat = 0;
			}
		}		
		if(Run_Flag == 2) goto Home;
	}
}


/*
*	param: none
* function:get the relative position with Next-Pos and absolute position
*/
void GoToNextPos(){
	
	Pos_Now.X = area[state][0];
	Pos_Now.Y = area[state][1];
	
	state++;
	if(state >= AREA_NUMBER)
		state = 0;/**if the state is last one ,go to first**/
	
	Pos_Next.X = area[state][0] - Pos_Now.X;
	Pos_Next.Y = area[state][1] - Pos_Now.Y;
	
	setPos(Pos_Next.X, Pos_Next.Y);
	
	/**the motor was set.when it was done,go to next**/
	MotorSetAndDone_Flag = MOTOR_SET;
	
}

/*
*	check the salt and temperature
*/
u8 CheckWater(){
	
	float temp = get_temp();
	float salt = get_salt();
	u16 saltMotorCtrlStop_flag = 30;
	
	uprintf(USART1 , "pro %d %f %f\n" , state , temp , salt);
	
	if(temp>TEMP_MAX){
		/*
		* add water
		*/
		addWaterMotor(RUN);
		while(temp>TEMP_MAX){
			/*wait until the area is normal*/
			temp = get_temp();
			uprintf(USART1 , "pro %d %f %f\n" , state , temp , salt);
			delay_ms(10);
			if(doingWhat == 1){
				uprintf(USART1, "The temperature now is %f.I can not leave until it is normal\n" , temp);
				doingWhat = 0;
			}
		}
		addWaterMotor(STOP);
	}else if(temp<TEMP_MIN){
		/*power mix-up-motor to low temp*/
		mixUpMotor(RUN);
		while(temp<TEMP_MIN){
			//wait until the area is normal
			temp = get_temp();
			uprintf(USART1 , "pro %d %f %f\n" , state , temp , salt);
			delay_ms(10);
			if(doingWhat == 1){
				uprintf(USART1, "The temperature now is %f.I can not leave until it is normal\n" , temp);
				doingWhat = 0;
			}
		}
		mixUpMotor(STOP);
	}
	
	if(salt>SALT_MAX){
		/*power add-water-motor and mix-up-motor to low salt*/
		mixUpMotor(RUN);
		addWaterMotor(RUN);
		while(!(salt<SALT_MAX)){
			//so wait.....
			salt = get_salt();
			uprintf(USART1 , "pro %d %f %f\n" , state , temp , salt);
			delay_ms(10);
			if(doingWhat == 1){
				uprintf(USART1, "The salt now is %f.I can not leave until it is normal\n" , salt);
				doingWhat = 0;
			}
		}
		mixUpMotor(STOP);
		addWaterMotor(STOP);
	}else if(salt<SALT_MIN){
		uprintf(USART1, "lower\n");
		/*power salt-motor and mix-up-motor to low salt*/
		mixUpMotor(RUN);
		saltMotor(RUN);
		/*wait and wait...*/
		while(!(salt>SALT_MIN)){
			salt = get_salt();
			uprintf(USART1 , "pro %d %f %f\n" , state , temp , salt);
			delay_ms(10);
//			if((GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_6) == 1) && saltMotorCtrlStop_flag == 30){
//				saltMotorCtrlStop_flag = 0;
//			}
//			if((GPIO_ReadInputDataBit(GPIOA , GPIO_Pin_6) == 0)&&saltMotorCtrlStop_flag == 0){
//				break;
//			}
////			}else{
////				saltMotorCtrlStop_flag --;
////			}
			if(doingWhat == 1){
				uprintf(USART1, "The salt now is %f.adding salt\n" , temp);
				doingWhat = 0;
			}
		}
		mixUpMotor(STOP);
		saltMotor(STOP);
	}
	
	uprintf(USART1 , "pro %d %f %f\n" , state , temp , salt);
	/*that mean the water is OK!*/
	return 1;
}
