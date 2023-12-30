/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.c
  * @version        : v2.0_Cube
  * @brief          : USB Device Custom HID interface file.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "usbd_custom_hid_if.h"

/* USER CODE BEGIN INCLUDE */
#include "usart.h"
/* USER CODE END INCLUDE */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/

/* USER CODE BEGIN PV */
/* Private variables ---------------------------------------------------------*/

/* USER CODE END PV */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief Usb device.
  * @{
  */

/** @addtogroup USBD_CUSTOM_HID
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Private_TypesDefinitions USBD_CUSTOM_HID_Private_TypesDefinitions
  * @brief Private types.
  * @{
  */

/* USER CODE BEGIN PRIVATE_TYPES */

/* USER CODE END PRIVATE_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Defines USBD_CUSTOM_HID_Private_Defines
  * @brief Private defines.
  * @{
  */

/* USER CODE BEGIN PRIVATE_DEFINES */

/* USER CODE END PRIVATE_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Macros USBD_CUSTOM_HID_Private_Macros
  * @brief Private macros.
  * @{
  */

/* USER CODE BEGIN PRIVATE_MACRO */

/* USER CODE END PRIVATE_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_Variables USBD_CUSTOM_HID_Private_Variables
  * @brief Private variables.
  * @{
  */

/** Usb HID report descriptor. */
__ALIGN_BEGIN static uint8_t CUSTOM_HID_ReportDesc_FS[USBD_CUSTOM_HID_REPORT_DESC_SIZE] __ALIGN_END =
{
  /* USER CODE BEGIN 0 */
		0x06, 0xd0, 0xf1,// USAGE_PAGE (FIDO Alliance)
		0x09, 0x01,// USAGE (Keyboard)
		0xa1, 0x01,// COLLECTION (Application)

		0x09, 0x20,                   //   USAGE (Input Report Data)
		0x15, 0x00,                   //   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00,             //   LOGICAL_MAXIMUM (255)
		0x75, 0x08,                   //   REPORT_SIZE (8)
		0x95, 0x40,                   //   REPORT_COUNT (64)
		0x81, 0x02,                   //   INPUT (Data,Var,Abs)
		0x09, 0x21,                   //   USAGE(Output Report Data)
		0x15, 0x00,                   //   LOGICAL_MINIMUM (0)
		0x26, 0xff, 0x00,             //   LOGICAL_MAXIMUM (255)
		0x75, 0x08,                   //   REPORT_SIZE (8)
		0x95, 0x40,                   //   REPORT_COUNT (64)
		0x91, 0x02,                   //   OUTPUT (Data,Var,Abs)
  /* USER CODE END 0 */
  0xC0    /*     END_COLLECTION	             */
};

/* USER CODE BEGIN PRIVATE_VARIABLES */
static uint8_t* cashe;

static HidStruct hidStruct = {.ChannelId = {0, 0, 0, 0}, .command = U2FHID_NONE, .finishedPacketSequence = true, .expectedSize = 0, .remainingSize = 0};
static uint32_t generatedChannels[] = {0x10111213, 0x01020304};//[10];//to change
static int generatedChannelsCount = 0;
/* USER CODE END PRIVATE_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */
extern USBD_HandleTypeDef hUsbDeviceFS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */
/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Private_FunctionPrototypes USBD_CUSTOM_HID_Private_FunctionPrototypes
  * @brief Private functions declaration.
  * @{
  */

static int8_t CUSTOM_HID_Init_FS(void);
static int8_t CUSTOM_HID_DeInit_FS(void);
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* recive);

/**
  * @}
  */

USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS =
{
  CUSTOM_HID_ReportDesc_FS,
  CUSTOM_HID_Init_FS,
  CUSTOM_HID_DeInit_FS,
  CUSTOM_HID_OutEvent_FS
};

/** @defgroup USBD_CUSTOM_HID_Private_Functions USBD_CUSTOM_HID_Private_Functions
  * @brief Private functions.
  * @{
  */

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Initializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_Init_FS(void)
{
  /* USER CODE BEGIN 4 */
	printf("started\r\n");
  return (USBD_OK);
  /* USER CODE END 4 */
}

/**
  * @brief  DeInitializes the CUSTOM HID media low layer
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_DeInit_FS(void)
{
  /* USER CODE BEGIN 5 */
  return (USBD_OK);
  /* USER CODE END 5 */
}

