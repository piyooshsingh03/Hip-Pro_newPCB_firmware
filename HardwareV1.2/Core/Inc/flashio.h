/*
 * flashio.h
 *
 *  Created on: Nov 10, 2025
 *      Author: USER
 */

#ifndef INC_FLASHIO_H_
#define INC_FLASHIO_H_

#include "stm32f1xx_hal.h"
//#include"var.h"
// block 0 sector 0 will be used as setting

#define pitch_stand_addressL 0x0 //2 byte //sector 0
#define roll_stand_addressL 0x1  //2 byte
#define yaw_stand_addressL 0x2  //2 byte
#define pitch_stand_addressR 0x3 //2 byte //sector 0
#define roll_stand_addressR 0x4 //2 byte
#define yaw_stand_addressR 0x5 //2 byte


#define pitch_sit_addressL 0x6 //2 byte //sector 0
#define roll_sit_addressL 0x7  //2 byte
#define yaw_sit_addressL 0x8  //2 byte
#define pitch_sit_addressR 0x9 //2 byte //sector 0
#define roll_sit_addressR 0xa //2 byte
#define yaw_sit_addressR 0xb //2 byte

//walking data
#define accL_min_address 0x18
#define accR_min_address 0x1a
#define accL_max_address 0x1c
#define accR_max_address 0x1e
#define Lw_max_address 0x20
#define Rw_max_address 0x21



#define med_address 0x1000 //sector1

#define Stand_time_address 0x5000 //sector5
#define Sit_time_address 0x5004 //sector5
#define Walk_time_address 0x5008 //sector5

#define Inf_Def_count_address 0x6000  //sector 6
#define SOS_count_address 0x6002
#define Inf_count_address 0x6004
#define Total_Step_count_address 0x600a
#define countRST_address 0x6100  //Address for reset count



//#define flash_read_address 0x2000 // 4byte //sector2

/////////////////////////////////////////////
#define flash_write_address 0x8000 // 4byte //sector8
#define data_start_address 0x9000ul    //sector9
#define setting_start_address 0x0ul


#define angularvR_address 0x10000
#define angularvL_address 0x10001
/////////////////////////// Posture read write addaress and function

extern uint32_t writeAddress;
extern uint32_t readAddress ;
extern uint16_t MEDI_TIME[4];

uint8_t writeFlash(uint32_t rtc, uint8_t posture);
void readFlash(void);
int16_t Read_int16_flash(uint32_t address);
void write_int16_flash(uint32_t address,int16_t Data);
uint32_t Read_uint32_flash(uint32_t address);
void write_uint32_flash(uint32_t address,uint32_t Data);
void ReadSetting(void);
void writeSetting(void);
void Read4stage_data(void);
void Readmanualcount(void);
void manualcountwrite(void);
void rstcountWrite(void);
void rstcountRead(void);
void Write4stage_data(void);
void dataWrite(uint32_t counter,uint8_t posture);
void EraseAllSector(void);
void Reset(void);
void EraseNextSector(void);
void cal_data(void);

#endif /* INC_FLASHIO_H_ */
