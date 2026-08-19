/*
 * function.c
 *
 *  Created on: Nov 7, 2025
 *      Author: USER
 */
#include"function.h"
#include"var.h"
#include"GPIO.h"
#include"BT.h"
#include"flashio.h"
#include"stdbool.h"    // boolean operation
extern UART_HandleTypeDef huart1;
extern IWDG_HandleTypeDef hiwdg;


//IMU DATA PROCESSING
void leftsensor(void) {
	ICM20948_Read_Accel(&hi2c2, &ICMDataL, ICM20948_ADDR_LOW);
	ICM20948_Read_Gyro(&hi2c2, &ICMDataL, ICM20948_ADDR_LOW);
	HAL_Delay(1);
	accL1 = ICMDataL.ACCEL_Z_RAW;
	float ax= ICMDataL.Ax;
	float ay= ICMDataL.Ay;
	float az= ICMDataL.Az;
	float wx= ICMDataL.Gx;
	float wy= ICMDataL.Gy;
	float wz= ICMDataL.Gz;
	LeftIMU.pitch = -atan2(ax, sqrt(ay *ay + az * az)) * (180.0f / 3.14f);
	LeftIMU.roll = -atan2(ay, sqrt(ax * ax + ay * az)) * (180.0f / 3.14f);
	LeftIMU.yaw += wz * 0.1f;
	LeftIMU.accelarationL = sqrt(ax*ax+ ay*ay + az*az);
	LeftIMU.angularvelocityL=sqrt(wx*wx+wy*wy+wz*wz);

}
void rightsensor(void) {

	ICM20948_Read_Accel(&hi2c2, &ICMDataR, ICM20948_ADDR);
	ICM20948_Read_Gyro(&hi2c2, &ICMDataR, ICM20948_ADDR);
	HAL_Delay(1);
	accR1 = ICMDataR.ACCEL_Z_RAW;
	float ax= ICMDataR.Ax;
	float ay= ICMDataR.Ay;
	float az= ICMDataR.Az;
	float wx= ICMDataR.Gx;
	float wy= ICMDataR.Gy;
	float wz= ICMDataR.Gz;
	RightIMU.pitch = -atan2(ax, sqrt(ay *ay + az * az)) * (180.0f / 3.14f);
	RightIMU.roll = -atan2(ay, sqrt(ax * ax + ay * az)) * (180.0f / 3.14f);
	RightIMU.yaw +=wz * .01f;
	RightIMU.accelarationR = sqrt(ax*ax+ ay*ay + az*az);
	RightIMU.angularvelocityR=sqrt(wx*wx+wy*wy+wz*wz);
}
void Callibration_value(void)
{
    mid_L.pitch = (stand_L.pitch + sit_L.pitch) / 2;
    mid_L.roll  = (stand_L.roll + sit_L.roll) / 2;
    mid_L.yaw   = (stand_L.yaw + sit_L.yaw) / 2;

    mid_R.pitch = (stand_R.pitch + sit_R.pitch) / 2;
    mid_R.roll  = (stand_R.roll + sit_R.roll) / 2;
    mid_R.yaw   = (stand_R.yaw + sit_R.yaw) / 2;

    walkDiff.left = (accL.max - accR.min) - 100;
    walkDiff.right = (accR.max - accL.min) - 100;

    fall_L.roll=(sit_L.roll-20);
    fall_R.roll=(sit_R.roll-20);
}



// Posture

uint8_t checkPosture() {
		 int16_t acc_diff_LR = accL1 - accR1;
		 int16_t acc_diff_RL = accR1 - accL1;
		 //walking
	   if (((( LeftIMU.accelarationL >1.3) || (RightIMU.accelarationR>1.3))&&(( LeftIMU.accelarationL <3) || (RightIMU.accelarationR<3)))&& (posture.sittostand_flag ==0)&&testFlags.fall2==0){
		if (time.walk_candidate_time==0){time.walk_candidate_time=HAL_GetTick();}
	     if (HAL_GetTick()-time.walk_candidate_time>400)
		    {   posture.walk_flag = 1;
		      if(((LeftIMU.angularvelocityL>Lw_max )||(RightIMU.angularvelocityR>Rw_max))){
					    walk_count+=1; testFlags.fall1=0;testFlags.fall2=0;testFlags.fall3=0;
					    return ( LeftIMU.accelarationL >1.06) ? WALKING_LEFT : WALKING_RIGHT;  //is a ternary operator (also called a conditional operator), which is a shorthand for an if-else statement.
					      }
					    }
		 else
			return (OldPostureStatus);
		}
	   //Standing
		else if ((LeftIMU.roll > (mid_L.roll+5)) && (RightIMU.roll >(mid_R.roll+5)) && ((LeftIMU.angularvelocityL<Lw_max ||RightIMU.angularvelocityR<Rw_max)))
		{
			posture.sit_flag = 0;
			posture.sittostand_flag =0;
			posture.stand_flag = 1;
			posture.STF=1;
			testFlags.fall1=0;
			testFlags.fall2=0;
			testFlags.fall3=0;
			if(posture.walk_flag ==1){
				posture.walk_flag = 0;
				time.walk_tick=HAL_GetTick();
			}
			 return (HAL_GetTick() - time.walk_tick > 1500) ? STAND : OldPostureStatus;
		}
		// Sitting
		else if ((LeftIMU.roll < (mid_L.roll + 5)) && (RightIMU.roll < (mid_R.roll+5)) && (LeftIMU.roll>fall_L.roll || RightIMU.roll >fall_R.roll)&& (RightIMU.roll!=0 ||LeftIMU.roll!=0)) {
			posture.walk_flag = 0;
			posture.sittostand_flag = 1;
			walk_count=0;
			if (posture.sit_flag == 0) {
				posture.sit_flag = 1;
				time.sit_tick = HAL_GetTick();
			}
			 // flag for intial timer for2sec
			return (HAL_GetTick() - time.sit_tick > 200) ? SIT : OldPostureStatus;
		}
			return (OldPostureStatus);
}

