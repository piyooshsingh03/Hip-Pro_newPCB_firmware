/*
 * var.c
 *
 *  Created on: Nov 7, 2025
 *      Author: USER
 */
#include"var.h"
HALLSensor HAL={0};   // hall sensor vaslue
float BAT_Voltage,V_BAT_per=0;

const uint8_t Device_ID[19] = "RHPLHP0425V1.1A010";
const uint16_t Resettime = 8000;   // To reset time if sensor got wrong data or disconnect
const float alpha=0.1;   // LOW Pass Filter
const int16_t error = 404; // Error code
float dt=0.0f;// To calculate time interval for velocity
uint8_t  command = 0;    //Command to check function exicution
uint8_t CurrentPostureStatus = DEFAULT;    //Default setup for posture detection
uint8_t OldPostureStatus = DEFAULT;   // Temporary variable to store posture value


uint32_t ID = 0, FlashSize=0,RTC_Counter = 0;  // Flash ID check and RTC time holder
uint8_t receiveData[10], data[10]; // received  and send data Bluetooth data
uint8_t battery=0;

//Fall detection
uint8_t fall1_Lroll=0,fall1_Rroll=0;
float fall_accL=0.0,fall_accR=0.0;
float Old_fall_accL=0.0,Old_fall_accR=0.0;
uint8_t fallwindow=5;

int16_t Fall_LWx=300;
int16_t Fall_RWx=300;

// Data conversion according need
int16_t filteredR_Wx,filteredR_Wy,filteredR_Wz=0;
int16_t filteredL_Wx,filteredL_Wy,filteredL_Wz=0;



uint8_t Stand_count,walk_count,walkingflag,Walk2stand=0; // these are use to support walking device on senario
uint8_t five_sitcount =0;
uint16_t SOS_count=0,Inf_Def_count=0,Num_Of_Inflation=0;   //Button count and auto reinflATION

// walking calibration
int16_t accL1=0,accR1=0;
int8_t Lw=80,Rw=80;
const uint8_t unbalance[]="WRONG";

// Reset counter
int8_t countRST=0;

manualbutton MP_flag={0};

uint16_t MEDI_TIME[4];
uint8_t INTERVEL_TIME = 2;
uint8_t M_T_Flag = 0;
uint32_t MED_COUNTER_RTC = 0;
uint16_t MED_COUNTER = 0;

// Declare and initialize to 0
TestFlags testFlags = {0}; // Resets all flags to 0 (false)
// Declare and initialize to 0
IMUData LeftIMU, RightIMU= {0};
// Declare and initialize to 0
Timing time = {0};  // All values will be set to 0
PostureFlags posture = {0}; // Sets all flags to 0;
Activity Act_data={0};

 struct PostureAngles stand_L = {acc_stand_default, acc_stand_default, 4};
 struct PostureAngles stand_R = {acc_stand_default, acc_stand_default, -4};

 struct PostureAngles mid_L = {0, 50, 0};
 struct PostureAngles mid_R = {0, 50, 0};

 struct PostureAngles sit_L = {acc_sit_default, acc_sit_default, 1};
 struct PostureAngles sit_R = {acc_sit_default, acc_sit_default, 1};

 struct PostureAngles fall_L = {0,0,0};
 struct PostureAngles fall_R = {0,0,0};

// Walking Differences
 struct WalkDiff walkDiff = {1000, 1000};

// Acceleration Limits
 struct AccLimits accL = {acc_walkmin_default, acc_walkmax_default};
 struct AccLimits accR = {acc_walkmin_default, acc_walkmax_default};


// Angular velocity values
int8_t Lw_max = Lw_default;
int8_t Rw_max = Rw_default;
int8_t angularvR = 0, WR = 0;
int8_t angularvL = 0, WL = 0;

Balance_Test B_test = {0};

falldetect count = {0};

