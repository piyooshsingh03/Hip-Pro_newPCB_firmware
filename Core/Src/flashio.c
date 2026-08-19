/*
 * flashio.c
 *
 *  Created on: Nov 10, 2025
 *      Author: USER
 */
#include"flashio.h"
#include"var.h"
uint32_t writeAddress = data_start_address;
uint32_t readAddress = data_start_address;
uint8_t writeFlash(uint32_t rtc, uint8_t posture){
	uint8_t data[5],temp=0;
	data[0] = (rtc&0xFF000000)>>24;
	data[1] = (rtc&0x00FF0000)>>16;
	data[2] = (rtc&0x0000FF00)>>8;
	data[3] = (rtc&0x000000FF)>>0;
	data[4] = posture;
	W25Q_Write(writeAddress,0,5,data);
	writeAddress+=5;
}
void readFlash(void){
	uint32_t rtc=0,status=0;
	rtc|=((uint32_t)W25Q_Read_Byte(readAddress));
	rtc=rtc<<8;
	rtc|=((uint32_t)W25Q_Read_Byte(readAddress+1));
	rtc=rtc<<8;
	rtc|=((uint32_t)W25Q_Read_Byte(readAddress+2));
	rtc=rtc<<8;
	rtc|=((uint32_t)W25Q_Read_Byte(readAddress+3));
	status = W25Q_Read_Byte(readAddress+4);
	//writeSetting();
}

///////////////////Genral 16 bit and 32 bit read write function
int16_t Read_int16_flash(uint32_t address){
	uint8_t data[2];
	data[0] = W25Q_Read_Byte(address);
	data[1] = W25Q_Read_Byte(address+1);
	return(((int16_t)(data[0])<<8)+data[1]);
}
void write_int16_flash(uint32_t address,int16_t Data){
	uint8_t data[2];
	data[0] = (Data>>8)&0xff;
	data[1] = Data&0xff;
	//W25Q_Write_16bit(address,data);
	W25Q_Write_Byte(address,data[0]);
	W25Q_Write_Byte(address+1,data[1]);
	data[0] = W25Q_Read_Byte(address);
	data[1] = W25Q_Read_Byte(address+1);
}
uint32_t Read_uint32_flash(uint32_t address){
	uint8_t data[4];
	//W25Q_Read(address,0,4,data);
	data[0] = W25Q_Read_Byte(address);
	data[1] = W25Q_Read_Byte(address+1);
	data[2] = W25Q_Read_Byte(address+2);
	data[3] = W25Q_Read_Byte(address+3);
	uint32_t temp = (((uint32_t)data[0])<<24) | (((uint32_t)data[1])<<16) | (((uint32_t)data[2])<<8) | (((uint32_t)data[3])<<0);
	return(temp);
}
void write_uint32_flash(uint32_t address,uint32_t Data){
	uint8_t data[4];
	data[0] = (Data>>24)&0xff;
	data[1] = (Data>>16)&0xff;
	data[2] = (Data>>8)&0xff;
	data[3] = (Data>>0)&0xff;
	//W25Q_Write_32bit(address,data);
	W25Q_Write_Byte(address,data[0]);
	W25Q_Write_Byte(address+1,data[1]);
	W25Q_Write_Byte(address+2,data[2]);
	W25Q_Write_Byte(address+3,data[3]);
}

