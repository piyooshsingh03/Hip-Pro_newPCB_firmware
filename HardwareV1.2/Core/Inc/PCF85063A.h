/*
 * PCF85063A.h
 *
 *  Created on: 14-Aug-2026
 *      Author: USER
 */

#ifndef INC_PCF85063A_H_
#define INC_PCF85063A_H_


#include "stm32f1xx_hal.h"
#include <stdbool.h>

static uint8_t DecimalToBCD(uint8_t decimal)
{
    return ((decimal / 10) << 4) | (decimal % 10);
}

static uint8_t BCDToDecimal(uint8_t bcd)
{
    return (((bcd >> 4) * 10) + (bcd & 0x0F));
}

static bool IsLeapYear(uint16_t year);

static uint8_t DaysInMonth(uint8_t month, uint16_t year);
/* ===========================
 * PCF85063A I2C Address
 * =========================== */
#define PCF85063A_I2C_ADDR      (0x51 << 1)

/* Register Map */
#define PCF85063A_REG_CONTROL_1     0x00
#define PCF85063A_REG_CONTROL_2     0x01
#define PCF85063A_REG_OFFSET        0x02
#define PCF85063A_REG_RAM           0x03

#define PCF85063A_REG_SECONDS       0x04
#define PCF85063A_REG_MINUTES       0x05
#define PCF85063A_REG_HOURS         0x06
#define PCF85063A_REG_DAYS          0x07
#define PCF85063A_REG_WEEKDAYS      0x08
#define PCF85063A_REG_MONTHS        0x09
#define PCF85063A_REG_YEARS         0x0A

/* Alarm Registers */
#define PCF85063A_REG_SECOND_ALARM  0x0B
#define PCF85063A_REG_MINUTE_ALARM  0x0C
#define PCF85063A_REG_HOUR_ALARM    0x0D
#define PCF85063A_REG_DAY_ALARM     0x0E
#define PCF85063A_REG_WEEKDAY_ALARM 0x0F

/* Timer Registers */
#define PCF85063A_REG_TIMER_VALUE   0x10
#define PCF85063A_REG_TIMER_MODE    0x11

#define PCF85063A_IST_OFFSET_SECONDS    (19800UL)   // UTC +5:30

#define SECONDS_PER_MINUTE    60U
#define MINUTES_PER_HOUR      60U
#define HOURS_PER_DAY         24U

#define SECONDS_PER_HOUR \
    (SECONDS_PER_MINUTE * MINUTES_PER_HOUR)

#define SECONDS_PER_DAY \
    (SECONDS_PER_HOUR * HOURS_PER_DAY)

typedef struct
{
    uint8_t seconds;
    uint8_t minutes;
    uint8_t hours;

    uint8_t day;
    uint8_t weekday;
    uint8_t month;
    uint16_t year;

    bool oscillatorStopped;
    bool batteryLow;
    bool alarmFlag;
    bool timerFlag;
} PCF85063A_TimeTypeDef;




HAL_StatusTypeDef PCF85063A_WriteRegister(I2C_HandleTypeDef *hi2c,uint8_t reg, uint8_t data);
HAL_StatusTypeDef PCF85063A_ReadRegister(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t *data);
HAL_StatusTypeDef PCF85063A_WriteRegisters(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t *data,uint8_t length);
HAL_StatusTypeDef PCF85063A_ReadRegisters(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t *data, uint8_t length);

HAL_StatusTypeDef PCF85063A_SetTime(I2C_HandleTypeDef *hi2c, const PCF85063A_TimeTypeDef *time);
HAL_StatusTypeDef PCF85063A_GetTime(I2C_HandleTypeDef *hi2c,PCF85063A_TimeTypeDef *time);

HAL_StatusTypeDef PCF85063A_SetUnixTime(I2C_HandleTypeDef *hi2c,uint32_t unixTime);
HAL_StatusTypeDef PCF85063A_GetUnixTime(I2C_HandleTypeDef *hi2c,uint32_t *unixTime);





#endif /* INC_PCF85063A_H_ */
