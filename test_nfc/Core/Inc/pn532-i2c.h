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
#include <stdbool.h>
#include <string.h>
#include "fido_u2f_hid.h"
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

#define PN532_I2C_ADDRESS                   (0x24 << 1)
#define PN532_I2C_READBIT                   (0x01)
#define PN532_I2C_BUSY                      (0x00)
#define PN532_I2C_READY                     (0x01)
#define PN532_I2C_READYTIMEOUT              (20)

#define PN532_FRAME_MAX_LENGTH				255
#define PN532_MODE_NORMAL					(0x01)
#define PN532_MODE_VIRTUAL					(0x02)
/**
 * @brief Set I2C handle for library to work correctly. (could be omitted with extern)
 * @param I2C handle defined in main.
 */
void Set_I2C(I2C_HandleTypeDef i2c);

/**
 * @brief Create a frame to send data to PN532 module.
 * @param data Pointer to the data.
 * @param size Size of the data.
 * @param output Pointer to the output buffer.
 * @param output_size Size of the output buffer.
 * @return The size of the created frame.
 */
size_t Make_Frame_For_Send(uint8_t* data, size_t size, uint8_t* output, size_t output_size);

/**
 * @DEPRICATED
 * @brief Create a larger frame to send data to PN532 module.
 * @param data Pointer to the data.
 * @param size Size of the data.
 * @param output Pointer to the output buffer.
 * @param output_size Size of the output buffer.
 * @return The size of the created frame.
 */
size_t Make_Frame_For_Send_Big(uint8_t* data, size_t size, uint8_t* output, size_t output_size);

/**
 * @brief Create a frame with request to "continue" communication with Android Application.
 * @param packet_index Index of requested response packet.
 * @param frameToSend_rest Pointer to the output buffer.
 * @param frameToSend_rest_size Size of the output buffer.
 */
void Make_Frame_For_Send_Rest(uint8_t packet_index, uint8_t* frameToSend_rest, size_t frameToSend_rest_size);

/**
 * @brief Wait X ms and then try to read response frame.
 * @param frame Pointer to the output buffer.
 * @param size Size of the output buffer.
 * @return An integer representing the success or failure of the operation. 0 - success, 1 - failure
 */
int Read_Frame(uint8_t* frame, size_t size);

/**
 * @brief Try reading response frame, if there is no response repeat this reading after waiting X ms.
 * @param frame Pointer to the output buffer.
 * @param size Size of the output buffer.
 * @return An integer representing the success or failure of the operation. 0 - success, 1 - failure
 */
int Read_Frame_Awaiting(uint8_t* frame, size_t size);

/**
 * @brief Set SAM configuration.
 * @param config Configuration value check defined values.
 */
void Set_SAM_Configuration(uint8_t config);

/**
 * @brief Get firmware version and print it to console.
 */
void Get_Firmware_Version(void);

/**
 * @brief Look for passive targets.
 * @param uid Pointer to the UID buffer.
 * @return Size of data copied to the UID buffer
 */
int Read_Passive_Target(uint8_t* uid);

/**
 * @brief Send request to Android Application via HCE.
 * @param data Pointer to the data.
 * @param size Size of the data.
 * @param response Pointer to the output buffer.
 * @param response_size Size of the output buffer.
 * @return Size of data copied to the output buffer
 */
int In_Data_Exchange(uint8_t* data, size_t size, uint8_t* response, size_t response_size);

/**
 * @brief Check checksum constrains of packet (length, data).
 * @param data Pointer to the packet.
 * @return An bool representing the success or failure of the operation.
 */
bool Is_Checksum_Correct(uint8_t* data);


#endif /* INC_PN532_I2C_H_ */
