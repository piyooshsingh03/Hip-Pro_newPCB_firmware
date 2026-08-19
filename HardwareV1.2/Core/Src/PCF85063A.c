/*
 *  PCF85063A.c
 *
 *  Created on: 27-Jul-2026
 *      Author: USER
 */
#include "PCF85063A.h"

static uint32_t CalendarToUnix(const PCF85063A_TimeTypeDef *time);

// write one byte to one rtc
HAL_StatusTypeDef PCF85063A_WriteRegister(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t data)
{
	HAL_StatusTypeDef ret;
	uint8_t txBuffer[2];
	txBuffer[0]=reg;
	txBuffer[1]=data;
	ret=HAL_I2C_Master_Transmit(hi2c, PCF85063A_I2C_ADDR, txBuffer, 2, HAL_MAX_DELAY);
	return ret;
}


// Read one register from the RTC
HAL_StatusTypeDef PCF85063A_ReadRegister(I2C_HandleTypeDef *hi2c,uint8_t reg,uint8_t *data)
{

	  HAL_StatusTypeDef status;
	    /* Send register address */
	    status = HAL_I2C_Master_Transmit(hi2c, PCF85063A_I2C_ADDR,&reg,1, HAL_MAX_DELAY);

	    if(status != HAL_OK)
	    {
	        return status;
	    }
	    /* Read register data */
	    return HAL_I2C_Master_Receive(hi2c, PCF85063A_I2C_ADDR, data,1, HAL_MAX_DELAY);
}

//  This is the multiple-register version.
HAL_StatusTypeDef PCF85063A_WriteRegisters(I2C_HandleTypeDef *hi2c,uint8_t reg, uint8_t *data,uint8_t length)
{
    HAL_StatusTypeDef status;
    uint8_t txBuffer[16];

    if(length > 15)
    {
        return HAL_ERROR;
    }

    txBuffer[0] = reg;

    for(uint8_t i = 0; i < length; i++)
    {
        txBuffer[i + 1] = data[i];
    }

    status = HAL_I2C_Master_Transmit( hi2c, PCF85063A_I2C_ADDR, txBuffer, length + 1, HAL_MAX_DELAY);

    return status;
}
//This is the multiple-register read version.

HAL_StatusTypeDef PCF85063A_ReadRegisters( I2C_HandleTypeDef *hi2c, uint8_t reg,uint8_t *data, uint8_t length)
{
    HAL_StatusTypeDef status;

    status = HAL_I2C_Master_Transmit(hi2c,PCF85063A_I2C_ADDR, &reg, 1, HAL_MAX_DELAY);

    if(status != HAL_OK)
    {
        return status;
    }

    return HAL_I2C_Master_Receive( hi2c, PCF85063A_I2C_ADDR, data,length, HAL_MAX_DELAY);
}

//This is your high-level RTC time-writing function.
HAL_StatusTypeDef PCF85063A_SetTime(I2C_HandleTypeDef *hi2c, const PCF85063A_TimeTypeDef *time)
{
	uint8_t txBuffer[7];
	uint8_t year;
	year = (uint8_t)(time->year % 100);
	txBuffer[0] = DecimalToBCD(time->seconds) & 0x7F;
	txBuffer[1] = DecimalToBCD(time->minutes) & 0x7F;
	txBuffer[2] = DecimalToBCD(time->hours)   & 0x3F;
	txBuffer[3] = DecimalToBCD(time->day)     & 0x3F;
	txBuffer[4] = time->weekday               & 0x07;
	txBuffer[5] = DecimalToBCD(time->month)   & 0x1F;
	txBuffer[6] = DecimalToBCD(year);

	return PCF85063A_WriteRegisters(hi2c,PCF85063A_REG_SECONDS,txBuffer,sizeof(txBuffer));
}

// reads all seven time registers.
HAL_StatusTypeDef PCF85063A_GetTime(I2C_HandleTypeDef *hi2c,PCF85063A_TimeTypeDef *time)
{
	uint8_t rxBuffer[7];
	HAL_StatusTypeDef status;

	status = PCF85063A_ReadRegisters(hi2c,PCF85063A_REG_SECONDS, rxBuffer,sizeof(rxBuffer));

	if(status != HAL_OK)
	{
	    return status;
	}
	time->oscillatorStopped = ((rxBuffer[0] & 0x80) != 0);

	time->seconds = BCDToDecimal(rxBuffer[0] & 0x7F);
	time->minutes = BCDToDecimal(rxBuffer[1] & 0x7F);
	time->hours   = BCDToDecimal(rxBuffer[2] & 0x3F);

	time->day     = BCDToDecimal(rxBuffer[3] & 0x3F);
	time->weekday = rxBuffer[4] & 0x07;
	time->month   = BCDToDecimal(rxBuffer[5] & 0x1F);

	time->year    = 2000 + BCDToDecimal(rxBuffer[6]);

	return HAL_OK;
}