//////////////// Setting write and read function
void ReadSetting(void){
	stand_L.pitch = W25Q_Read_Byte(pitch_stand_addressL);
	if(stand_L.pitch<-90 || stand_L.pitch>90) stand_L.pitch = 50;//acc_stand_default;
	stand_L.roll = W25Q_Read_Byte(roll_stand_addressL);
	if(stand_L.roll<0 || stand_L.roll>90) stand_L.roll = 50;//acc_stand_default;
	stand_L.yaw = W25Q_Read_Byte(yaw_stand_addressL);
//    if(standY_L<0 || standYaw_L>90) standYaw_L = 50;//acc_stand_default;
	stand_R.pitch = W25Q_Read_Byte(pitch_stand_addressR);
	if(stand_R.pitch<-90 || stand_R.pitch>90) stand_R.pitch = 50;//acc_stand_default;
	stand_R.roll = W25Q_Read_Byte(roll_stand_addressR);
	if(stand_R.roll<0 || stand_R.roll>90) stand_R.roll = 50;//acc_stand_default;
	stand_R.yaw = W25Q_Read_Byte(yaw_stand_addressR);
//		if(standYaw_R<0 || standYaw_R>90) standYaw_R = 50;//acc_stand_default;


	sit_L.pitch = W25Q_Read_Byte(pitch_sit_addressL);
    if(sit_L.pitch<-90 || sit_L.pitch>90) sit_L.pitch = 50;//acc_stand_default;
	sit_L.roll = W25Q_Read_Byte(roll_sit_addressL);
	if(sit_L.roll<-30 || sit_L.roll>90) sit_L.roll = 50;//acc_stand_default;
	sit_L.yaw = W25Q_Read_Byte(yaw_sit_addressL);
//		if(sitYaw_L<-30 || sitYaw_L>90) sitYaw_L = 50;//acc_stand_default;
	sit_R.pitch = W25Q_Read_Byte(pitch_sit_addressR);
	if(sit_R.pitch<-90 || sit_R.pitch>90) sit_R.pitch = 50;//acc_stand_default;
	sit_R.roll = W25Q_Read_Byte(roll_sit_addressR);
	if(sit_R.roll<-30 || sit_R.roll>90) sit_R.roll = 50;//acc_stand_default;
	sit_R.yaw = W25Q_Read_Byte(yaw_sit_addressR);
//		if(sitYaw_R<-30 || sitYaw_R>90) sitYaw_R = 50;//acc_stand_default;

	accL.min = Read_int16_flash(accL_min_address);
//	   if(accL_min <-2000 || accL_min>2000) accL_min= 500;//default;
	accR.min= Read_int16_flash(accR_min_address);
//	  if(accR_min <-2000 || accR_min>2000) accR_min= 500;//default;
	accL.max= Read_int16_flash(accL_max_address);
//	   if(accL_max<500 || accL_max>3000) accL_max = 2000;//default;
	accR.max= Read_int16_flash(accR_max_address);
//       if(accR_max<500 || accR_max>3000) accR_max = 2000;//default;

		Lw_max = W25Q_Read_Byte(Lw_max_address);
	if(Lw_max<50 || Lw_max>400) Lw_max = 80;//default;
		Rw_max = W25Q_Read_Byte(Rw_max_address);
	if(Rw_max<50 || Rw_max>400) Rw_max = 90;//default;

	MEDI_TIME[0] = Read_int16_flash(med_address);
	MEDI_TIME[1] = Read_int16_flash(med_address+0x02);
	MEDI_TIME[2] = Read_int16_flash(med_address+0x04);
	MEDI_TIME[3] = Read_int16_flash(med_address+0x06);

}
void writeSetting(void){
	W25Q_Erase_Sector(0);
	W25Q_Erase_Sector(1);
	W25Q_Delay(5);
	//write_uint32_flash(flash_write_address,writeAddress);

	W25Q_Write_Byte(pitch_stand_addressL,stand_L.pitch);
	W25Q_Write_Byte(roll_stand_addressL,stand_L.roll);
	W25Q_Write_Byte(yaw_stand_addressL,stand_L.yaw);
	W25Q_Write_Byte(pitch_stand_addressR,stand_R.pitch);
	W25Q_Write_Byte(roll_stand_addressR,stand_R.roll);
	W25Q_Write_Byte(yaw_stand_addressR,stand_R.yaw);


	W25Q_Write_Byte(pitch_sit_addressL,sit_L.pitch);
	W25Q_Write_Byte(roll_sit_addressL,sit_L.roll);
	W25Q_Write_Byte(yaw_sit_addressL,sit_L.yaw);
	W25Q_Write_Byte(pitch_sit_addressR,sit_R.pitch);
	W25Q_Write_Byte(roll_sit_addressR,sit_R.roll);
	W25Q_Write_Byte(yaw_sit_addressR,sit_R.yaw);

    write_int16_flash(accL_min_address,accL.min);
	write_int16_flash(accR_min_address,accR.min);
	write_int16_flash(accL_max_address,accL.max);
	write_int16_flash(accR_max_address,accR.max);


	W25Q_Write_Byte(Lw_max_address,Lw_max);
	W25Q_Write_Byte(Rw_max_address,Rw_max);

	write_int16_flash(med_address,MEDI_TIME[0]);
	write_int16_flash(med_address+0x02,MEDI_TIME[1]);
	write_int16_flash(med_address+0x04,MEDI_TIME[2]);
	write_int16_flash(med_address+0x06,MEDI_TIME[3]);

//	write_uint32_flash(acc_stand_address,standAccConst);
}

