#include "ADC.h"
#include "stm32f10x_gpio.h"
#include "TIM.h"

#define SMAX 1.5
#define SMIN 0

GPIO_InitTypeDef GPIO_initstruct;
ADC_InitTypeDef ADC_initstruct;
void saltAndTempInit(){
	GPIO_initstruct.GPIO_Pin = GPIO_Pin_1;
	GPIO_initstruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_initstruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA , &GPIO_initstruct);
	
	ADC_initstruct.ADC_ContinuousConvMode = DISABLE;
	ADC_initstruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_initstruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_initstruct.ADC_Mode = ADC_Mode_Independent;
	ADC_initstruct.ADC_NbrOfChannel = 1;
	ADC_initstruct.ADC_ScanConvMode = DISABLE;
	
	ADC_Init(ADC1 , &ADC_initstruct);
	ADC_RegularChannelConfig(ADC1 , ADC_Channel_1 , 1 ,ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADC1 , ENABLE);
	
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1));
	
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1));
	
	
	GPIO_initstruct.GPIO_Pin = GPIO_Pin_2;
	GPIO_Init(GPIOA , &GPIO_initstruct);
	
	ADC_initstruct.ADC_ContinuousConvMode = DISABLE;
	ADC_initstruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_initstruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_initstruct.ADC_Mode = ADC_Mode_Independent;
	ADC_initstruct.ADC_NbrOfChannel = 1;
	ADC_initstruct.ADC_ScanConvMode = DISABLE;
	
	ADC_Init(ADC2 , &ADC_initstruct);
	ADC_RegularChannelConfig(ADC2 , ADC_Channel_2 , 1 ,ADC_SampleTime_55Cycles5);
	
	ADC_Cmd(ADC2 , ENABLE);
	
	ADC_ResetCalibration(ADC2);
	while(ADC_GetResetCalibrationStatus(ADC2));
	
	ADC_StartCalibration(ADC2);
	while(ADC_GetCalibrationStatus(ADC2));


}


float get_salt(){
	static float temp[5] = {0};
	int i = 0;
	float ave = 0;
	
	s16 result = 0;
	float result_f = 0;
	
	ADC_SoftwareStartConvCmd(ADC1 , ENABLE);
	while(!ADC_GetFlagStatus(ADC1 , ADC_FLAG_EOC));
	result = ADC_GetConversionValue(ADC1);
	
	result_f = ((result/1000.0)*5);
	
	if(result_f > SMAX)
		result_f = SMAX;
	
	
	for(i = 0; i<5 ; i++){
		ave += temp[i];
		if(i == 4){
			temp[i] = result_f;
			break;
		}
		temp[i] = temp[i+1];
	}
	
	result_f = ave / 5.0;
	
	return result_f;
}


float get_temp(){
	
	static float temp[5] = {0};
	int i = 0;
	float ave = 0;
	
	s16 result = 0;
	float result_f = 0;
	ADC_SoftwareStartConvCmd(ADC2 , ENABLE);
	while(!ADC_GetFlagStatus(ADC2 , ADC_FLAG_EOC));
	result = ADC_GetConversionValue(ADC2);
	
	result_f = ((result/1000.0)*16.444);
	
	for(i = 0; i<5 ; i++){
		ave += temp[i];
		if(i == 4){
			temp[i] = result_f;
			break;
		}
		temp[i] = temp[i+1];
	}
	
	result_f = ave / 5.0;
	
	return result_f;
}
