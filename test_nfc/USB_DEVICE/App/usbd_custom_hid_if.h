/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : usbd_custom_hid_if.h
  * @version        : v2.0_Cube
  * @brief          : Header for usbd_custom_hid_if.c file.
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

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USBD_CUSTOM_HID_IF_H__
#define __USBD_CUSTOM_HID_IF_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "usbd_customhid.h"
#include "fido_u2f_hid.h"

/* USER CODE BEGIN INCLUDE */
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <string.h>
#include <stdio.h>
/* USER CODE END INCLUDE */

/** @addtogroup STM32_USB_OTG_DEVICE_LIBRARY
  * @brief For Usb device.
  * @{
  */

/** @defgroup USBD_CUSTOM_HID USBD_CUSTOM_HID
  * @brief Usb custom human interface device module.
  * @{
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Defines USBD_CUSTOM_HID_Exported_Defines
  * @brief Defines.
  * @{
  */

/* USER CODE BEGIN EXPORTED_DEFINES */
//#define TYPE_INIT 			0x80
//#define U2FHID_VENDOR_FIRST (TYPE_INIT | 0x40)  // First vendor defined command
//#define U2FHID_VENDOR_LAST  (TYPE_INIT | 0x7f)  // Last vendor defined command

//used for data inside MSG hid frame
 /*
enum U2FISO7816_Command{
	U2FISO7816_REGISTER = 0x01,
	U2FISO7816_AUTHENTICATE = 0x02,
	U2FISO7816_VERSION = 0x03,
};

//used for hid communication frame

enum U2FHID_Command
{
	U2FHID_NONE = 0x00,
	U2FHID_PING = (TYPE_INIT | 0x01),
	U2FHID_MSG = (TYPE_INIT | 0x03),
	U2FHID_LOCK = (TYPE_INIT | 0x04),
	U2FHID_INIT = (TYPE_INIT | 0x06),
	U2FHID_WINK = (TYPE_INIT | 0x08),
	U2FHID_SYNC = (TYPE_INIT | 0x3c),
	U2FHID_ERROR = (TYPE_INIT | 0x3f),
};

typedef struct _HidStruct{
	uint8_t  ChannelId[4];
	enum U2FHID_Command command;
	bool finishedPacketSequence;
	int expectedSize;
	int remainingSize;
} HidStruct;

static uint8_t AID[] = {0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
static uint8_t NFC_SEND_RETRIES = 5;*/
/* USER CODE END EXPORTED_DEFINES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Types USBD_CUSTOM_HID_Exported_Types
  * @brief Types.
  * @{
  */

/* USER CODE BEGIN EXPORTED_TYPES */

/* USER CODE END EXPORTED_TYPES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Macros USBD_CUSTOM_HID_Exported_Macros
  * @brief Aliases.
  * @{
  */

/* USER CODE BEGIN EXPORTED_MACRO */

/* USER CODE END EXPORTED_MACRO */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_Variables USBD_CUSTOM_HID_Exported_Variables
  * @brief Public variables.
  * @{
  */

/** CUSTOMHID Interface callback. */
extern USBD_CUSTOM_HID_ItfTypeDef USBD_CustomHID_fops_FS;

/* USER CODE BEGIN EXPORTED_VARIABLES */

/* USER CODE END EXPORTED_VARIABLES */

/**
  * @}
  */

/** @defgroup USBD_CUSTOM_HID_Exported_FunctionsPrototype USBD_CUSTOM_HID_Exported_FunctionsPrototype
  * @brief Public functions declaration.
  * @{
  */

/* USER CODE BEGIN EXPORTED_FUNCTIONS */

size_t Parce_Hid_Packet(uint8_t* data, uint8_t* output);
//extends size by 7 (init) or 5 (all others) so it expects correct inputs of 57 or 59 respectively if wholeSequenceSize != 0 then packet will be treated as init
size_t Make_Packet_To_Send(uint8_t* data, size_t data_size, uint8_t* output, size_t output_size, uint8_t command, uint8_t sequnce, int wholeSequenceSize);
void Splice_Data_And_Send(uint8_t* data, size_t data_size, uint8_t command, USBD_HandleTypeDef* hUsbDeviceFS);
size_t Handle_Init(uint8_t* data, size_t data_size, uint8_t* response);
size_t Make_Packet_To_Send_NFC(uint8_t* data, size_t data_size, uint8_t* response_data);

/* USER CODE END EXPORTED_FUNCTIONS */

/**
  * @}
  */

/**
  * @}
  */

/**
  * @}
  */

#ifdef __cplusplus
}
#endif

#endif /* __USBD_CUSTOM_HID_IF_H__ */