void Read4stage_data(void)
{
	angularvR = W25Q_Read_Byte(angularvR_address);
	angularvL = W25Q_Read_Byte(angularvL_address);
}

void Readmanualcount(void)
{
	Inf_Def_count = Read_int16_flash(Inf_Def_count_address);
	SOS_count = Read_int16_flash(SOS_count_address);
	Num_Of_Inflation = Read_int16_flash(Inf_count_address);

	Act_data.TotalStand_time =  Read_uint32_flash(Stand_time_address);    // total stand time
	Act_data.TotalSit_time =  Read_uint32_flash(Sit_time_address);       // total sit time
	Act_data.TotalWalk_time =  Read_uint32_flash(Walk_time_address);       // total sit time
	Act_data.Total_Step_count = Read_uint32_flash(Total_Step_count_address);       // total sit time

}

void manualcountwrite(void)
{
	   W25Q_Erase_Sector(6);
	   W25Q_Erase_Sector(5);
		W25Q_Delay(5);
		write_int16_flash(Inf_Def_count_address,Inf_Def_count);
		write_int16_flash(SOS_count_address,SOS_count);
		write_int16_flash(Inf_count_address,Num_Of_Inflation);

		write_uint32_flash(Stand_time_address,Act_data.TotalStand_time);
		write_uint32_flash(Sit_time_address,Act_data.TotalSit_time);
		write_uint32_flash(Walk_time_address,Act_data.TotalWalk_time);

		write_uint32_flash(Total_Step_count_address,Act_data.Total_Step_count);
}
void rstcountWrite(void)
{
	W25Q_Erase_Sector(6);
	W25Q_Delay(5);
	W25Q_Write_Byte(countRST_address,countRST);  // to store rest count for limited rst
}
void rstcountRead(void)
{
	countRST = W25Q_Read_Byte(countRST_address);
}
void Write4stage_data(void)
{
	W25Q_Erase_Sector(16);
	W25Q_Write_Byte(angularvR_address,angularvR);
	W25Q_Write_Byte(angularvL_address,angularvL);
}

///// basic fucntion to read and write data
void dataWrite(uint32_t counter,uint8_t posture){
	write_uint32_flash(writeAddress,counter); writeAddress+=4;
	W25Q_Write_Byte(writeAddress,posture); writeAddress++;
	W25Q_Erase_Sector(8);
	write_uint32_flash(flash_write_address,writeAddress);
}
void EraseAllSector(void){
	uint16_t MaxSector = 512;
	for(uint16_t i=4;i<MaxSector;i++)
	{
		W25Q_Erase_Sector(i);
	}
	W25Q_Erase_Sector(1);
	writeAddress = data_start_address;
	write_uint32_flash(flash_write_address,writeAddress);
}
void Reset(void){
	chipErase();
	writeAddress = data_start_address;
	write_uint32_flash(flash_write_address,writeAddress);
	writeSetting();
}
void EraseNextSector(void){
	uint8_t Sector = writeAddress/4096+1;
	W25Q_Erase_Sector(Sector);
}