// Commands

void commandFunc(void) {
	uint8_t size = 0;
	uint8_t data[13];

	data[0] = 0xaa;
	data[1] = command;
	data[2] = 0x55;
	switch (command) {
				//stand calibrate
	case 0x1:              //stand calibrate
			data[3] = ((LeftIMU.pitch) & 0xff);
			data[4] = ((LeftIMU.roll) & 0xff);
			data[5] = ((LeftIMU.yaw) & 0xff);
			data[6] = ((RightIMU.pitch) & 0xff);
			data[7] = ((RightIMU.roll) & 0xff);
			data[8] = ((RightIMU.yaw) & 0xff);
			size = 9;
			stand_L.pitch = LeftIMU.pitch;
			stand_L.roll  = LeftIMU.roll;
			stand_L.yaw   = LeftIMU.yaw;
			stand_R.pitch = RightIMU.pitch;
			stand_R.roll  = RightIMU.roll;
			stand_R.yaw   = RightIMU.yaw;
//		}
		break;
	case 0x11:
		data[3] = ((stand_L.pitch) & 0xff);
		data[4] = ((stand_L.roll) & 0xff);
		data[5] = ((stand_L.yaw) & 0xff);
		data[6] = ((stand_R.pitch) & 0xff);
		data[7] = ((stand_R.roll) & 0xff);
		data[8] = ((stand_R.yaw) & 0xff);
		size = 9;
		break;
	case 0x2:              //sit calibrate

			data[3] = ((LeftIMU.pitch) & 0xff);
			data[4] = ((LeftIMU.roll) & 0xff);
			data[5] = ((LeftIMU.yaw) & 0xff);
		    data[6] = ((RightIMU.pitch) & 0xff);
			data[7] = ((RightIMU.roll) & 0xff);
			data[8] = ((RightIMU.yaw) & 0xff);
			size = 9;
			sit_L.pitch = LeftIMU.pitch;
			sit_L.roll  = LeftIMU.roll;
			sit_L.yaw   = LeftIMU.yaw;
			sit_R.pitch = RightIMU.pitch;
			sit_R.roll  = RightIMU.roll;
			sit_R.yaw   = RightIMU.yaw;

		break;
	case 0x12:
		data[3] = ((sit_L.pitch) & 0xff);
		data[4] = ((sit_L.roll) & 0xff);
		data[5] = ((sit_L.yaw) & 0xff);
		data[6] = ((sit_R.pitch) & 0xff);
		data[7] = ((sit_R.roll) & 0xff);
		data[8] = ((sit_R.yaw) & 0xff);
		size = 9;
		break;
	case 0x3:
		uint32_t ticktock=HAL_GetTick();
		const uint16_t walk_Time=10000;
		send8bit(&huart1,03);   //acknowladgement to application to calibration start status
		//walk calibrate   (we have to also check posture for calibration)
		while(HAL_GetTick()-ticktock<walk_Time){
			if (accL1 >= ICMDataL.ACCEL_Z_RAW)
				{accL.min = ICMDataL.ACCEL_Z_RAW;}
		    if (accR1 <= ICMDataR.ACCEL_Z_RAW)
				{accR.max = ICMDataR.ACCEL_Z_RAW;}
		    if (accL1 <= ICMDataL.ACCEL_Z_RAW)
				{accL.max = ICMDataL.ACCEL_Z_RAW;}
		    if (accR1 >= ICMDataR.ACCEL_Z_RAW)
				{accR.min = ICMDataR.ACCEL_Z_RAW;}
			if (Lw<=LeftIMU.angularvelocityL)
			    {Lw_max=LeftIMU.angularvelocityL;}
			if (Rw<=RightIMU.angularvelocityR)
				{Rw_max=RightIMU.angularvelocityR;}
			HAL_Delay(10);
			Lw=LeftIMU.angularvelocityL;
			Rw=RightIMU.angularvelocityR;
			accL1=ICMDataL.ACCEL_Z_RAW;
			accR1=ICMDataR.ACCEL_Z_RAW;
		}
		data[3] = (((int16_t) accL.min) >>8&0xff);
		data[4] = (((int16_t) accL.min) & 0xff);
		data[5] = (((int16_t) accR.max) >>8&0xff);
		data[6] = (((int16_t) accR.max) & 0xff);
		data[7] = (((int16_t) accR.min)>>8&0xff);
		data[8] = (((int16_t) accR.min) & 0xff);
		data[9] = (((int16_t) accL.max) >>8&0xff);
		data[10] = (((int16_t) accL.max) & 0xff);
		data[11] = (((int8_t) Lw_max) & 0xff);
		data[12] = (((int8_t) Rw_max) & 0xff);
		size = 13;
		break;
	case 0x13:	//Get walking calibration data
		data[3] = (((int16_t) accL.min) >>8&0xff);
		data[4] = (((int16_t) accL.min) & 0xff);
		data[5] = (((int16_t) accR.max) >>8&0xff);
		data[6] = (((int16_t) accR.max) & 0xff);
		data[7] = (((int16_t) accR.min)>>8&0xff);
		data[8] = (((int16_t) accR.min) & 0xff);
		data[9] = (((int16_t) accL.max) >>8&0xff);
		data[10] = (((int16_t) accL.max) & 0xff);
		data[11] = (((int8_t) Lw_max) & 0xff);
		data[12] = (((int8_t) Rw_max) & 0xff);
		size = 13;
		break;
	 case 0x4: //rtc set
		RTC_Counter = receiveData[3] << 24 | receiveData[4] << 16
				| receiveData[5] << 8 | receiveData[6];
//		send32bitRTC(&huart1,counter);
//		setRTC(&hrtc, RTC_Counter);
		data[3] = (RTC_Counter >> 24) & 0xff;
		data[4] = (RTC_Counter >> 16) & 0xff;
		data[5] = (RTC_Counter >> 8) & 0xff;
		data[6] = RTC_Counter & 0xff;
		size = 7;
		break;

// balance test
				case 0x08:
					testFlags.Bal_test_Off = true ;  // balance test on
					testFlags.inf = true;
					data[3] = (testFlags.Bal_test_Off);
					size = 4;
					break;
				case 0x20:
					if(CurrentPostureStatus==SIT && testFlags.Bal_test_Off == true) {
					testFlags.thirty_second_test = true;   // 30 second sit to stand start
					data[3] = (testFlags.thirty_second_test);
//					data[4] = (RTC_Counter >> 24) & 0xff;
//					data[5] = (RTC_Counter >> 16) & 0xff;
//					data[6] = (RTC_Counter >> 8) & 0xff;
//					data[7] = RTC_Counter & 0xff;
					size =4;
					}
					else
					{sendPosture(&huart1, CurrentPostureStatus);size = 3;}
					break;
				case 0x21:
                    if(testFlags.Bal_test_Off == true){
                    	testFlags.thirty_second_test = false;  // 30 second sit to stand stop
					data[3] = (testFlags.thirty_second_test);
					sendMergedData(&huart1, B_test.B_SitCount, B_test.StandCount);
					size = 4;
                    }
					break;
				case 0x22:
					if (CurrentPostureStatus==SIT && testFlags.Bal_test_Off == true) {
						testFlags.five_time_test = true;   //5 times sit to stand start
					data[3] = (testFlags.five_time_test);
//					data[4] = (RTC_Counter >> 24) & 0xff;
//					data[5] = (RTC_Counter >> 16) & 0xff;
//					data[6] = (RTC_Counter >> 8) & 0xff;
//					data[7] = RTC_Counter & 0xff;
					B_test.Starttime=RTC_Counter;
					size = 4;
					}
					else
					{sendPosture(&huart1, CurrentPostureStatus);size = 3;}
					break;
				case 0x23:
					  if(testFlags.Bal_test_Off == true){
						  testFlags.five_time_test = false;   // 5 times sit to stand stop
					data[3] = (testFlags.five_time_test);
					B_test.FiveSittime=(RTC_Counter-B_test.Starttime);
					send8bit(&huart1,B_test.FiveSittime);
					  }
					size = 4;
					break;
				case 0x24:
					if (CurrentPostureStatus==SIT && testFlags.Bal_test_Off == true) {
						testFlags.TUG_test = true;   //TUG test start

					data[3] = (testFlags.TUG_test);
					B_test.TUGtimer1=RTC_Counter;
					size = 4;
					}
					else
					{sendPosture(&huart1, CurrentPostureStatus);size = 3;}
					break;
				case 0x25:
					 if(testFlags.Bal_test_Off == true){
						 testFlags.TUG_test = false;   // TUG test Stop
				   data[3] = (testFlags.TUG_test);
				   B_test.TotalTUG_time=(RTC_Counter-B_test.TUGtimer1);
				   sendMergedData(&huart1, B_test.TUGstep_count, B_test.TotalTUG_time);
				   B_test.TUGstep_count=0;B_test.TotalTUG_time=0;
				   size = 4;
					 }
				   break;
				case 0x26:
					if (CurrentPostureStatus==STAND && testFlags.Bal_test_Off == true) {
						testFlags.Four_Stage_test  = true;   //TUG test start
					data[3] = (testFlags.Four_Stage_test );
					data[4] = (RTC_Counter >> 24) & 0xff;
					data[5] = (RTC_Counter >> 16) & 0xff;
					data[6] = (RTC_Counter >> 8) & 0xff;
					data[7] = RTC_Counter & 0xff;
					size = 8;
					}
					else
					{sendPosture(&huart1, CurrentPostureStatus);size = 3;}
					break;

			case 0x28:
				if ( testFlags.Bal_test_Off == true) {

					testFlags.Four_Stage_test  = false;   // TUG test Stop
					data[3] = (testFlags.Four_Stage_test ) & 0xff;
					size = 4;
				   }
				    break;

	case 0x09:
		testFlags.Bal_test_Off = false;   // Stop balance test
		testFlags.five_time_test = false;
		testFlags.thirty_second_test = false;
		testFlags.TUG_test = false;
		testFlags.Four_Stage_test  = false;
					data[3] = (testFlags.Bal_test_Off) & 0xff;
					size = 4;
					break;
	case 0x29:
		BUZZER_ON();;
		  size = 3;
		  break;
	case 0x30:
			  size = 3;
			  break;
  //battery status
	case 0x07:
		      HAL_GPIO_WritePin(BAT_MEAS_EN_GPIO_Port, BAT_MEAS_EN_Pin, SET);
						data[3] = (battery) & 0xff;
						size = 4;
						break;
	// balance test stop
	case 0x14: //rtc get
		data[3] = (RTC_Counter >> 24) & 0xff;
		data[4] = (RTC_Counter >> 16) & 0xff;
		data[5] = (RTC_Counter >> 8) & 0xff;
		data[6] = RTC_Counter & 0xff;
		size = 7;
		break;
	case 0xab: //read press count
		data[3] = Inf_Def_count >> 8 & 0xff;
		data[4] = Inf_Def_count & 0xff;
		data[5] = SOS_count >> 8 & 0xff;
		data[6] = SOS_count & 0xff;
		data[7] = Num_Of_Inflation >> 8 & 0xff;
		data[8] = Num_Of_Inflation & 0xff;
		size = 9;
		break;
	case 0xac:
		//get total step count
		data[3] = (Act_data.Total_Step_count >> 24) & 0xff;
		data[4] = (Act_data.Total_Step_count >> 16) & 0xff;
		data[5] = (Act_data.Total_Step_count >> 8) & 0xff;
		data[6] = Act_data.Total_Step_count & 0xff;
		size = 7;
		break;
	case 0xc0: //get activity time

		sendMerged32byteData(&huart1, Act_data.TotalStand_time, Act_data.TotalSit_time, Act_data.TotalWalk_time);
		size = 3;
		break;

	case 0x06:
//		testFlags.ReadDataFromFlash = 1;
		testFlags.fall1=0; testFlags.fall2=0;testFlags.fall3=0;testFlags.StopSOS=1;
			size = 3;
			break;
	case 0xfe: //posture
		data[3] = (CurrentPostureStatus) & 0xff;
		size = 4;
		break;
	case 0x16:
		EraseAllSector();
		size = 3;
		break;
         // Reset device
	case 0xaa:
		sendString(&huart1,(uint8_t*)"X", 1);
		NVIC_SystemReset();
		break;
	case 0xbb: //Delete press count
		W25Q_Erase_Sector(6);
		size = 3;
		break;
	case 0xcc: //Delete total activity time
		W25Q_Erase_Sector(5);
		size = 3;
		break;
		//DEVICE id
	case 0xdd:
		sendString(&huart1, (int8_t*)Device_ID, 20);
		break;
	case 0xee: //Delete alarm
		W25Q_Erase_Sector(1);
		size = 3;
		break;
	case 0xff: //setting store
		writeSetting();
		size = 3;
		break;
	}
	command = 0;
	btReceiveReset(&huart1);
	sendString(&huart1,(int8_t*)data, size);
}
void HealthStatus() {
	    BAT_Voltage=(IN_VALUE[4] * (3.3f / 4095.0f));
		if (testFlags.Status==1)
		   {
			   if ( HAL_GetTick()-time.STtime>20000)
			   {
				   CheckStatus();time.STtime=HAL_GetTick();
			   }
		   }
		// total voltage on pin 2.837
		else if (BAT_Voltage < 2.2) {
			 greenledoff();
			 redled();           //glow red led
			}
		else {
				greenled();
				redledoff();
			}
}


