/*
 * BT.h
 *
 *  Created on: Nov 10, 2025
 *      Author: USER
 */

#ifndef INC_BT_H_
#define INC_BT_H_


#include "stm32f1xx_hal.h"
extern uint8_t uart_tx_complete;

void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);
void sendString(UART_HandleTypeDef *huartx, int8_t *data, uint16_t size);
void send8bit(UART_HandleTypeDef *huartx,uint8_t Data);
void sendPosture(UART_HandleTypeDef *huartx,uint8_t Data);
void send16bit(UART_HandleTypeDef *huartx,int16_t Data);
void send32bit(UART_HandleTypeDef *huartx,uint32_t Data);
void send32bitRTC(UART_HandleTypeDef *huartx,uint32_t Data);
void btReceiveReset(UART_HandleTypeDef *huartx);
uint8_t btFuncCheck(UART_HandleTypeDef *huartx);
uint8_t btFuncInit(UART_HandleTypeDef *huartx);
void sendMergedData(UART_HandleTypeDef *huart, uint8_t data1, uint8_t data2);
void sendMerge6dData(UART_HandleTypeDef *huart, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6);
void sendMerged32byteData(UART_HandleTypeDef *huart, uint32_t data1, uint32_t data2, uint32_t data3);
void send32bitfloat_M1(UART_HandleTypeDef *huartx, float Data);
void send32bitfloat_M2(UART_HandleTypeDef *huartx, float Data);
void send12Floats(UART_HandleTypeDef *huartx, uint8_t data[30]);
void caldata(UART_HandleTypeDef *huartx, uint8_t data[12]);

#endif /* INC_BT_H_ */
