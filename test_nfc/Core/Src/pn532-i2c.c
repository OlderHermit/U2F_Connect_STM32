/*
 * pn532-i2c.c
 *
 *  Created on: Nov 3, 2023
 *      Author: Zdzisław Jaxa-Małachowski
 */

#include "pn532-i2c.h"

extern I2C_HandleTypeDef hi2c1;
uint8_t PN532_ACK_RESPONSE[] = {0x01, 0x00, 0x00, 0xFF, 0x00, 0xFF};

size_t Make_Frame_For_Send(uint8_t* data, size_t size, uint8_t* output, size_t output_size){
	int checksum = 0;

	if (size + 7 > PN532_FRAME_MAX_LENGTH) {
		printf("Too much data for a frame got %d\r\n", size);
		return 0;
		//printf("Too much data making extended frame\r\n");
		//return Make_Frame_For_Send_Big(data, size, output, output_size); // Data must be array of 1 to 255 bytes.
	}
	if (output_size < size + 7) {
		printf("Frame size was too small\r\n");
		return 0;
	}

	output[0] = PN532_PREAMBLE;
	output[1] = PN532_STARTCODE1;
	output[2] = PN532_STARTCODE2;
	output[3] = size & 0xFF;
	output[4] = (~size + 1) & 0xFF;

	for (uint8_t i = 0; i < size; i++) {
		output[5 + i] = data[i];
		checksum += data[i];
	}
	output[size + 5] = (~checksum + 1) & 0xFF;
	output[size + 6] = PN532_POSTAMBLE;
	return size + 7;
}

//appears to does not work -> splicing data into less packets //internal buffer will return error over 264 bytes // messages over 180 are likely to return 0x0b error
size_t Make_Frame_For_Send_Big(uint8_t* data, size_t size, uint8_t* output, size_t output_size){
	int checksum = 0;

	if (size + 10 > PN532_FRAME_MAX_LENGTH * 257 || size < 1) {
		printf("Too much data for extended frame\r\n");
		return 0; // Data must be array of 1 to 256 * 256 + 256 bytes.
	}
	if (output_size < size + 10) {
		printf("Frame size was too small\r\n");
		return 0;
	}

	output[0] = PN532_PREAMBLE;
	output[1] = PN532_STARTCODE1;
	output[2] = PN532_STARTCODE2;
	output[3] = 0xFF;
	output[4] = 0xFF;
	output[5] = (size >> 8) & 0xFF;
	output[6] = size & 0xFF;
	output[7] = (~(output[5] + output[6]) + 1) & 0xFF;

	for (int i = 0; i < size; i++) {
		output[8 + i] = data[i];
		checksum += data[i];
	}
	output[size + 8] = (~checksum + 1) & 0xFF;
	output[size + 9] = PN532_POSTAMBLE;
	return size + 10;
}

int Check_For_ACK(void){
	HAL_StatusTypeDef result;
	uint8_t ackbuff[6];
	result = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, ackbuff, sizeof(ackbuff), HAL_MAX_DELAY);

	if (result != HAL_OK) {
		printf("I2C receive error for ACK: %d\r\n", result);
		return HAL_ERROR;
	}

	for(int i = 0; i < sizeof(ackbuff); i++){
		if(ackbuff[i] != PN532_ACK_RESPONSE[i]){
			printf("I2C receive error ACK at %d expected 0x%X and got 0x%X\r\n", i, PN532_ACK_RESPONSE[i], ackbuff[i]);
			return HAL_ERROR;
		}
	}
	return HAL_OK;
}

int Read_Frame(uint8_t* frame, size_t size){
	HAL_StatusTypeDef result;
	HAL_Delay(150);
	if (Check_For_ACK() != HAL_OK){
		return HAL_ERROR;
	}
	HAL_Delay(50);
	result = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frame, size, HAL_MAX_DELAY);
	if (result != HAL_OK) {
		printf("I2C receive error for main frame: %d\r\n", result);
		return HAL_ERROR;
	}
	if (frame[4] == 0){
		printf("Unable to read data size = 0\r\n");
		return HAL_ERROR;
	}
	return HAL_OK;
}

int Read_Frame_Awaiting(uint8_t* frame, size_t size){
	uint8_t first = 1;
	HAL_StatusTypeDef result;
	HAL_Delay(50);
	while(Check_For_ACK() != HAL_OK){//timeout?
		HAL_Delay(500);
	}
	HAL_Delay(150);
	while(first == 1 || frame[0] == 0){//timeout needed
		first = 0;
		result = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frame, size, HAL_MAX_DELAY);
		if (result != HAL_OK) {
			printf("I2C receive error for main frame: %d\r\n", result);
			return HAL_ERROR;
		}
		if (frame[4] == 0){
			printf("Unable to read data size = 0\r\n");
			return HAL_ERROR;
		}
		//add check of sum?
	}
	return HAL_OK;
}