void ManualMode(void){
	    // Read button states once
	    bool sw2_pressed = !HAL_GPIO_ReadPin(SW2_GPIO_Port, SW2_Pin);
	    bool sw1_pressed = !HAL_GPIO_ReadPin(SW1_GPIO_Port, SW1_Pin);

	    if ((sw2_pressed||sw1_pressed)&&testFlags.butonflag==0){
	    	testFlags.butonflag=1;
	    	time.Buttonpresed=HAL_GetTick();
            }

	    if (sw2_pressed) {
	    	if(HAL_GetTick()-time.Buttonpresed >=1500)
	    	{
	        HAL_Delay(70);
	        Inf_Def_count++;
	        handlePostureChange();
	        MP_flag.MSW = 1;
	        manualcountwrite();
	    	}

	     }
	    else if (sw1_pressed) {
	    	if( HAL_GetTick()-time.Buttonpresed >=1500 && HAL_GetTick()-time.Buttonpresed<2000)
	    		    	{
	            HAL_Delay(70);
	            BUttonSOS();
	            manualcountwrite();
	        }
	    	else if ((HAL_GetTick()-time.Buttonpresed) >=2000)
	    		  {
	    	      testFlags.fall1=0; testFlags.fall2=0;testFlags.fall3=0; testFlags.StopSOS=1;
	    		   }
	             }
	    else {
	    	    	testFlags.butonflag=0;time.Buttonpresed=0;
	    	    }
}

