/*
 * var.h
 *
 *  Created on: Nov 7, 2025
 *      Author: USER
 */

#ifndef INC_VAR_H_
#define INC_VAR_H_
#include "stm32f1xx_hal.h"
#include "math.h"
// Posture default
#define STAND  1
#define SIT 2
#define WALKING_LEFT 5
#define WALKING_RIGHT 6
#define DEFAULT 0
#define MSTAND  3
#define MSIT 4
#define Fall 7

// Calibration default
#define acc_stand_default 50
#define acc_sit_default 30
#define acc_walkmax_default 2000
#define acc_walkmin_default 500
#define Lw_default 70
#define Rw_default 70
// posture
extern  uint8_t CurrentPostureStatus ;    //Default setup for posture detection
extern uint8_t OldPostureStatus ;   // Temporary variable to store posture value

//hall value
//hall value
typedef struct {
	volatile float hal1;
	volatile float hal2;
	volatile float hal3;
	volatile float hal4;
} HALLSensor;
extern  HALLSensor HAL;
extern float BAT_Voltage,V_BAT_per;


extern const uint8_t Device_ID[19];
extern const uint16_t Resettime;   // To reset time if sensor got wrong data or disconnect
extern const float alpha;   // LOW Pass Filter
extern const int16_t error ; // Error code
extern float dt;   // To calculate time interval for velocity
extern uint8_t command;    //Command to check function exicution
extern  uint8_t CurrentPostureStatus ;    //Default setup for posture detection
extern uint8_t OldPostureStatus ;   // Temporary variable to store posture value

extern uint32_t ID,FlashSize,RTC_Counter ;  // Flash ID check and RTC time holder
extern uint8_t receiveData[10], data[10]; // received  and send data Bluetooth data

extern float bat_voltage, Hal1, Hal2, Hal3, Hal4;
extern uint16_t IN_value[5];

//hall value
extern float hal1,hal2,hal3,hal4;

//Fall detection
extern uint8_t fall1_Lroll,fall1_Rroll;
extern float fall_accL,fall_accR;
extern float Old_fall_accL,Old_fall_accR;
extern uint8_t fallwindow;

extern int16_t Fall_LWx;
extern int16_t Fall_RWx;

extern uint8_t battery;  // battery status in percent


// Data conversion according need

extern uint8_t Stand_count,walk_count,walkingflag,Walk2stand; // these are use to support walking device on senario
extern uint8_t five_sitcount;
extern uint16_t SOS_count,Inf_Def_count,Num_Of_Inflation;   //Button count and auto reinflATION

// walking calibration
extern int16_t accL1,accR1;
extern int8_t Lw,Rw;
extern const uint8_t unbalance[];

// Reset counter
extern int8_t countRST;
extern int8_t RSTflag;



//manual button press
typedef struct {
uint8_t MSW : 1;
uint8_t MSTATUS : 1;
uint8_t SOS : 1;
}manualbutton;   //manual process flags
extern manualbutton MP_flag;

// Bitfield for boolean flags
typedef struct  {
    uint8_t Bal_test_Off : 1;
    uint8_t thirty_second_test : 1;
    uint8_t five_time_test : 1;
    uint8_t TUG_test : 1;
    uint8_t Four_Stage_test : 1;
    uint8_t First_Stage_test : 1;
    uint8_t Second_Stage_test : 1;
    uint8_t Third_Stage_test : 1;
    uint8_t Fourth_Stage_test : 1;
    uint8_t inf : 1;
    uint8_t ReadDataFromFlash : 1; // bool variable to data read from flash
    uint8_t I2c : 1;// Check flag for Device status
    uint8_t Spi : 1;
    uint8_t rtc : 1;
    uint8_t Imu : 1;
    uint8_t flash : 1;
    uint8_t Status : 1;
    uint8_t systemrst: 1;  // Reset flag
    uint8_t RSTflag: 1;
    uint8_t fall1: 1;   // Prefall
   	uint8_t fall2: 1;  // Post fall
   	uint8_t fall3: 1;  // Post fall
    uint8_t StopSOS: 1;   // Stop sos
	uint8_t prefall: 1;
	uint8_t falldetected: 1;
	uint8_t butonflag: 1;
	uint8_t sw2_pressed:1;
}TestFlags;
extern TestFlags testFlags;


 typedef struct {
    int8_t pitch, roll, yaw;
    float accelarationR,accelarationL;
    float angularvelocityR,angularvelocityL;
}IMUData;
extern IMUData LeftIMU, RightIMU;