static bool IsLeapYear(uint16_t year)
{
    if ((year % 400) == 0)
    {
        return true;
    }

    if ((year % 100) == 0)
    {
        return false;
    }

    if ((year % 4) == 0)
    {
        return true;
    }

    return false;
}

static uint8_t DaysInMonth(uint8_t month, uint16_t year)
{
    switch (month)
    {
        case 1:     // January
        case 3:     // March
        case 5:     // May
        case 7:     // July
        case 8:     // August
        case 10:    // October
        case 12:    // December
            return 31;

        case 4:     // April
        case 6:     // June
        case 9:     // September
        case 11:    // November
            return 30;

        case 2:     // February
            return IsLeapYear(year) ? 29 : 28;

        default:
            return 0;       // Invalid month
    }
}

static void UnixToCalendar(uint32_t unixTime,
                           PCF85063A_TimeTypeDef *time)
{
	unixTime += PCF85063A_IST_OFFSET_SECONDS;
    uint32_t days=0;
    uint32_t totalDays=0;
    uint32_t remainingSeconds=0;

    uint16_t year = 1970;
    uint16_t daysInYear=0;

    uint8_t month = 1;
    uint8_t daysInCurrentMonth=0;

    days = unixTime / SECONDS_PER_DAY;
    totalDays = days;
    remainingSeconds = unixTime % SECONDS_PER_DAY;

    while (1)
    {
        daysInYear = IsLeapYear(year) ? 366 : 365;

        if (days < daysInYear)
        {
            break;
        }

        days -= daysInYear;
        year++;
    }

    time->year = year;

    while (1)
    {
        daysInCurrentMonth = DaysInMonth(month, year);

        if (days < daysInCurrentMonth)
        {
            break;
        }

        days -= daysInCurrentMonth;
        month++;
    }

    time->month = month;
    time->day = (uint8_t)(days + 1);

    time->hours = (uint8_t)(remainingSeconds / SECONDS_PER_HOUR);
    remainingSeconds %= SECONDS_PER_HOUR;

    time->minutes = (uint8_t)(remainingSeconds / SECONDS_PER_MINUTE);
    remainingSeconds %= SECONDS_PER_MINUTE;

    time->seconds = (uint8_t)remainingSeconds;

    time->weekday = (uint8_t)((totalDays + 4U) % 7U);
}

HAL_StatusTypeDef PCF85063A_SetUnixTime(I2C_HandleTypeDef *hi2c, uint32_t unixTime)
{
    PCF85063A_TimeTypeDef time;
    UnixToCalendar(unixTime, &time);
    return PCF85063A_SetTime(hi2c, &time);
}

static uint32_t CalendarToUnix(const PCF85063A_TimeTypeDef *time)
{
	    uint32_t unixTime = 0;
	    uint32_t days = 0;

	    uint16_t year;
	    uint8_t month;

	    for (year = 1970; year < time->year; year++)
	    {
	        if (IsLeapYear(year))
	        {
	            days += 366U;
	        }
	        else
	        {
	            days += 365U;
	        }
	    }
	    for (month = 1; month < time->month; month++)
	    {
	        days += DaysInMonth(month, time->year);
	    }

	    days += (time->day - 1U);

	    unixTime = days * SECONDS_PER_DAY;

	    unixTime += (uint32_t)time->hours * SECONDS_PER_HOUR;
	    unixTime += (uint32_t)time->minutes * SECONDS_PER_MINUTE;
	    unixTime += (uint32_t)time->seconds;

	    /* Convert local IST time back to standard Unix (UTC) */
	    unixTime -= PCF85063A_IST_OFFSET_SECONDS;

	    return unixTime;
}

HAL_StatusTypeDef PCF85063A_GetUnixTime(I2C_HandleTypeDef *hi2c, uint32_t *unixTime)
{
    HAL_StatusTypeDef status;
    PCF85063A_TimeTypeDef time;

    status = PCF85063A_GetTime(hi2c, &time);
    if (status != HAL_OK)
    {
        return status;
    }

    *unixTime = CalendarToUnix(&time);

    return HAL_OK;
}