void Define_Mode(void){                                           // Manual Or Automatic
	if (CurrentPostureStatus != MP_flag.MSTATUS && MP_flag.MSW == 1)
			{
				CurrentPostureStatus = MP_flag.MSTATUS;
				OldPostureStatus = MP_flag.MSTATUS;

			}
			else{MP_flag.MSW = 0;}
}

void automaticMode(void){
	valvevalue();
	if (CurrentPostureStatus != OldPostureStatus && RTC_Counter != 0)
			{

		        updatepostureflags();
				OldPostureStatus = CurrentPostureStatus;
				standposture();
				sitposture();
				walkposture	();
			}
	       reinflation();
	       updatetime();
}


// System Status Functions
void CheckStatus() {
	    if (testFlags.I2c == 1){sendString(&huart1, (int8_t*)"b",1);testFlags.Status=1;}
		else if (testFlags.Spi == 1){sendString(&huart1,(int8_t*) "d", 1);testFlags.Status=1;}
		else if (testFlags.rtc == 1){sendString(&huart1,(int8_t*) "c", 1);testFlags.Status=1;}
		else if (testFlags.Imu== 1){sendString(&huart1,(int8_t*) "f", 1);testFlags.Status=1;}
		else if (testFlags.flash == 1){sendString(&huart1,(int8_t*) "e", 1);testFlags.Status=1;}
		else {sendString(&huart1, (int8_t*)"Z", 1);testFlags.Status=0;}
}
// Posture Handling Functions
void handlePostureChange(void) {
	if (CurrentPostureStatus == SIT || CurrentPostureStatus == 0 || MP_flag.MSTATUS == SIT) {
    if (INFLATION() != HAL_OK)  Error_Handler();
    dataWrite(RTC_Counter, MSTAND);
    sendPosture(&huart1, MSTAND);
    MP_flag.MSTATUS = STAND;
}
else if (CurrentPostureStatus == STAND || CurrentPostureStatus == 0 || MP_flag.MSTATUS == STAND) {
    if (DEFLATION() != HAL_OK)  Error_Handler();
    dataWrite(RTC_Counter, MSIT);
    sendPosture(&huart1, MSIT);
    MP_flag.MSTATUS = SIT;
    posture.hold = 1;  // hold deflation in standing
}}


