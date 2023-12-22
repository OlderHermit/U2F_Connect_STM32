/*
 * pn532-i2c.h
 *
 *  Created on: Nov 3, 2023
 *      Author: Author: Zdzisław Jaxa-Małachowski
 */

#ifndef INC_PN532_I2C_H_
#define INC_PN532_I2C_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "stm32l4xx_hal.h"

//#define PRINT_DEBUG

#define PN532_PREAMBLE                      (0x00)
#define PN532_STARTCODE1                    (0x00)
#define PN532_STARTCODE2                    (0xFF)
#define PN532_POSTAMBLE                     (0x00)

#define PN532_HOST_CODE                   	(0xD4)

// PN532 Commands
#define PN532_COMMAND_DIAGNOSE              (0x00)
#define PN532_COMMAND_GETFIRMWAREVERSION    (0x02)
#define PN532_COMMAND_GETGENERALSTATUS      (0x04)
#define PN532_COMMAND_READREGISTER          (0x06)
#define PN532_COMMAND_WRITEREGISTER         (0x08)
#define PN532_COMMAND_READGPIO              (0x0C)
#define PN532_COMMAND_WRITEGPIO             (0x0E)
#define PN532_COMMAND_SETSERIALBAUDRATE     (0x10)
#define PN532_COMMAND_SETPARAMETERS         (0x12)
#define PN532_COMMAND_SAMCONFIGURATION      (0x14)
#define PN532_COMMAND_POWERDOWN             (0x16)
#define PN532_COMMAND_RFCONFIGURATION       (0x32)
#define PN532_COMMAND_RFREGULATIONTEST      (0x58)
#define PN532_COMMAND_INJUMPFORDEP          (0x56)
#define PN532_COMMAND_INJUMPFORPSL          (0x46)
#define PN532_COMMAND_INLISTPASSIVETARGET   (0x4A)
#define PN532_COMMAND_INATR                 (0x50)
#define PN532_COMMAND_INPSL                 (0x4E)
#define PN532_COMMAND_INDATAEXCHANGE        (0x40)
#define PN532_COMMAND_INCOMMUNICATETHRU     (0x42)
#define PN532_COMMAND_INDESELECT            (0x44)
#define PN532_COMMAND_INRELEASE             (0x52)
#define PN532_COMMAND_INSELECT              (0x54)
#define PN532_COMMAND_INAUTOPOLL            (0x60)
#define PN532_COMMAND_TGINITASTARGET        (0x8C)
#define PN532_COMMAND_TGSETGENERALBYTES     (0x92)
#define PN532_COMMAND_TGGETDATA             (0x86)
#define PN532_COMMAND_TGSETDATA             (0x8E)
#define PN532_COMMAND_TGSETMETADATA         (0x94)
#define PN532_COMMAND_TGGETINITIATORCOMMAND (0x88)
#define PN532_COMMAND_TGRESPONSETOINITIATOR (0x90)
#define PN532_COMMAND_TGGETTARGETSTATUS     (0x8A)

#define PN532_WAKEUP                        (0x55)

#define PN532_I2C_ADDRESS                   (0x24 << 1)
#define PN532_I2C_READBIT                   (0x01)
#define PN532_I2C_BUSY                      (0x00)
#define PN532_I2C_READY                     (0x01)
#define PN532_I2C_READYTIMEOUT              (20)

#define PN532_MIFARE_ISO14443A              (0x00)

// Mifare Commands
#define MIFARE_CMD_AUTH_A                   (0x60)
#define MIFARE_CMD_AUTH_B                   (0x61)
#define MIFARE_CMD_READ                     (0x30)
#define MIFARE_CMD_WRITE                    (0xA0)
#define MIFARE_CMD_TRANSFER                 (0xB0)
#define MIFARE_CMD_DECREMENT                (0xC0)
#define MIFARE_CMD_INCREMENT                (0xC1)
#define MIFARE_CMD_RESTORE                  (0xC2)

#define PN532_FRAME_MAX_LENGTH              255
#define PN532_MODE_NORMAL					(0x01)
#define PN532_MODE_VIRTUAL					(0x02)

#define NFC_ID_0							(0x01)
#define NFC_ID_1							(0x02)
#define NFC_ID_2							(0x03)
#define NFC_ID_3							(0x04)
#define NFC_ID_4							(0x05)
#define NFC_ID_5							(0x06)
#define NFC_ID_6							(0x07)
#define NFC_ID_7							(0x08)
#define NFC_ID_8							(0x09)
#define NFC_ID_9							(0x10)

void Set_I2C(I2C_HandleTypeDef i2c);

size_t Make_Frame_For_Send(uint8_t* data, size_t size, uint8_t* output, size_t output_size);

size_t Make_Frame_For_Send_Big(uint8_t* data, size_t size, uint8_t* output, size_t output_size);

int Read_Frame(uint8_t* frame, size_t size);

int Read_Frame_Awaiting(uint8_t* frame, size_t size);

void Set_SAM_Configuration(uint8_t);

void Get_Firmware_Version(void);

int Read_Passive_Target(uint8_t* uid);

int In_Data_Exchange(uint8_t* data, size_t size, uint8_t* response, size_t response_size);

int Mifare_Auth(uint8_t* uid, size_t uid_size, int block_number);

int Mifare_Read(uint8_t* response, size_t size, int block_number);

int Mifare_Write(uint8_t* response, size_t size, int block_number, uint8_t* data, size_t data_size);

int ISO1443_4_Read(uint8_t* response, size_t size);

void Tg_Init_As_Target(void);

void In_Jump_For_Dep(void);

void Set_Parameters(uint8_t flags);






#endif /* INC_PN532_I2C_H_ */
