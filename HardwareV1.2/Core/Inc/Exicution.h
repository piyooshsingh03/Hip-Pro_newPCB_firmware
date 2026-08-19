#ifndef Exicution_H_
#define Exicution_H_

#include"Function.h"
extern UART_HandleTypeDef huart1;
//////////////////////////////inflation and deflation///////////////////////

//void valvevalue()    //SET OFFSET OF ADC VALUE
//{
//
//	HAL.hal1=0.9f * HAL.hal1 + 0.1f * (IN_value[1] * (3.3f / 4095.0f));
//	HAL.hal2=0.9f * HAL.hal2 + 0.1f * (IN_value[2] * (3.3f / 4095.0f));
//	HAL.hal3=0.9f * HAL.hal3 + 0.1f * (IN_value[3] * (3.3f / 4095.0f));
//	HAL.hal4=0.9f * HAL.hal4 + 0.1f * (IN_value[4] * (3.3f / 4095.0f));
//}

// hall sensor value
void valvevalue(void)    //SET OFFSET OF ADC VALUE
{
	HAL.hal1=0.85f * HAL.hal1 + 0.15f * (IN_VALUE[0] * (3.3f / 4095.0f));
	HAL.hal2=0.85f * HAL.hal2 + 0.15f * (IN_VALUE[1] * (3.3f / 4095.0f));
	HAL.hal3=0.85f * HAL.hal3 + 0.15f * (IN_VALUE[2] * (3.3f / 4095.0f));
	HAL.hal4=0.85f * HAL.hal4 + 0.15f * (IN_VALUE[3] * (3.3f / 4095.0f));
}
void Reset_valvevalue()    //SET OFFSET OF ADC VALUE
{
	HAL.hal1=0;
	HAL.hal2=0;
	HAL.hal3=0;
	HAL.hal4=0;
}



HAL_StatusTypeDef OpenValve(){
	uint8_t flag1=0,flag2=0,flag3=0;
	uint16_t timout=1500;
	Reset_valvevalue();
	time.tick = HAL_GetTick();
	 N20_1_WKUP();
	 N20_2_WKUP();
	N20_1_ACW();                                     // n-20 (+ to + and - to -)
	N20_2_ACW();                                     // n-20 (+ to + and - to -)
	while(HAL_GetTick()-time.tick<timout){
		valvevalue();
		if((HAL.hal3>1.4f+ 0.01f) &&(!flag1)){
			N20_1_OFF();
			N20_1_SLEEP();
			flag1=1;
		}
		if((HAL.hal2>1.4f+ 0.01f)&&(!flag2)){
			N20_2_OFF();
            N20_2_SLEEP();
			flag2=1;
		}
		if(flag1&&flag2){
			if((HAL.hal3 < 1.4f)){ N20_1_WKUP();N20_1_ACW();HAL_Delay(400);N20_1_SLEEP();}
			if((HAL.hal2 < 1.4f)){ N20_2_WKUP();N20_2_ACW();HAL_Delay(400);N20_2_SLEEP();}
			return(HAL_OK);
		}

	}
	if((HAL.hal3 < 1.4f)){ N20_1_WKUP();N20_1_ACW();HAL_Delay(400);N20_1_SLEEP();}
	if((HAL.hal2 < 1.4f)){ N20_2_WKUP();N20_2_ACW();HAL_Delay(400);N20_2_SLEEP();}
	N20_1_OFF();
	N20_2_OFF();
	N20_1_SLEEP();
	N20_2_SLEEP();
	return(HAL_TIMEOUT);
}
HAL_StatusTypeDef CloseValve(){
	uint8_t flag1=0,flag2=0,flag3=0;
	uint16_t timout=1500;    // to detect valve position
	Reset_valvevalue();
	time.tick = HAL_GetTick();
	N20_1_WKUP();
	N20_2_WKUP();
	N20_1_CW();
	N20_2_CW();
	while(HAL_GetTick()-time.tick<timout){
		valvevalue();
		if((HAL.hal1>1.35f+ 0.01f)&&(!flag2)){      // left pump  .01f
			N20_2_OFF();
			flag2=1;
			N20_2_SLEEP();
						}
		if((HAL.hal4>1.4f+ 0.01f)&&(!flag1)){      // Right pump
			N20_1_OFF();
			flag1=1;
			N20_1_SLEEP();
		}
		if(flag1&&flag2){
			if((HAL.hal4 < 1.4f)){ N20_1_WKUP();N20_1_CW();HAL_Delay(400);N20_1_SLEEP();}
			if((HAL.hal1 < 1.35f)){ N20_2_WKUP();N20_2_CW();HAL_Delay(400);N20_2_SLEEP();}
			return(HAL_OK);
		}
	}
	if((HAL.hal4 < 1.4f)){ N20_1_WKUP();N20_1_CW();HAL_Delay(400);N20_1_SLEEP();}
	if((HAL.hal1 < 1.35f)){ N20_2_WKUP();N20_2_CW();HAL_Delay(400);N20_2_SLEEP();}
    N20_1_OFF();
	N20_2_OFF();
	N20_1_SLEEP();
	N20_2_SLEEP();
	return(HAL_TIMEOUT);
}
HAL_StatusTypeDef INFLATION(){
	time.INF_time=1500;
	FAN1_ON();
	FAN2_ON();
	if(OpenValve()==HAL_TIMEOUT) {
		FAN1_OFF();
		FAN2_OFF();
		return(HAL_TIMEOUT);
	}
	time.INF_start=HAL_GetTick();
	while(HAL_GetTick()-time.INF_start<time.INF_time){};//pump running time
	if(CloseValve()==HAL_TIMEOUT) {
		FAN1_OFF();
		FAN2_OFF();
		return(HAL_TIMEOUT);
	}
	FAN1_OFF();
    FAN2_OFF();
	return(HAL_OK);
}
HAL_StatusTypeDef DEFLATION(){
	return(OpenValve());
}

void fall(void)
{
	if (testFlags.fall1==1 ||testFlags.fall2==1)
		{
		SOS();   //active pre fall  // && Stand_count>=1 optional
		}


}


#endif // EXICUTION_H_