// SOS Function
void SOS(void) {
	if(testFlags.StopSOS==0){
    SOS_count++;
    for (int i = 0; i < 5; i++) {
        sendString(&huart1, (int8_t*)"S", 1);
        BUZZER_ON();

    }
	}
}
void BUttonSOS(void) {
	if(testFlags.StopSOS==0){
    SOS_count++;
    for (int i = 0; i < 5; i++) {
        sendString(&huart1, (int8_t*)"B", 1);
        BUZZER_ON();
    }
  }
}

// Posture Update Functions
void updatepostureflags(void) {
	if((OldPostureStatus==STAND) && (CurrentPostureStatus == WALKING_LEFT || CurrentPostureStatus == WALKING_RIGHT))
						{
							posture.standtowalk_flag=1;
						}
						if((OldPostureStatus==SIT) && CurrentPostureStatus == STAND)
						{
							posture.sittostand_flag = 1;
						}
						if((OldPostureStatus==WALKING_LEFT ||OldPostureStatus==WALKING_RIGHT) && CurrentPostureStatus == STAND)
						{
							posture.walktostand_flag = 1;
						}
						if((OldPostureStatus==WALKING_LEFT ||OldPostureStatus==WALKING_RIGHT) && CurrentPostureStatus == SIT)
						{
							posture.walktosit_flag = 1;
						}
						if((OldPostureStatus==SIT) && (CurrentPostureStatus==WALKING_LEFT ||CurrentPostureStatus==WALKING_RIGHT))
							{
							 posture.sittowalk=1;
							}
}

