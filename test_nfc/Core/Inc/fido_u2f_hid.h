/*
 * fido_u2f_hid.h
 *
 *  Created on: Dec 1, 2023
 *      Author: Zdzisław Jaxa-Małachowski
 */
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#ifndef INC_FIDO_U2F_HID_H_
#define INC_FIDO_U2F_HID_H_

#define TYPE_INIT 			0x80
//#define U2FHID_VENDOR_FIRST (TYPE_INIT | 0x40)  // First vendor defined command
//#define U2FHID_VENDOR_LAST  (TYPE_INIT | 0x7f)  // Last vendor defined command

//used for data inside MSG hid frame
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
//non hid part to extracted somewhere else
static uint8_t AID[] = {0xF0, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06};
static uint8_t NFC_SEND_RETRIES = 5;
static size_t MAX_DATA_PER_PN532_FRAME = 200;
/*
size_t Parce_Hid_Packet(uint8_t* data, uint8_t* output);
HidStruct* Get_Hid_Struct(void);
//extends size by 7 (init) or 5 (all others) so it expects correct inputs of 57 or 59 respectively if wholeSequenceSize != 0 then packet will be treated as init
size_t Make_Packet_To_Send(uint8_t* data, size_t data_size, uint8_t* output, size_t output_size, uint8_t command, uint8_t sequnce, int wholeSequenceSize);
void Splice_Data_And_Send(uint8_t* data, size_t data_size, uint8_t command, USBD_HandleTypeDef* hUsbDeviceFS);
void Handle_Init(uint8_t* data, size_t data_size, USBD_HandleTypeDef* hUsbDeviceFS);
*/

#endif /* INC_FIDO_U2F_HID_H_ */
