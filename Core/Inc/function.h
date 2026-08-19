/*
 * function.h
 *
 *  Created on: Nov 7, 2025
 *      Author: USER
 */

#ifndef INC_FUNCTION_H_
#define INC_FUNCTION_H_
#include"var.h"
#include"icm20948.h"
#include"BT.h";
#include "W25Qxx.h"
void valvevalue(void);
void leftsensor(void);
void rightsensor(void);
uint8_t checkPosture();
void Callibration_value(void);
void HealthStatus();
void ManualMode(void);
void Define_Mode(void);
void automaticMode(void);
void balancetest(void) ;
void CheckStatus();
void HealthStatus();
void handlePostureChange(void);
void SOS(void);
void BUttonSOS(void);
void updatepostureflags(void);
void standposture();
void sitposture();
void walkposture() ;
void reinflation();
void updatetime(void);
void Error(void);
void _30sec_test(void);
void _5timessittostand_test(void);
void TUG_Test(void);
void _4Stage_test(void);
void sendLivedata(void);
void cal_data(void);
void systemReset(void);
void readmemory(void);
void commandFunc(void);
#endif /* INC_FUNCTION_H_ */