void standposture() {
	if ((CurrentPostureStatus == STAND || posture.sittowalk==1 )&& posture.hold == 0 ) {
		                  sendPosture(&huart1, CurrentPostureStatus);
		                  testFlags.StopSOS=0;
							if (posture.standtowalk_flag==0)
							{
								if(INFLATION()!= HAL_OK)  HARDWRAE_ERROR();
								Stand_count+=1;
								walkingflag=1;uwTick = 0;
								posture.sittowalk=0;
							}
						   if (posture.STF==1 && posture.walktostand_flag==0)
						   {Act_data.TotalSit_time += (RTC_Counter - Act_data.old_sit_time);} // time will save only sit to stand
						 if (posture.walktostand_flag == 1)
						 {
							 Act_data.TotalWalk_time += (RTC_Counter - Act_data.old_walk_time);
							 posture.walktostand_flag = 0;
						 }
						 Act_data.old_stand_time = RTC_Counter;  Act_data.Step_count=0;
						  dataWrite(RTC_Counter, CurrentPostureStatus);
						  manualcountwrite();
						}
}
void sitposture() {
	if (CurrentPostureStatus == SIT) {
						        sendPosture(&huart1, CurrentPostureStatus);
						        if (testFlags.systemrst==0 && (testFlags.fall1==0 && testFlags.fall2==0)){
						        	if(DEFLATION()!=HAL_OK){HARDWRAE_ERROR();}
						        }
						  	  	walkingflag=1;uwTick = 0;
						  	    Walk2stand=0;
						  	  	if (posture.walk_flag == 0&&posture.sittostand_flag == 1 && posture.walktosit_flag==0){
						  	  	Act_data.TotalStand_time += (RTC_Counter - Act_data.old_stand_time);
						  	  	}  // time will save walk to stand or sit to stand
							  if (posture.walktosit_flag==1){
								  Act_data.TotalWalk_time += (RTC_Counter - Act_data.old_walk_time);
								  posture.walktosit_flag=0;
							  }
							  Act_data.old_sit_time = RTC_Counter;
							  posture.hold = 0;
							posture.standtowalk_flag=0;
							 Act_data.Step_count=0;
							dataWrite(RTC_Counter, CurrentPostureStatus);
							manualcountwrite();
							}
}
void walkposture() {

	if (((CurrentPostureStatus == WALKING_LEFT)||(CurrentPostureStatus == WALKING_RIGHT)) && (CurrentPostureStatus !=0)) {
										if(walk_count>=1 &&walkingflag==0 )
										{if(INFLATION()!= HAL_OK)  HARDWRAE_ERROR();
										walkingflag=1;
										Walk2stand=1;
										posture.standtowalk_flag=1;
										}
										if(walk_count>=6 ){
											sendPosture(&huart1, CurrentPostureStatus);
										}
									posture.standtowalk_flag=1;
									Act_data.Total_Step_count+=1;
									 Act_data.Step_count +=1;
						      if (posture.walk_flag == 1 && posture.sittostand_flag == 0 &&  Act_data.Step_count==1)
						      {
						    	  Act_data.TotalStand_time += (RTC_Counter - Act_data.old_stand_time);
						    	  Act_data.old_walk_time = RTC_Counter;
						      }   // time will save staND TO WALK
						//                     if (Step_count>10){sendPosture(&huart1, Step_count);Step_count=0;}}
							dataWrite(RTC_Counter, CurrentPostureStatus);
							manualcountwrite();
							}
}

// Re-inflation and Time Update
void reinflation() {

	if ((CurrentPostureStatus == STAND|| posture.walk_flag == 1)&& RTC_Counter != 0&& posture.hold == 0 && time.Reinflate > 300000) {
									if (INFLATION() != HAL_OK)  HARDWRAE_ERROR();
									Act_data.TotalStand_time += (RTC_Counter - Act_data.old_stand_time);
									Act_data.old_stand_time = RTC_Counter;
									Act_data.old_sit_time = RTC_Counter;
									uwTick = 0;
									Num_Of_Inflation += 1;
									manualcountwrite();
								}
}
void updatetime(void) {
	if (CurrentPostureStatus == SIT && RTC_Counter != 0&& time.Reinflate > 120000) {
									Act_data.TotalSit_time += (RTC_Counter - Act_data.old_sit_time);
									Act_data.old_sit_time = RTC_Counter;
									Act_data.old_stand_time = RTC_Counter;
									uwTick = 0;
									manualcountwrite();
								}
}


void Error(void) {
    redled();
    BUZZER_ON();
    NVIC_SystemReset();
}


