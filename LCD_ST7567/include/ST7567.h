/*
 * ST7567.h
 *
 *  Created on: Jan 29, 2024
 *      Author: kolya
 */

#ifndef INC_ST7567_H_
#define INC_ST7567_H_

#include "main.h"

#define ST7567_MAX_HEIGHT_BITS 65
#define ST7567_MAX_WIDTH_BITS 132

#define ST7567_SCREEN_WIDTH 128
#define ST7567_SCREEN_HEIGHT 64

typedef enum
{
	ST7567_DisplayON = 0xAF,
	ST7567_DisplayOFF = 0xAE,

	/*
	 * Using send(ST7567_StartLineSet | startLine)
	 */
	ST7567_StartLineSet = 0x40,

	/*
	 * Using send(ST7567_PageAddressSet | page)
	 */
	ST7567_PageAddressSet = 0xB0,

	/*
	 * Using:
	 * send(ST7567_SetColumnAddressHigh | ((address >> 4) & 0x0F))
	 * send(ST7567_SetColumnAddressLow | (address & 0x0F))
	 */
	ST7567_SetColumnAddressHigh = 0x10,
	ST7567_SetColumnAddressLow = 0x00,

	/*
	 * Using: send(ST7567_SEGDirectionSet | 0 or 1)
	 */
	ST7567_SEGDirectionSet = 0xA0,

	/*
	 * Using: send(ST7567_InverseDisplaySet | 0 or 1)
	 */
	ST7567_InverseDisplaySet = 0xA6,

	/*
	 * Using: send(ST7567_AllPixelON | 0 or 1)
	 */
	ST7567_AllPixelON = 0xA4,

	/*
	 * See: ST7567_BiasSetting
	 * Using: send(ST7567_BiasSelect | bias)
	 */
	ST7567_BiasSelect = 0xA2,

	/*
	 * TODO: check it
	 * Using: send(ST7567_ColumnAddressIncrement)
	 */
	ST7567_ColumnAddressIncrement = 0xE0,

	/*
	 * TODO: check it
	 * Using: send(ST7567_EndColumnAddressIncrement)
	 */
	ST7567_EndColumnAddressIncrement = 0xEE,

	/*
	 * Using: send(ST7567_SoftwareReset)
	 */
	ST7567_SoftwareReset = 0xE2,

	/*
	 * See: ST7567_COMDirection
	 * Using: send(ST7567_COMDirectionSet | direction)
	 */
	ST7567_COMDirectionSet = 0xC0,

	/*
	 * See: ST7567_PowerControl
	 * Using: send(ST7567_PowerControl | booster | regulator | follower)
	 */
	ST7567_SetPowerControl = 0x28,

	/*
	 * See: ST7567_RegulationRatio
	 * Using: send(ST7567_SetRegulationRatio | rr)
	 */
	ST7567_SetRegulationRatio = 0x20,

	/*
	 * Using:
	 * send(ST7567_SetEV)
	 * send(EV)
	 */
	ST7567_SetEV = 0x81,

	/*
	 * See: ST7567_Booster
	 * Using:
	 * send(ST7567_SetBooster)
	 * send(booster)
	 */
	ST7567_SetBooster = 0xF1,

	/*
	 * Using: send(ST7567_NOP)
	 */
	ST7567_NOP = 0xE3,
} ST7567_Command;

typedef enum
{
	ST7567_Booster_x4 = 0x00,
	ST7567_Booster_x5 = 0x01,
} ST7567_Booster;

typedef enum
{
	ST7567_RR_3_0 = 0x00,
	ST7567_RR_3_5 = 0x01,
	ST7567_RR_4_0 = 0x02,
	ST7567_RR_4_5 = 0x03,
	ST7567_RR_5_0 = 0x04,
	ST7567_RR_5_5 = 0x05,
	ST7567_RR_6_0 = 0x06,
	ST7567_RR_6_5 = 0x07,
} ST7567_RegulationRatio;

typedef enum
{
	ST7567_PowerControlBoosterOFF = 0x00,
	ST7567_PowerControlBoosterON = 0x04,

	ST7567_PowerControlRegulatorOFF = 0x00,
	ST7567_PowerControlRegulatorON = 0x02,

	ST7567_PowerControlFollowerOFF = 0x00,
	ST7567_PowerControlFollowerON = 0x01,
} ST7567_PowerControl;

typedef enum
{
	ST7567_Bias_1_9 = 0x00,
	ST7567_Bias_1_7 = 0x01,
} ST7567_BiasSetting;

typedef enum
{
	ST7567_COMDirection_Normal = 0x00,
	ST7567_COMDirection_Reverse = 0x08,
} ST7567_COMDirection;

void ST7567_init(SPI_HandleTypeDef * const spi);
void ST7567_reset();

void ST7567_powerSave();
void ST7567_wakeUp();

void ST7567_refresh();

void ST7567_writeDisplayData();

void ST7567_writeData(uint8_t * const data, const uint16_t size);
void ST7567_setPos(const uint8_t x, const uint8_t y);

void ST7567_writeText(const char * const str, const uint8_t startLine);

void ST7567_setFont8x8(const uint8_t* const font);

void ST7567_sendCommand(uint8_t * const command, const uint8_t size);

#endif /* INC_ST7567_H_ */