typedef struct {
	  uint32_t Oldtime;     // set old time to find difference
	  uint32_t STtime;      // STtime time intervel to check status of device
	  uint32_t Runtime;     // Reinflation after set at 5 minuts
	  uint32_t Reinflate;   // Reinflation after set at 5 minuts
	  uint32_t Sysrsttime;  // reset time for system in mall function
	  uint32_t sit_tick;    // for starting siting timer
	  uint32_t walk_tick;   // for starting walking timer
	  uint32_t tick;    //Valve open and close
	  uint32_t falldetectiontime;  //Fall detection checking
	  uint32_t fallnotdetectiont;  //Fall detection checking
	  uint32_t walk_candidate_time;   // To deferantiat walking and falling
	  uint32_t Buttonpresed;
	  uint32_t INF_time;
	  uint32_t INF_start;
} Timing;
extern Timing time;

// Bitfield for posture flags
typedef struct {
    uint8_t stand_flag : 1;
    uint8_t walkcheck : 1;
    uint8_t sit_flag : 1;
    uint8_t walk_flag : 1;
    uint8_t standtowalk_flag : 1;
    uint8_t walktostand_flag : 1;
    uint8_t walktosit_flag : 1;
    uint8_t sittostand_flag : 1;
    uint8_t standtosit_flag : 1;
    uint8_t STF : 1;
    uint8_t hold : 1; // to hold deflation while standing
    uint8_t sittowalk : 1;

}PostureFlags;
extern PostureFlags posture;
typedef struct {
	uint32_t Total_Step_count;
	uint8_t  Step_count;   // Temperory step count
	uint32_t TotalStand_time;
	uint32_t old_stand_time;
	uint32_t TotalSit_time;
	uint32_t old_sit_time;
	uint32_t TotalWalk_time;
	uint32_t old_walk_time;
}Activity;
extern Activity Act_data;


struct PostureAngles {
    int8_t pitch;
    int8_t roll;
    int8_t yaw;
};

struct AccLimits {
    int16_t min;
    int16_t max;
};

struct WalkDiff {
    int16_t left;
    int16_t right;
};

// Grouped Posture Angles
 extern struct PostureAngles stand_L ;
 extern struct PostureAngles stand_R ;

 extern struct PostureAngles mid_L ;
 extern struct PostureAngles mid_R ;

 extern struct PostureAngles sit_L ;
 extern struct PostureAngles sit_R ;

 extern struct PostureAngles  fall_L;
 extern struct PostureAngles  fall_R ;

// Walking Differences
 extern struct WalkDiff walkDiff ;

// Acceleration Limits
 extern struct AccLimits accL ;
 extern struct AccLimits accR ;

// Angular velocity values
extern int8_t Lw_max;
extern int8_t Rw_max;
extern int8_t angularvR , WR ;
extern int8_t angularvL , WL ;

typedef struct {

	// 30 second test
	uint8_t StandCount;
	uint8_t B_SitCount;

	// Time counter of balance test
	uint64_t ticktock ;
	uint32_t TUGtimer1;
	uint8_t TotalTUG_time;
	uint8_t TUGstep_count;

	// 5 time sit to stand timecounter
	uint32_t Starttime;
	uint8_t FiveSittime;

}Balance_Test;
extern Balance_Test B_test;

typedef struct {
	uint8_t accl;
	uint8_t movement;
 }falldetect;

 extern falldetect count;

#endif /* INC_VAR_H_ */
