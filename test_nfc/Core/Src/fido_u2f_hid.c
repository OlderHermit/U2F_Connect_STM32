/*
 * fido_u2f_hid.c
 *
 *  Created on: Dec 1, 2023
 *      Author: Zdzisław Jaxa-Małachowski
 */
#include "fido_u2f_hid.h"
/*
static HidStruct hidStruct = {.ChannelId = {0, 0, 0, 0}, .command = U2FHID_NONE, .finishedPacketSequence = true, .expectedSize = 0, .remainingSize = 0};
static uint32_t generatedChannels[] = {0x10111213, 0x01020304};//[10];//to change
static int generatedChannelsCount = 0;

HidStruct* Get_Hid_Struct(void){
	return &hidStruct;
}

size_t Parce_Hid_Packet(uint8_t* data, uint8_t* output){
	int howMuchData = 0;
	bool checkIfItIsInit = (data[4] & 0x80);//all commands starts with 0b1xxxxxxx

	if(checkIfItIsInit && !hidStruct.finishedPacketSequence){
		printf("New packet started unexpectedly\r\n");
	}
	if(checkIfItIsInit) {
		memcpy(hidStruct.ChannelId, data, sizeof(hidStruct.ChannelId));
		hidStruct.command = data[4];
		hidStruct.finishedPacketSequence = false;
		hidStruct.expectedSize = data[5] * 256 + data[6];
		hidStruct.remainingSize = hidStruct.expectedSize;
	}
	if(checkIfItIsInit){
		howMuchData = (hidStruct.remainingSize > 57)? 57 : hidStruct.remainingSize;
		memcpy(output, data + 7, howMuchData * sizeof(uint8_t));
	} else {
		//check seq
		howMuchData = (hidStruct.remainingSize > 59)? 59 : hidStruct.remainingSize;
		memcpy(output, data + 5, howMuchData * sizeof(uint8_t));

	}

	hidStruct.remainingSize -= howMuchData;
	if(hidStruct.remainingSize == 0){
		hidStruct.finishedPacketSequence = true;
	}
	return howMuchData;
}

size_t Make_Packet_To_Send(uint8_t* data, size_t data_size, uint8_t* output, size_t output_size, uint8_t command, uint8_t sequnce, int wholeSequenceSize){
	if (output_size != 64){
		printf("buffer should have size of 64; (0x40)\r\n");
		return 0;
	}
	memcpy(output, hidStruct.ChannelId, sizeof(hidStruct.ChannelId));
	if(wholeSequenceSize > 0){
		output[4] = command;
		output[5] = (wholeSequenceSize >> 8) & 0xFF;
		output[6] = wholeSequenceSize & 0xFF;
		if (data_size > 57){
			printf("data should not be greater then 57 for init packet got %d \r\n", data_size);
			return 0;
		}
		memcpy(output + 7, data, sizeof(uint8_t)*data_size);
		return data_size;
	}
	output[4] = sequnce;
	if (data_size > 59){
		printf("data should not be greater then 59 for init packet got %d \r\n", data_size);
		return 0;
	}
	memcpy(output + 5, data, sizeof(uint8_t)*data_size);
	return data_size;
}

void Splice_Data_And_Send(uint8_t* data, size_t data_size, uint8_t command, USBD_HandleTypeDef* hUsbDeviceFS) {
	if(data_size <= 57){
		uint8_t packet[64];
		Make_Packet_To_Send(data, data_size, packet, sizeof(packet), command, 0, data_size);
		USBD_CUSTOM_HID_SendReport(hUsbDeviceFS, packet, sizeof(packet));
	} else {
		size_t remaining_size = data_size;
		size_t send_already_size = 0;
		uint8_t packet[64];
		Make_Packet_To_Send(data, 57, packet, sizeof(packet), command, 0, data_size);
		USBD_CUSTOM_HID_SendReport(hUsbDeviceFS, packet, sizeof(packet));
		remaining_size -= 57;
		send_already_size += 57;

		for(int i = 0; remaining_size > 0; i++){
			size_t take_data_size = (remaining_size > 59)?59:remaining_size;
			Make_Packet_To_Send(data + send_already_size, take_data_size, packet, sizeof(packet), command, i, 0);
			USBD_CUSTOM_HID_SendReport(hUsbDeviceFS, packet, sizeof(packet));
			remaining_size -= take_data_size;
			send_already_size += take_data_size;
		}
	}
}

void Handle_Init(uint8_t* data, size_t data_size, USBD_HandleTypeDef* hUsbDeviceFS){
	//uint8_t packet[19];
	uint8_t packet[64];
	packet[0] = 0xff;
	packet[1] = 0xff;
	packet[2] = 0xff;
	packet[3] = 0xff;
	packet[4] = U2FHID_INIT;
	packet[5] = 0x00;
	packet[6] = 17;
	for(int i = 0; i < 8; i++){
		packet[i+7] = data[i];
	}
	packet[15] = 0x10;
	packet[16] = 0x11;
	packet[17] = 0x12;
	packet[18] = 0x13;
	packet[19] = 2;//test
	packet[20] = 2;
	packet[21] = 1;
	packet[22] = 1;
	packet[23] = 0;//no wink no lock*//*
	for(int i = 19; i < sizeof(packet); i++){
		packet[i] = 0x00;
	}

	/*packet[0] = U2FHID_INIT;
	packet[1] = 17;
	for(int i = 0; i < 8; i++){
		packet[i+2] = data[i];
	}
	packet[10] = (generatedChannels[generatedChannelsCount] >> 24) & 0xFF;
	packet[11] = (generatedChannels[generatedChannelsCount] >> 16) & 0xFF;
	packet[12] = (generatedChannels[generatedChannelsCount] >> 8) & 0xFF;
	packet[13] = generatedChannels[generatedChannelsCount] & 0xFF;
	packet[14] = 2;//test
	packet[15] = 2;
	packet[16] = 1;
	packet[17] = 1;
	packet[18] = 0;//no wink no lock*/
/*
	printf("Will send packet: \r\n");
	for(int i = 0; i < sizeof(packet); i++){
		printf("%02X ", packet[i]);
	}
	printf("\r\n");
	USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, packet, sizeof(packet));
	printf("Have send\r\n");
}
*/