/**
  * @brief  Manage the CUSTOM HID class events
  * @param  event_idx: Event index
  * @param  state: Event state
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
static int8_t CUSTOM_HID_OutEvent_FS(uint8_t* recive)
{
  /* USER CODE BEGIN 6 */
	uint8_t parced[0x40];
	bool will_generate_new_alloc = false;

	//Print_Packet();
	if(hidStruct.finishedPacketSequence){
		will_generate_new_alloc = true;
	}

	size_t parced_size = Parce_Hid_Packet(recive, parced);
	if(will_generate_new_alloc){
		cashe = malloc((hidStruct.expectedSize) * sizeof(uint8_t));
	}
	memcpy(cashe + (hidStruct.expectedSize - hidStruct.remainingSize - parced_size), parced, parced_size*sizeof(uint8_t));

	if(hidStruct.finishedPacketSequence){
		// make operations
		uint8_t* response_data;//size to be changed
		size_t response_data_size = 0;
		switch(hidStruct.command){
		case U2FHID_INIT:
			response_data = malloc(17 * sizeof(uint8_t));
			response_data_size = Handle_Init(cashe, hidStruct.expectedSize, response_data);
			printf("prepared init\r\n");
			break;
		case U2FHID_PING:
		case U2FHID_MSG:
			//if(sizeof(cashe) < 5){
			//	printf("HID MSG had not enought data (header) found %d bytes\r\n", );
			//	break;
			//}
			switch(cashe[1]){
			case U2FISO7816_VERSION:
				response_data = malloc(64 * 20 * sizeof(uint8_t));//to be changed
				response_data_size = 64*20;//to be changed
				break;
			case U2FISO7816_AUTHENTICATE:
				response_data = malloc(64 * 20 * sizeof(uint8_t));//to be changed
				response_data_size = 64*20;//to be changed
				break;
			case U2FISO7816_REGISTER:
				response_data = malloc(64 * 20 * sizeof(uint8_t));//to be changed
				response_data_size = 64*20;//to be changed
				break;
			}

			if(hidStruct.command == U2FHID_PING){
				//implementation of iso 7816 adds 5 bytes at the beginning and 2 bytes response code and 1 for number of packets response was split to
				response_data = malloc((hidStruct.expectedSize + 8)* sizeof(uint8_t));
				response_data_size = hidStruct.expectedSize + 8;
				//adding code for service cache contains only data without code (in case of ping)
				uint8_t *new_cashe = malloc((hidStruct.expectedSize + 6) * sizeof(uint8_t));//could be changed to realloc to omit memcpy
				new_cashe[0] = 0x00;
				new_cashe[1] = 0x05;
				new_cashe[2] = 0x00;
				new_cashe[3] = 0x00;
				new_cashe[4] = 0x00;
				new_cashe[5] = 0x00;//00 05 00 00 00 00
				memcpy(new_cashe + 6, cashe, hidStruct.expectedSize * sizeof(uint8_t));
				hidStruct.expectedSize += 6;
				free(cashe);
				cashe = new_cashe;

			}
			if(response_data_size == 0){
				break;
			}
			//cannot be extracted to function i2c receive breaks stm32(data becomes corrupted)
			int number_of_packets = How_Many_Frames_Are_Needed_NFC(hidStruct.expectedSize);
			//check if multiple sends are needed if no this code V if yes for with frames while until received correctly then return
			if(number_of_packets == 1){
				uint8_t* send_data = malloc((hidStruct.expectedSize + 12) * sizeof(uint8_t) + sizeof(AID));
				uint8_t uid[7];
				uint8_t communications_failed_attemts = 0;
				size_t send_data_size = Make_Packet_To_Send_NFC(cashe, hidStruct.expectedSize, send_data);
				while(communications_failed_attemts < NFC_SEND_RETRIES){
					int uidSize = Read_Passive_Target(uid);
					if (uidSize != 0){
						size_t tmp_response_data_size = In_Data_Exchange(send_data, send_data_size, response_data, response_data_size);

						if(tmp_response_data_size != 0 /*&& tmp_response_data_size >= 9*/){
							//response_data_size = tmp_response_data_size - 3;//removing 10 00 ff added by PN532
							//memmove(response_data, response_data + 9, response_data_size * sizeof(uint8_t));
							//add code checking
							//response_data_size = tmp_response_data_size;
							for(int i = 0; i < response_data_size; i++){
								printf("%02x ", response_data[i]);
							}
							printf("\r\n");
							break;
						}
						printf("something went wrong will request re send\r\n");
						for(int i = 0; i < 30; i++){
							printf("%02x ", response_data[i]);
						}
						printf("\r\n");
						communications_failed_attemts++;
						//change data to request just re-send if that was communication error (not NFC code)
					}
					printf("mobile not found\r\n");
				}
			} else {
				uint8_t* send_data = malloc((MAX_DATA_PER_PN532_FRAME + 12 + 5) * sizeof(uint8_t) + sizeof(AID));
				uint8_t uid[7];
				for(int i = 0; i < number_of_packets; i++){
					bool finished_sucessfully = false;
					uint8_t communications_failed_attemts = 0;
					size_t send_data_size = Splice_And_Make_Packet_To_Send_NFC(cashe, hidStruct.expectedSize, send_data, i);
					while(communications_failed_attemts < NFC_SEND_RETRIES && !finished_sucessfully){
						int uidSize = Read_Passive_Target(uid);
						if (uidSize != 0){
							printf("phone found\r\n");
							size_t tmp_response_data_size = In_Data_Exchange(send_data, send_data_size, response_data, response_data_size);
							// data was parced in data_exchange fun
							if(tmp_response_data_size != 0){
								//for(int j = 0; j < tmp_response_data_size; j++){
								//	printf("%02x ", response_data[j]);
								//}
								//printf("\r\n");
								printf("packet number %d out of %d received correctly\r\n", i , number_of_packets);
								response_data_size = tmp_response_data_size;
								finished_sucessfully = true;
								continue;
							}
							printf("something went wrong response code seems to be inccorect\r\n");
							//moved inside in data exchange so probably obsolete V
							communications_failed_attemts++;
						} else {
							printf("mobile not found\r\n");
						}
					}
				}
			}
			printf("prepared msg\r\n");
			break;
		/*case U2FHID_PING:
			response_data = malloc(hidStruct.expectedSize * sizeof(uint8_t));
			response_data_size = hidStruct.expectedSize;
			response_data = cashe;
			printf("prepared ping\r\n");
			break;*/
		default:
			//take care of exception
			break;
		}
		//cannot be extracted to function SendReport breaks stm32
		uint8_t giga_packet[64*25];//bad to change
		size_t giga_packet_size = 0;
		if(response_data_size <= 57){
			uint8_t packet[64];
			Make_Packet_To_Send(response_data, response_data_size, packet, sizeof(packet), hidStruct.command, 0, response_data_size);
			USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, packet, sizeof(packet));
		} else {
			size_t remaining_size = response_data_size;
			size_t send_already_size = 0;
			uint8_t packet[64];
			Make_Packet_To_Send(response_data, 57, packet, sizeof(packet), hidStruct.command, 0, response_data_size);
			memcpy(giga_packet, packet, sizeof(packet));
			giga_packet_size += 64;
			remaining_size -= 57;
			send_already_size += 57;

			for(int i = 0; remaining_size > 0; i++){
				size_t take_data_size = (remaining_size > 59)?59:remaining_size;
				Make_Packet_To_Send(response_data + send_already_size, take_data_size, packet, sizeof(packet), hidStruct.command, i, 0);
				memcpy(giga_packet + 64*(i+1), packet, sizeof(packet));
				giga_packet_size += 64;
				remaining_size -= take_data_size;
				send_already_size += take_data_size;
			}
		}
		//printf("will send:\r\n");
		//for(int i = 0; i < giga_packet_size; i++){
		//	printf("%02X ", giga_packet[i]);
		//}
		//printf("\r\n");
		USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, giga_packet, giga_packet_size * sizeof(uint8_t));
		free(response_data);
		free(cashe);
	}

	/* Start next USB packet transfer once data processing is completed */
	USBD_CUSTOM_HID_ReceivePacket(&hUsbDeviceFS);

	return (USBD_OK);
  /* USER CODE END 6 */
}