int Read_Frame_No_ACK(uint8_t* frame, size_t size){
	HAL_StatusTypeDef result;
	HAL_Delay(50);
	result = HAL_I2C_Master_Receive(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frame, size, HAL_MAX_DELAY);
	if (result != HAL_OK) {
		printf("I2C receive error for main frame: %d\r\n", result);
		return HAL_ERROR;
	}
	if (frame[4] == 0){
		printf("Unable to read data size = 0\r\n");
		return HAL_ERROR;
	}
	return HAL_OK;
}

void Set_SAM_Configuration(uint8_t mode){
	/*
	 * - 0x01, normal mode
	 * - 0x14, timeout 50ms * 20 = 1 second
	 * - 0x01, use IRQ pin
	 */
	uint8_t response[PN532_FRAME_MAX_LENGTH];
	uint8_t setSam[] = {
			PN532_HOST_CODE, PN532_COMMAND_SAMCONFIGURATION, mode, 0x14, 0x01
	};
	uint8_t frameToSend[sizeof(setSam) + 7];
	Make_Frame_For_Send(setSam, sizeof(setSam), frameToSend, sizeof(frameToSend));

	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frameToSend, sizeof(frameToSend), HAL_MAX_DELAY);
	HAL_Delay(100);
	Read_Frame(response, sizeof(response));

	if (response[6] != 0xD5) {
		printf("Invalid frame identifier\r\n");
		return;
	}

	if (response[7] != 0x15) {
		printf("Invalid command code\r\n");
		return;
	}
	printf("Default configuration loaded\r\n");
}

void Get_Firmware_Version(void){
	uint8_t response[PN532_FRAME_MAX_LENGTH];
	uint8_t getFirmwareVersion[] = {PN532_HOST_CODE, PN532_COMMAND_GETFIRMWAREVERSION};
	uint8_t frameToSend[sizeof(getFirmwareVersion) + 7];
	Make_Frame_For_Send(getFirmwareVersion, sizeof(getFirmwareVersion), frameToSend, sizeof(frameToSend));

	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frameToSend, sizeof(frameToSend), HAL_MAX_DELAY);
	HAL_Delay(100);
	Read_Frame(response, sizeof(response));

	if (response[6] != 0xD5) {
		printf("Invalid frame identifier\r\n");
		return;
	}

	if (response[7] != 0x03) {
		printf("Invalid command code\r\n");
		return;
	}

	uint8_t icVersion = response[8];
	uint8_t fwVersion1 = response[9];
	uint8_t fwVersion2 = response[10];

	printf(" IC Version: 0x%X\r \n", icVersion);
	printf("Firmware Version: %d.%d\r \n", fwVersion1, fwVersion2);
}

int Read_Passive_Target(uint8_t* uid){
	uint8_t response[PN532_FRAME_MAX_LENGTH];
	uint8_t readPasiveTarget[] = {PN532_HOST_CODE, PN532_COMMAND_INLISTPASSIVETARGET, 0x01, 0x00};
	uint8_t frameToSend[sizeof(readPasiveTarget) + 7];
	Make_Frame_For_Send(readPasiveTarget, sizeof(readPasiveTarget), frameToSend, sizeof(frameToSend));

	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frameToSend, sizeof(frameToSend), HAL_MAX_DELAY);
	HAL_Delay(100);
	Read_Frame(response, sizeof(response));

	if (response[6] != 0xD5) {
		#ifdef PRINT_DEBUG
			printf("Invalid frame identifier\r\n");
		#endif
		return 0;
	}

	if (response[7] != 0x4B) {
		printf("Invalid command code\r\n");
		return 0;
	}

	if (response[8] != 0x01) {
		printf("More than one card detected\r\n");
		return 0;
	}

	if (response[13] > 7) {
		printf("Card seems to have too long UID\r\n");
		return 0;
	}

	for(int i = 0; i < response[13]; i++){
		uid[i] = response[14+i];
	}
	return response[13];

}