void _30sec_test(void) {
//    B_test.ticktock = HAL_GetTick();
//    uint16_t timeout = 30000;
//
    if (testFlags.thirty_second_test) {
//        while (HAL_GetTick() - B_test.ticktock < timeout) {
//            CurrentPostureStatus = check_30sec_5time_test();
            sendLivedata();
//            RTC_Counter = (RTC->CNTH << 16) | (RTC->CNTL);
//            command = btFuncCheck(&huart1);
//            HAL_IWDG_Refresh(&hiwdg);
//            if (command != 0) commandFunc();
//            if (CurrentPostureStatus != OldPostureStatus) {
//                if (CurrentPostureStatus == SIT) B_test.B_SitCount++;
//                if (CurrentPostureStatus == STAND) B_test.StandCount++;
//
//                OldPostureStatus = CurrentPostureStatus;
//                sendPosture(&huart1, CurrentPostureStatus);
//
//            }
//        }
//
//        testFlags.thirty_second_test = false;
//        sendMergedData(&huart1, B_test.B_SitCount, B_test.StandCount);
//        B_test.B_SitCount =0;
//        B_test.StandCount =0;
    }
}

// 5 times sit-to-stand test
void _5timessittostand_test(void) {
    if (testFlags.five_time_test) {
    	sendLivedata();
//        if (CurrentPostureStatus != OldPostureStatus) {
//            OldPostureStatus = CurrentPostureStatus;
//            sendPosture(&huart1, CurrentPostureStatus);
//            if (CurrentPostureStatus == STAND) {five_sitcount++;}
//                if (five_sitcount >= 5 && CurrentPostureStatus == SIT) {
//                    testFlags.five_time_test = false;
//                    B_test.FiveSittime = (RTC_Counter - B_test.Starttime);
//                    five_sitcount = 0;
//                    send8bit(&huart1, B_test.FiveSittime);
//                    B_test.FiveSittime = 0;
//                }
//        }
    }
}

// TUG Test
void TUG_Test(void) {
    if (testFlags.TUG_test) {
    	sendLivedata();
//        if (CurrentPostureStatus == 5 || CurrentPostureStatus == 6 || CurrentPostureStatus == 1 || CurrentPostureStatus == 2) {
//            if (CurrentPostureStatus == 5 || CurrentPostureStatus == 6) B_test.TUGstep_count++;
//
//            if (CurrentPostureStatus != OldPostureStatus) {
//                OldPostureStatus = CurrentPostureStatus;
//                sendPosture(&huart1, CurrentPostureStatus);
//
//                if (CurrentPostureStatus == SIT && B_test.TUGstep_count >= 2) {
//                    testFlags.TUG_test = false;
//                    B_test.TotalTUG_time = (RTC_Counter - B_test.TUGtimer1);
//                    sendMergedData(&huart1, B_test.TUGstep_count, B_test.TotalTUG_time);
//                    B_test.TUGstep_count = 0;
//                    B_test.TotalTUG_time = 0;
//                }
//            }
//        }
    }
}

// 4-Stage Test
void _4Stage_test(void) {
    if (testFlags.Four_Stage_test) {
        B_test.ticktock = HAL_GetTick();

        while (HAL_GetTick() - B_test.ticktock < 2000) {
            if (WR <= RightIMU.angularvelocityR) WR = RightIMU.angularvelocityR;
            if (WL <= LeftIMU.angularvelocityL) WL = LeftIMU.angularvelocityL;
            Write4stage_data();
        }

        while (HAL_GetTick() - B_test.ticktock < 12000) {
            RTC_Counter = (RTC->CNTH << 16) | (RTC->CNTL);
            rightsensor();
            leftsensor();


            if (RightIMU.angularvelocityR > WR + 10 || LeftIMU.angularvelocityL > WL + 10) {
                angularvL = LeftIMU.angularvelocityL;
                angularvR = LeftIMU.angularvelocityR;
                BUZZER_ON();
                sendString(&huart1, (int8_t*)unbalance, 5);
                send8bit(&huart1, angularvL);
                send8bit(&huart1, angularvR);
            }
            HAL_IWDG_Refresh(&hiwdg);
        }
        WR = 0;
        WL = 0;
        testFlags.Four_Stage_test = false;
        send32bit(&huart1, RTC_Counter);
    }
}

// Balance test function
void balancetest(void) {
    while (testFlags.Bal_test_Off) {
    	HAL_IWDG_Refresh(&hiwdg);
        RTC_Counter = (RTC->CNTH << 16) | (RTC->CNTL);

        if (testFlags.inf) {
            if (INFLATION() != HAL_OK) Error();
            cal_data();
            testFlags.inf = false;
        }

        CurrentPostureStatus = checkPosture();
        command = btFuncCheck(&huart1);
        if (command != 0)
        	{
        	commandFunc();
        	}

        _30sec_test();
        _5timessittostand_test();
        TUG_Test();
        _4Stage_test();
    }
}

