/*
 * icm20948.c
 *
 *  Created on: Aug 3, 2023
 *      Author: harsh
 */


#include <math.h>
#include "icm20948.h"
#include "var.h"
//#include "var.h"
ICM20948_t ICMDataL={0}; // Acelrometer data  Left Leg
ICM20948_t ICMDataR={0}; // Acelrometer data Right Leg
extern UART_HandleTypeDef huart1;
const uint16_t i2c_timeout = 100;
const double Accel_Z_corrector = 14418.00;
extern int8_t countRST;
extern int8_t RSTflag;
uint32_t timer;

HAL_StatusTypeDef ICM20948_WriteRegister(uint8_t regAddr, uint8_t data,
		I2C_HandleTypeDef *I2Cx,uint8_t address) {
	return HAL_I2C_Mem_Write(I2Cx, (address << 1), regAddr,
			I2C_MEMADD_SIZE_8BIT, &data, 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef SelectBank(uint8_t bank, I2C_HandleTypeDef *I2Cx,uint8_t address) {
	return ICM20948_WriteRegister(REG_BANK_SEL, (bank << 4), I2Cx,address);
}

uint8_t ICM20948_Init(I2C_HandleTypeDef *I2Cx,uint8_t address) {
	uint8_t check;
	HAL_Delay(100);

	if (HAL_I2C_Mem_Read(I2Cx, (address << 1), WHO_AM_I_REG, 1, &check,
			1, HAL_MAX_DELAY) != HAL_OK)
		;

	SelectBank(0, I2Cx,address);

	if (check == 234 ) {

		if (ICM20948_WriteRegister(PWR_MGMT_1, 0xE1, I2Cx,address) != HAL_OK) {
			 sendString(&huart1,(int8_t*)"f", 1);
			 testFlags.RSTflag=1;

		}
		HAL_Delay(10);
		if (ICM20948_WriteRegister(PWR_MGMT_1, 0x01, I2Cx,address) != HAL_OK) {
			sendString(&huart1,(int8_t*)"f", 1);
			testFlags.RSTflag=1;
		}
		HAL_Delay(10);

		if (ICM20948_WriteRegister(PWR_MGMT_2, 0x00, I2Cx,address) != HAL_OK) {
			sendString(&huart1,(int8_t*)"f", 1);
			testFlags.RSTflag=1;
		}
		HAL_Delay(10);
		if (ICM20948_WriteRegister(USER_CTRL, 0x80, I2Cx,address) != HAL_OK) {
			sendString(&huart1,(int8_t*)"f", 1);
			testFlags.RSTflag=1;
		}
		HAL_Delay(10);
				if (ICM20948_WriteRegister(LP_CONFIG, 0x00, I2Cx,address) != HAL_OK) {   //// enable accelerometer duty cycling
								//			sendString(&huart1,(int8_t*)"f", 1);
								//			testFlags.RSTflag=1;
				}
		HAL_Delay(10);
		SelectBank(2, I2Cx,address);
		HAL_Delay(10);
		if (ICM20948_WriteRegister(ACCEL_INTEL_CTRL, 0x03, I2Cx,address) != HAL_OK) {   //Enable Wake-on-Motion Logic
					sendString(&huart1,(int8_t*)"f", 1);
					testFlags.RSTflag=1;
		}
		HAL_Delay(10);
		if (ICM20948_WriteRegister(ACCEL_WOM_THR, 0x10, I2Cx,address) != HAL_OK) {   //Set thresold 25 (ACCEL_WOM_THR = 25;  // threshold ≈ 25 × 4 mg = 100 mg)
							sendString(&huart1,(int8_t*)"f", 1);
							testFlags.RSTflag=1;
		}
		HAL_Delay(10);
		SelectBank(0, I2Cx,address);
		HAL_Delay(10);
		if (ICM20948_WriteRegister(INT_ENABLE, 0x08, I2Cx,address) != HAL_OK) {   //// enable INterrupt  Set bit WOM_INT_EN = 1
											sendString(&huart1,(int8_t*)"f", 1);
											testFlags.RSTflag=1;
		}
		HAL_Delay(10);
		if (ICM20948_WriteRegister(INT_PIN_CFG, 0x00, I2Cx,address) != HAL_OK) {
		}
		HAL_Delay(10);
		SelectBank(2, I2Cx,address);
		HAL_Delay(10);

		if (ICM20948_WriteRegister(ACCEL_CONFIG, 0x1D, I2Cx,address) != HAL_OK) {
			sendString(&huart1,(int8_t*)"f", 1);
			testFlags.RSTflag=1;
		}
		HAL_Delay(10);
		SelectBank(2, I2Cx,address);
		HAL_Delay(10);
		if (ICM20948_WriteRegister(GYRO_CONFIG, 0x01, I2Cx,address) != HAL_OK) {
			sendString(&huart1,(int8_t*)"f", 1);
			testFlags.RSTflag=1;
		}
		HAL_Delay(10);
		if (testFlags.RSTflag==1){
			countRST+=1;
		    rstcountWrite();
		       if (countRST<=2)
		          {Error();}
		       else if (countRST>2){
		             sendString(&huart1,(int8_t*)"f", 1);
		             testFlags.Imu=1;
		             testFlags.RSTflag=0;rstcountWrite();countRST=0;sendString(&huart1,(int8_t*)"f", 1);}
		               }
		else
		    sendString(&huart1,(int8_t*)"F", 1);
		testFlags.Imu=0;
		return 0;
	}
	else
	{
		testFlags.Imu=1;
		    sendString(&huart1,(int8_t*)"f", 1);
		    Error();
	       return 1;
	 }
}

void ICM20948_Read_Accel(I2C_HandleTypeDef *I2Cx, ICM20948_t *DataStruct,uint8_t address) {
    SelectBank(0, I2Cx,address);
	uint8_t Rec_Data[12];
	HAL_I2C_Mem_Read(I2Cx, (address << 1), ACCEL_XOUT_H, 1, Rec_Data, 12,
			i2c_timeout);

	DataStruct->ACCEL_X_RAW = (int16_t) (Rec_Data[0] << 8 | Rec_Data[1]);
	DataStruct->ACCEL_Y_RAW = (int16_t) (Rec_Data[2] << 8 | Rec_Data[3]);
	DataStruct->ACCEL_Z_RAW = (int16_t) (Rec_Data[4] << 8 | Rec_Data[5]);



	DataStruct->Ax = DataStruct->ACCEL_X_RAW / 4096.0;
	DataStruct->Ay = DataStruct->ACCEL_Y_RAW / 4096.0;
	DataStruct->Az = DataStruct->ACCEL_Z_RAW / 4096.0;

}

void ICM20948_Read_Gyro(I2C_HandleTypeDef *I2Cx, ICM20948_t *DataStruct,uint8_t address) {
	uint8_t Rec_Data[12];
	SelectBank(0,I2Cx,address);

	HAL_I2C_Mem_Read(I2Cx, (address << 1), ACCEL_XOUT_H, 1, Rec_Data, 12, i2c_timeout);

	DataStruct->Gyro_X_RAW = (int16_t) (Rec_Data[6] << 8 | Rec_Data[7]);
	DataStruct->Gyro_Y_RAW = (int16_t) (Rec_Data[8] << 8 | Rec_Data[9]);
	DataStruct->Gyro_Z_RAW = (int16_t) (Rec_Data[10] << 8 | Rec_Data[11]);

	DataStruct->Gx = DataStruct->Gyro_X_RAW / 65.5 ;
	DataStruct->Gy = DataStruct->Gyro_Y_RAW / 65.5 ;
	DataStruct->Gz = DataStruct->Gyro_Z_RAW / 65.5 ;

//    DataStruct->NormGx = DataStruct->Gx * .060975f;
//    DataStruct->NormGy = DataStruct->Gy * .060975f;
//    DataStruct->NormGz = DataStruct->Gz * .060975f;
}
void ICM20948_Read_Temp(I2C_HandleTypeDef *I2Cx, ICM20948_t *DataStruct,uint8_t address) {
	uint8_t Rec_Data[14];
	SelectBank(0,I2Cx,address);
	HAL_I2C_Mem_Read(I2Cx, (address << 1), ACCEL_XOUT_H, 1, Rec_Data, 14, i2c_timeout);
	DataStruct->Temperature = (int16_t) (Rec_Data[12] << 8 | Rec_Data[13]);
}
// Check for WOM interrupt (called in ISR)
bool ICM20948_Check_WOM(void)
{
    uint8_t status;
    SelectBank(0,&hi2c2,ICM20948_ADDR_LOW);
    HAL_I2C_Mem_Read(&hi2c2, (ICM20948_ADDR_LOW << 1), INT_STATUS, 1, &status, 1, i2c_timeout);  //
  //  HAL_I2C_Mem_Read(&hi2c1, ICM20948_ADDR, REG_INT_STATUS_1, 1, &status, 1, HAL_MAX_DELAY);

    if (status & (1 << 3))
        return true;
    else
        return false;
}