int In_Data_Exchange(uint8_t* data, size_t size, uint8_t* response, size_t response_size){
	uint8_t inDataExchange[size + 2];
	uint8_t read_frame[256];
	uint16_t read_size = 0;
	bool first = true;
	bool resend = false;
	uint8_t remaining = 0;
	uint8_t number_of_expected_packets = 0;

	inDataExchange[0] = PN532_HOST_CODE;
	inDataExchange[1] = PN532_COMMAND_INDATAEXCHANGE;
	memcpy(inDataExchange + 2, data, size * sizeof(uint8_t));

	uint8_t frameToSend[sizeof(inDataExchange) + 7];
	Make_Frame_For_Send(inDataExchange, sizeof(inDataExchange), frameToSend, sizeof(frameToSend));

	//frame for continue //12 + AID - NFC wrap, 2 - PN532 command, 7 - data size, 7 - PN532 wrap
	uint8_t frameToSend_rest[21 + sizeof(AID) + 7];

	HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frameToSend, sizeof(frameToSend), HAL_MAX_DELAY);
	HAL_Delay(500);
	while(first || remaining > 0){//timeout needed cout-out

		//continue
		 if(!first || resend){
			Make_Frame_For_Send_Rest(number_of_expected_packets - remaining, frameToSend_rest, sizeof(frameToSend_rest));
			if(read_frame[8] == 0x01){//lost target
				while(1){////timeout needed cout-out
					uint8_t uid[4];
					int uidSize = Read_Passive_Target(uid);
					if(uidSize != 0){
						printf("new uid found\r\n");
						break;
					}
					HAL_Delay(100);
					printf("mobile not found\r\n");
				}
			}
			HAL_I2C_Master_Transmit(&hi2c1, (uint16_t)PN532_I2C_ADDRESS, frameToSend_rest, sizeof(frameToSend_rest), HAL_MAX_DELAY);
			HAL_Delay(500);
			resend = false;
		}
		Read_Frame_Awaiting(read_frame, sizeof(read_frame));
		printf("\r\n");
		if (read_frame[6] != 0xD5) {
			printf("Invalid frame identifier\r\n");
			resend = true;
			continue;
		} else if (read_frame[7] != 0x41) {
			printf("Invalid command code\r\n");
			resend = true;
			continue;
		} else if (read_frame[8] != 0x00) {//still breaks 0b -> full stop 01 -> infinite ack at 4
			printf("Invalid command status got %02x\r\n", read_frame[8]);
			HAL_Delay(300);
			resend = true;
			continue;
		} else if (read_frame[read_frame[4] + 6 - 2] != 0x90 || read_frame[read_frame[4] + 6 - 1] != 0x00){
			printf("Invalid NFC response status %02x%02x\r\n", read_frame[read_frame[4] + 6 - 2], read_frame[read_frame[4] + 6 - 1]);
			return -1;
		}

		if(!Is_Checksum_Correct(read_frame)){
			resend = true;
			continue;
		}
		if (first && read_frame[9] == 0x01){
			//10 = pn532 status code etc. - 4, length & checksum - 2, return preambule - 3, frame number - 1
			//6 = return preambule - 3, frame number - 1, return code - 2
			memcpy(response, read_frame + 10, (read_frame[4]-6) * sizeof(uint8_t));
			printf("single part data finished\r\n");
			return read_frame[4]-11+5;
		}
		if (first){
			number_of_expected_packets = read_frame[9];
			remaining = number_of_expected_packets - 1;
			first = false;
			memcpy(response, read_frame + 10, (read_frame[4]-6) * sizeof(uint8_t));
			read_size += read_frame[4]-6;
			printf("waiting for next part of data, expect %d more \r\n", remaining);
		} else {
			remaining--;
			//9 = no frame number - 1
			//5 = no frame number - 1
			memcpy(response + read_size, read_frame + 9, (read_frame[4]-5) * sizeof(uint8_t));
			read_size += read_frame[4]-5;
			printf("waiting for next part of data, expect %d more \r\n", remaining);
		}
	}

	printf("multiple part data finished %d packets remaining\r\n", remaining);
	return read_size;
}

void Make_Frame_For_Send_Rest(uint8_t packet_index, uint8_t* frameToSend_rest, size_t frameToSend_rest_size){
	uint8_t* send_rest_frame_NFC = malloc((7 + 12) * sizeof(uint8_t) + sizeof(AID));
	uint8_t send_rest_data[7];
	send_rest_data[0] = 0x00;
	send_rest_data[1] = 0x04;
	send_rest_data[2] = 0x00;
	send_rest_data[3] = 0x00;
	send_rest_data[4] = 0x00;
	send_rest_data[5] = 0x00;
	send_rest_data[6] = packet_index;

	size_t send_rest_frame_NFC_size = Make_Packet_To_Send_NFC(send_rest_data, sizeof(send_rest_data), send_rest_frame_NFC);
	uint8_t inDataExchange_rest[send_rest_frame_NFC_size + 2];
	inDataExchange_rest[0] = PN532_HOST_CODE;
	inDataExchange_rest[1] = PN532_COMMAND_INDATAEXCHANGE;
	memcpy(inDataExchange_rest + 2, send_rest_frame_NFC, send_rest_frame_NFC_size * sizeof(uint8_t));
	Make_Frame_For_Send(inDataExchange_rest, sizeof(inDataExchange_rest), frameToSend_rest, frameToSend_rest_size);
}

//for time being only for normal frames
bool Is_Checksum_Correct(uint8_t* data){
	uint16_t checksum = 0;
	if((data[4] + data[5]) & 0xFF != 0x00){
		printf("length error len = %02x, check = %02x\r\n", data[4], data[5]);
		return false;
	}
	for(int i = 0; i < data[4]; i++){
		checksum += data[6 + i];
	}
	if((checksum + data[6 + data[4]]) & 0xFF != 0x00){
		printf("checksum error\r\n");
		return false;
	}
	return true;
}


