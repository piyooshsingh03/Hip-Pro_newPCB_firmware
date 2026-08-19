/*
 * GPIO.h
 *
 *  Created on: Oct 31, 2025
 *      Author: USER
 */

#ifndef INC_GPIO_H_
#define INC_GPIO_H_
#include <stdbool.h>
#include "main.h"
#include "stm32f1xx_hal.h"
extern bool P_Pressed;
extern bool Read_Status;
extern bool EN_MICRO;
extern bool USB_STAT;
extern volatile uint16_t IN_VALUE[5];
void Powerswitch(void);
void FAN1_ON(void);
void FAN1_OFF(void);
void FAN2_ON(void);
void FAN2_OFF(void);
void N20_1_CW(void);
void N20_1_ACW(void);
void N20_1_OFF(void);
void N20_2_CW(void);
void N20_2_ACW(void);
void N20_2_OFF(void);
void N20_1_WKUP(void);
void N20_1_SLEEP(void);
void N20_2_WKUP(void);
void N20_2_SLEEP(void);
void BUZZER_ON(void);
void BUZZER_OFF(void);
void Battery_level(void);
void BAT_CHAR_STATUS(void);
void redled(void) ;
void greenled(void);
void redledoff(void);
void greenledoff(void);
#endif /* INC_GPIO_H_ */
