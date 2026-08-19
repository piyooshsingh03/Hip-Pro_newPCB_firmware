#include"BT.h"
#include"var.h"
#include"string.h"
uint8_t  uart_tx_complete = 0;
void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart) {
    uart_tx_complete = 1;  // Set flag when transmission is done
}

void sendString(UART_HandleTypeDef *huartx, int8_t *data, uint16_t size) {
    uart_tx_complete = 0;  // Reset flag before transmission
    HAL_UART_Transmit_IT(huartx, (uint8_t*)data, size);

     while (!uart_tx_complete);  // Wait until transmission is done
}

void send8bit(UART_HandleTypeDef *huartx,uint8_t Data){
	HAL_UART_Transmit_IT(huartx,&Data, 1);
	HAL_Delay(100);
}
void sendPosture(UART_HandleTypeDef *huartx,uint8_t Data){
	uint8_t data[1];
//	data[0] = 0xaa; data[1]=0xfe; data[2]=0x55;
	data[0]=Data;
	HAL_UART_Transmit_IT(huartx,&data[0], 1);
	HAL_Delay(100);
}
void send16bit(UART_HandleTypeDef *huartx,int16_t Data){
	uint8_t data[2];
	data[0] = (Data>>8)&0xff;
	data[1] = Data&0xff;
	HAL_UART_Transmit_IT(huartx,&data[0], 2);
	HAL_Delay(100);
}

void send32bit(UART_HandleTypeDef *huartx,uint32_t Data){
	uint8_t data[4];
	data[0] = (Data>>24)&0xff;
	data[1] = (Data>>16)&0xff;
	data[2] = (Data>>8)&0xff;
	data[3] = Data&0xff;
	HAL_UART_Transmit_IT(huartx,&data[0], 4);
	HAL_Delay(100);
}
void send32bitfloat_M1(UART_HandleTypeDef *huartx, float Data) {
    union {
        float f;
        uint8_t b[4];
    } converter;

    converter.f = Data;

    // Send in big-endian (MSB first)
    HAL_UART_Transmit(huartx, converter.b, 4, HAL_MAX_DELAY);
}
void send32bitfloat_M2(UART_HandleTypeDef *huartx, float Data) {
    uint8_t data[4];
    memcpy(data, &Data, 4);   // Copy raw float bytes into array
    HAL_UART_Transmit(huartx, data, 4, HAL_MAX_DELAY);
}

void send32bitRTC(UART_HandleTypeDef *huartx,uint32_t Data){
	uint8_t data[7];
	data[0] = 0xAA;
	data[1] = 0x14;
	data[2] = 0x55;
	data[3] = (Data>>24)&0xff;
	data[4] = (Data>>16)&0xff;
	data[5] = (Data>>8)&0xff;
	data[6] = Data&0xff;
	HAL_UART_Transmit_IT(huartx,&data[0], 7);
	HAL_Delay(100);
}
#define BUFFERSIZE 10
extern uint8_t receiveData[BUFFERSIZE];

/*

 */
void btReceiveReset(UART_HandleTypeDef *huartx){
	huartx->pRxBuffPtr = receiveData;
	huartx->RxXferCount = BUFFERSIZE;
	memset(receiveData, 0, sizeof(receiveData));  // Clear buffer
	HAL_UART_Receive_IT(huartx, receiveData, 5); // Re-enable UART interrupt
}
 uint8_t btFuncCheck(UART_HandleTypeDef *huartx){
	if((huartx->RxXferCount)<5){
			  if(receiveData[0]==0xAA && receiveData[2]==0x55){
				  return(receiveData[1]);
			  }
			  else { btReceiveReset(huartx); return(0); }
		  }
	return 0;  // Ignore incomplete data
}

uint8_t btFuncInit(UART_HandleTypeDef *huartx){
	  HAL_UART_Receive_IT(huartx, receiveData, BUFFERSIZE);
	  return 0;
}
void sendMergedData(UART_HandleTypeDef *huart, uint8_t data1, uint8_t data2) {
    uint8_t buffer[3];
    buffer[0] = 0xFF; // First byte
    buffer[1] = data2; // Second byte
    buffer[2] = data2; // Second byte

    // Send both bytes together
    HAL_UART_Transmit(huart, buffer, 3, HAL_MAX_DELAY);
    HAL_Delay(100);
}
void sendMerge6dData(UART_HandleTypeDef *huart, uint8_t data1, uint8_t data2, uint8_t data3, uint8_t data4, uint8_t data5, uint8_t data6) {
    uint8_t buffer[6];
    buffer[0] = data1; // First byte
    buffer[1] = data2; // Second byte
    buffer[2] = data3; // First byte
    buffer[3] = data4; // Second byte
    buffer[4] = data5; // First byte
    buffer[5] = data6; // Second byte


    // Send both bytes together
    HAL_UART_Transmit(huart, buffer, 6, HAL_MAX_DELAY);
    HAL_Delay(100);
}
void sendMerged32byteData(UART_HandleTypeDef *huart, uint32_t data1, uint32_t data2, uint32_t data3) {

    uint8_t data[12];  // 3 values, each 4 bytes (32 bits)

        // Combine the three 32-bit values into the buffer
        data[0] = (Act_data.TotalStand_time >> 24) & 0xFF;
        data[1] = (Act_data.TotalStand_time >> 16) & 0xFF;
        data[2] = (Act_data.TotalStand_time >> 8) & 0xFF;
        data[3] =  Act_data.TotalStand_time & 0xFF;

        data[4] = (Act_data.TotalSit_time >> 24) & 0xFF;
        data[5] = (Act_data.TotalSit_time >> 16) & 0xFF;
        data[6] = (Act_data.TotalSit_time >> 8) & 0xFF;
        data[7] =  Act_data.TotalSit_time & 0xFF;

        data[8] = (Act_data.TotalWalk_time >> 24) & 0xFF;
        data[9] = (Act_data.TotalWalk_time >> 16) & 0xFF;
        data[10]= (Act_data.TotalWalk_time >> 8) & 0xFF;
        data[11]= (Act_data.TotalWalk_time) & 0xFF;

    // Send both bytes together
    HAL_UART_Transmit(huart, data, 12, HAL_MAX_DELAY);
    HAL_Delay(100);
}
void send12Floats(UART_HandleTypeDef *huartx, uint8_t data[30]) {
    int8_t packet[30];   // 2 bytes header + 24 bytes data
    packet[0] = 0xA4;     // Header byte 1
    packet[29] = 0xB4;     // Header byte 2

    memcpy(&packet[1], data, 28);

    HAL_UART_Transmit(huartx, packet, 30, HAL_MAX_DELAY);
    HAL_Delay(100);
}
void caldata(UART_HandleTypeDef *huartx, uint8_t data[12]) {
    uint8_t packet[12];   // 2 bytes header + 24 bytes data
    packet[0] = 0x44;     // Header byte 1
    packet[11] = 0x44;     // Header byte 2

    memcpy(&packet[1], data, 10);

    HAL_UART_Transmit(huartx, packet, 12, HAL_MAX_DELAY);
    HAL_Delay(100);
}
