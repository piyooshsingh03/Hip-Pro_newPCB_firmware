/*
 * GPIO.c
 *
 *  Created on: Oct 31, 2025
 *      Author: USER
 */
#include"main.h"
#include"GPIO.h"
#include"var.h"
bool P_Pressed=true;
bool Read_Status=true;
bool toggel_pin_stautus=false;
bool EN_MICRO=false;
bool USB_STAT=false;
volatile uint16_t IN_VALUE[5]={0};
void Powerswitch(void)   //device on / off
{
	P_Pressed = !HAL_GPIO_ReadPin(PB_IN_MICRO_GPIO_Port, PB_IN_MICRO_Pin);
	if(P_Pressed)
	{
		HAL_Delay(1000);
		HAL_GPIO_TogglePin(EN_MICRO_GPIO_Port, EN_MICRO_Pin);
		BUZZER_ON();
		toggel_pin_stautus=HAL_GPIO_ReadPin(EN_MICRO_GPIO_Port, EN_MICRO_Pin);
		HAL_Delay(1000);
		BUZZER_OFF();
		HAL_Delay(4000);
//		if(!EN_MICRO){
//			HAL_GPIO_WritePin(EN_MICRO_GPIO_Port, EN_MICRO_Pin, SET);
//			EN_MICRO=true;
//			BUZZER_ON();
//			HAL_Delay(2000);
//		}
//		if(EN_MICRO)
//		{
//			HAL_GPIO_WritePin(EN_MICRO_GPIO_Port, EN_MICRO_Pin, RESET);
//			BUZZER_ON();
//			EN_MICRO=false;
//		}
//		HAL_GPIO_TogglePin(EN_MICRO_GPIO_Port, EN_MICRO_Pin);

	}
	/*  temprory  witching testing
	 * HAL_GPIO_WritePin(EN_MICRO_GPIO_Port, EN_MICRO_Pin, GPIO_PIN_SET);
		  osDelay(2000);
		  HAL_GPIO_WritePin(EN_MICRO_GPIO_Port, EN_MICRO_Pin, GPIO_PIN_RESET);
		  osDelay(8000);

		  uint32_t start = HAL_GetTick();
		  			  		while (HAL_GetTick() - start < 300) // run for 1 second
		  			  		{
		  			  		    HAL_GPIO_TogglePin(BUZZER_GPIO_Port, BUZZER_Pin);
		  			  		    delay_us(125);   // 125 µs half-period → 4 kHz
		  			  		}
		  			  		HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
		  			  	osDelay(500);
	*/
}

void Battery_level(void)
{
	Read_Status=HAL_GPIO_ReadPin(BAT_MEAS_EN_GPIO_Port, BAT_MEAS_EN_Pin);
	if(Read_Status)
	{
		BAT_Voltage=(IN_VALUE[4] * (3.3f / 4095.0f));
		HAL_GPIO_WritePin(BAT_MEAS_EN_GPIO_Port, BAT_MEAS_EN_Pin, RESET);
		V_BAT_per=(BAT_Voltage/2.4f)*100.0f;
		vTaskDelay(100);
		send8bit(&huart1,V_BAT_per);
		Read_Status=false;
	}

}

void BAT_CHAR_STATUS(void)
{
	USB_STAT= HAL_GPIO_ReadPin(Power_Range_GPIO_Port, Power_Range_Pin);
}
void FAN1_ON(void){HAL_GPIO_WritePin(FAN_1_GPIO_Port, FAN_1_Pin, SET);}
void FAN1_OFF(void){HAL_GPIO_WritePin(FAN_1_GPIO_Port, FAN_1_Pin, RESET);}
void FAN2_ON(void){HAL_GPIO_WritePin(FAN_2_GPIO_Port, FAN_2_Pin, SET);}
void FAN2_OFF(void){HAL_GPIO_WritePin(FAN_2_GPIO_Port, FAN_2_Pin, RESET);}

// LED Control
void redled(void) { HAL_GPIO_WritePin(LOW_BAT_LED_GPIO_Port, LOW_BAT_LED_Pin, SET); }
void greenled(void) { HAL_GPIO_WritePin(FULL_BAT_LED_GPIO_Port, FULL_BAT_LED_Pin, SET); }
void redledoff(void) { HAL_GPIO_WritePin(LOW_BAT_LED_GPIO_Port, LOW_BAT_LED_Pin, RESET); }
void greenledoff(void) { HAL_GPIO_WritePin(FULL_BAT_LED_GPIO_Port, FULL_BAT_LED_Pin, RESET); }

void N20_1_CW(void)
{
	HAL_GPIO_WritePin(N20_1__GPIO_Port, N20_1__Pin, RESET);
	HAL_GPIO_WritePin(N20_1_B13_GPIO_Port, N20_1_B13_Pin, SET);
}
void N20_1_ACW(void)
{
	HAL_GPIO_WritePin(N20_1__GPIO_Port, N20_1__Pin, SET);
	HAL_GPIO_WritePin(N20_1_B13_GPIO_Port, N20_1_B13_Pin, RESET);
}
void N20_1_OFF(void)
{
	HAL_GPIO_WritePin(N20_1__GPIO_Port, N20_1__Pin, RESET);
	HAL_GPIO_WritePin(N20_1_B13_GPIO_Port, N20_1_B13_Pin, RESET);
}
void N20_2_CW(void)
{
	HAL_GPIO_WritePin(N20_2__GPIO_Port, N20_2__Pin, SET);
	HAL_GPIO_WritePin(N20_2_B15_GPIO_Port, N20_2_B15_Pin, RESET);
}
void N20_2_ACW(void)
{
	HAL_GPIO_WritePin(N20_2__GPIO_Port, N20_2__Pin, RESET);
	HAL_GPIO_WritePin(N20_2_B15_GPIO_Port, N20_2_B15_Pin, SET);
}
void N20_2_OFF(void)
{
	HAL_GPIO_WritePin(N20_2__GPIO_Port, N20_2__Pin, RESET);
	HAL_GPIO_WritePin(N20_2_B15_GPIO_Port, N20_2_B15_Pin, RESET);
}



void N20_1_WKUP(void){HAL_GPIO_WritePin(Sleep_N20_1_GPIO_Port, Sleep_N20_1_Pin, SET);}
void N20_1_SLEEP(void){HAL_GPIO_WritePin(Sleep_N20_1_GPIO_Port, Sleep_N20_1_Pin, RESET);}
void N20_2_WKUP(void){HAL_GPIO_WritePin(Sleep_N20_2_GPIO_Port, Sleep_N20_2_Pin, SET);}
void N20_2_SLEEP(void){HAL_GPIO_WritePin(Sleep_N20_2_GPIO_Port, Sleep_N20_2_Pin, RESET);}

void BUZZER_ON(void)
{
	uint32_t start = HAL_GetTick();
				  		while (HAL_GetTick() - start < 1000) // run for 1 second
				  		{
				  			    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_SET);
				  			    delay_us(150);   // half period (125 us)
				  			    HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
				  			    delay_us(150);
				  		}
}

void BUZZER_OFF(void)
{
	HAL_GPIO_WritePin(BUZZER_GPIO_Port, BUZZER_Pin, GPIO_PIN_RESET);
}