/* USER CODE BEGIN 7 */
/**
  * @brief  Send the report to the Host
  * @param  report: The report to be sent
  * @param  len: The report length
  * @retval USBD_OK if all operations are OK else USBD_FAIL
  */
/*
static int8_t USBD_CUSTOM_HID_SendReport_FS(uint8_t* report, size_t len)
{
  return USBD_CUSTOM_HID_SendReport(&hUsbDeviceFS, report, len);
}
*/
/* USER CODE END 7 */

/* USER CODE BEGIN PRIVATE_FUNCTIONS_IMPLEMENTATION */
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
	if(wholeSequenceSize > 0){//first packet (init packet)
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

size_t Handle_Init(uint8_t* data, size_t data_size, uint8_t* response){
	memcpy(response, data, 8 * sizeof(uint8_t));
	response[8] = (generatedChannels[generatedChannelsCount] >> 24) & 0xFF;
	response[9] = (generatedChannels[generatedChannelsCount] >> 16) & 0xFF;
	response[10] = (generatedChannels[generatedChannelsCount] >> 8) & 0xFF;
	response[11] = generatedChannels[generatedChannelsCount] & 0xFF;
	response[12] = 2;//test change this parameters to mean something
	response[13] = 2;
	response[14] = 1;
	response[15] = 1;
	response[16] = 0;//no wink no lock*/

	return 17;
}

size_t Make_Packet_To_Send_NFC(uint8_t* data, size_t data_size, uint8_t* response_data){
	size_t AID_length = sizeof(AID)/sizeof(uint8_t);

	response_data[0] = 0x01;
	response_data[1] = 0x00;
	response_data[2] = 0xA4;
	response_data[3] = 0x04;
	response_data[4] = 0x00;
	response_data[5] = AID_length;
	memcpy(response_data + 6, AID, sizeof(AID));
	response_data[6 + AID_length] = hidStruct.ChannelId[0];
	response_data[6 + AID_length + 1] = hidStruct.ChannelId[1];
	response_data[6 + AID_length + 2] = hidStruct.ChannelId[2];
	response_data[6 + AID_length + 3] = hidStruct.ChannelId[3];
	response_data[6 + AID_length + 4] = 0x00;//0x01?
	memcpy(response_data + 6 + 5 + AID_length, data, data_size * sizeof(uint8_t));
	response_data[6 + 5 + AID_length + data_size] = 0x00;

	return data_size + 12 + AID_length;
}

size_t Splice_And_Make_Packet_To_Send_NFC(uint8_t* data, size_t data_size, uint8_t* response_data, uint8_t packet_number){
	size_t AID_length = sizeof(AID)/sizeof(uint8_t);
	size_t included_data_size = (MAX_DATA_PER_PN532_FRAME < (data_size - packet_number * MAX_DATA_PER_PN532_FRAME)) ? MAX_DATA_PER_PN532_FRAME : data_size - packet_number * MAX_DATA_PER_PN532_FRAME;
	size_t offset = 0;

	response_data[0] = 0x01;
	response_data[1] = 0x00;
	response_data[2] = 0xA4;
	response_data[3] = 0x04;
	response_data[4] = 0x00;
	response_data[5] = AID_length;
	memcpy(response_data + 6, AID, sizeof(AID));
	response_data[6 + AID_length] = hidStruct.ChannelId[0];
	response_data[6 + AID_length + 1] = hidStruct.ChannelId[1];
	response_data[6 + AID_length + 2] = hidStruct.ChannelId[2];
	response_data[6 + AID_length + 3] = hidStruct.ChannelId[3];
	response_data[6 + AID_length + 4] = (How_Many_Frames_Are_Needed_NFC(data_size) - packet_number) & 0xFF;
	if(packet_number != 0){
		memcpy(response_data + 6 + 5 + AID_length, data, 6 * sizeof(uint8_t));
		offset = 6;
	}
	memcpy(response_data + 6 + 5 + offset + AID_length, data + packet_number * MAX_DATA_PER_PN532_FRAME, included_data_size * sizeof(uint8_t));
	response_data[6 + 5 + offset + AID_length + included_data_size] = 0x00;

	return 12 + offset + AID_length + included_data_size;
}

int How_Many_Frames_Are_Needed_NFC(size_t data_size){
	data_size -= 5;
	if(data_size <= 0){
		printf("error during checking number of required packets\r\n");
		return 1;
	}
	int count = data_size / MAX_DATA_PER_PN532_FRAME;
	count = data_size % MAX_DATA_PER_PN532_FRAME != 0 ? count + 1 : count;
	return count;
}



/* USER CODE END PRIVATE_FUNCTIONS_IMPLEMENTATION */
/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