void sendLivedata(void)
{
	struct temp{
		int16_t AX;
		int16_t AY;
		int16_t AZ;
		int16_t GX;
		int16_t GY;
		int16_t GZ;
	};
	struct temp IMU_L,IMU_R,Gyro_L,Gyro_R;
	IMU_R.AX=ICMDataR.Ax *1000;
	IMU_R.AY=ICMDataR.Ay *1000;
	IMU_R.AZ=ICMDataR.Az *1000;
	IMU_L.AX=ICMDataL.Ax *1000;
	IMU_L.AY=ICMDataL.Ay *1000;
	IMU_L.AZ=ICMDataL.Az *1000;
	Gyro_R.GX=ICMDataR.Gx * 1000;
	Gyro_R.GY=ICMDataR.Gy * 1000;
	Gyro_R.GZ=ICMDataR.Gz * 1000;
	Gyro_L.GX=ICMDataL.Gx * 1000;
	Gyro_L.GY=ICMDataL.Gy * 1000;
	Gyro_L.GZ=ICMDataL.Gz * 1000;
	int8_t imuData[27];
	imuData[0] = (IMU_R.AX>>8)&0xff;
	imuData[1] = IMU_R.AX &0xff;
	imuData[2] = (IMU_R.AY>>8)&0xff;
	imuData[3] = IMU_R.AY &0xff;
	imuData[4] = (IMU_R.AZ>>8)&0xff;
	imuData[5] = IMU_R.AZ &0xff;
	imuData[6] = (IMU_L.AX>>8)&0xff;
	imuData[7] = IMU_L.AX &0xff;
	imuData[8] = (IMU_L.AY>>8)&0xff;
	imuData[9] = IMU_L.AY &0xff;
	imuData[10] = (IMU_L.AZ>>8)&0xff;
	imuData[11] = IMU_L.AZ &0xff;
	imuData[12] = (Gyro_R.GX>>8)&0xff;
	imuData[13] = Gyro_R.GX &0xff;
	imuData[14] = (Gyro_R.GY>>8)&0xff;
	imuData[15] = Gyro_R.GY &0xff;
	imuData[16] = (Gyro_R.GZ>>8)&0xff;
	imuData[17] = Gyro_R.GZ &0xff;
	imuData[18] = (Gyro_L.GX>>8)&0xff;
	imuData[19] = Gyro_L.GX &0xff;
	imuData[20] = (Gyro_L.GY>>8)&0xff;
	imuData[21] = Gyro_L.GY &0xff;
	imuData[22] = (Gyro_L.GZ>>8)&0xff;
	imuData[23] = (Gyro_L.GZ)&0xff;
	imuData[24] = RightIMU.pitch;
	imuData[25] = RightIMU.roll;
	imuData[26] = LeftIMU.pitch;
	imuData[27] = LeftIMU.roll;
	send12Floats(&huart1, imuData);
}
void cal_data(void)
{
	uint8_t calData[10];
	calData[0]=stand_L.pitch;
	calData[1]=stand_L.roll;
	calData[2]=stand_R.pitch;
	calData[3]=stand_R.roll;
	calData[4]=sit_L.pitch;
	calData[5]=sit_L.roll;
	calData[6]=sit_R.pitch;
	calData[7]=sit_R.roll;
	calData[8]=Lw_max;
	calData[9]=Rw_max;
	caldata(&huart1, calData);
}

// System reset function
void systemReset(void) {

    HAL_IWDG_Refresh(&hiwdg);
    static float previousLax = 0, previousLay = 0, previousLaz = 0;
    static float previousRax = 0, previousRay = 0, previousRaz = 0;

    if ((ICMDataL.Ax == 0 && ICMDataL.Ay == 0 && ICMDataL.Az == 0) ||
        (ICMDataR.Ax == 0 && ICMDataR.Ay== 0 && ICMDataR.Az == 0) ||
        (previousLax == ICMDataL.Ax && previousLay == ICMDataL.Ay && previousLaz == ICMDataL.Az) ||
        (previousRax == ICMDataR.Ax && previousRay == ICMDataR.Ay && previousRaz == ICMDataR.Az)) {

        if (testFlags.systemrst==0) {
            testFlags.systemrst = 1;
            time.Sysrsttime = HAL_GetTick();
        }

        if (testFlags.systemrst == 1 && HAL_GetTick() - time.Sysrsttime > Resettime) {
        	testFlags.systemrst = 0;
        	 Error();
        }

    }
    if ((ICMDataR.ACCEL_Z_RAW>20000 && ICMDataR.ACCEL_Z_RAW<-8000 )|| (ICMDataL.ACCEL_Z_RAW>20000 && ICMDataL.ACCEL_Z_RAW<-8000))
           {
           	 Error();
           }

}


void readmemory(void)
{
    ID = W25Q_ReadID();
    FlashSize=W25Q_GetFlashSize()/1000000;
    bool isValidID = (ID == 15679512 || ID == 15679510 || ID == 15679511);
    sendString(&huart1, (int8_t*)(isValidID ? "E" : "e"), 1);
    if (!isValidID) {
        testFlags.flash = 1;
    }
}
