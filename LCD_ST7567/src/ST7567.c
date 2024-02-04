/*
 * ST7567.c
 *
 *  Created on: Jan 29, 2024
 *      Author: kolya
 */

#include "ST7567.h"
#include "main.h"
#include <string.h>

#if defined(ST7567_CS_GPIO_Port) && defined(ST7567_CS_Pin)
#define ST7567_CS_SELECT() (ST7567_CS_GPIO_Port->BSRR = (ST7567_CS_Pin << 16))
#define ST7567_CS_DESELECT() (ST7567_CS_GPIO_Port->BSRR = ST7567_CS_Pin)
#else
#error "Not defined CS pin"
#endif // defined ST7567_CS

#if defined(ST7567_RST_GPIO_Port) && defined(ST7567_RST_Pin)
#define ST7567_RST_SELECT() (ST7567_RST_GPIO_Port->BSRR = (ST7567_RST_Pin << 16))
#define ST7567_RST_DESELECT() (ST7567_RST_GPIO_Port->BSRR = ST7567_RST_Pin)
#else
#define ST7567_RST_SELECT()
#define ST7567_RST_DESELECT()
#endif // defined ST7567_RST

#if defined(ST7567_A0_GPIO_Port) && defined(ST7567_A0_Pin)
#define ST7567_A0_RESET() (ST7567_A0_GPIO_Port->BSRR = (ST7567_A0_Pin << 16))
#define ST7567_A0_SET() (ST7567_A0_GPIO_Port->BSRR = ST7567_A0_Pin)
#else
#error "Not defined A0 pin"
#endif // defined ST7567_CS

static SPI_HandleTypeDef* ST7567_spi = NULL;

static uint8_t ST7567_GDRAM[ST7567_SCREEN_HEIGHT / 8][ST7567_SCREEN_WIDTH] = {0};
static const uint8_t* ptr_font8x8 = NULL;

void ST7567_writeData(uint8_t * const data, const uint16_t size)
{
	ST7567_A0_SET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, data, size, 1000);
	ST7567_CS_DESELECT();
}

void ST7567_reset()
{
#if defined(ST7567_RST_GPIO_Port) && defined(ST7567_A0_Pin)
	// Hardware reset
	ST7567_RST_SELECT();
	// TODO: delay
	HAL_Delay(10);
	ST7567_RST_DESELECT();
#else
	// Software reset
	uint8_t array[] = {
			ST7567_SoftwareReset,
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();
#endif // defined ST7567_RST
}

void ST7567_init(SPI_HandleTypeDef * const spi)
{
	ST7567_spi = spi;
	//ST7567_reset();

	ST7567_RST_DESELECT();
	HAL_Delay(50);
	ST7567_RST_SELECT();
	HAL_Delay(500);
	ST7567_RST_DESELECT();
	HAL_Delay(10);



	uint8_t array[] = {
			ST7567_BiasSelect | ST7567_Bias_1_7,
			ST7567_SEGDirectionSet | 0,
			ST7567_COMDirectionSet | ST7567_COMDirection_Normal,

			ST7567_SetPowerControl | ST7567_PowerControlBoosterON |
			ST7567_PowerControlRegulatorON | ST7567_PowerControlFollowerON,

			ST7567_SetRegulationRatio | ST7567_RR_6_0,

			ST7567_SetEV,
			0,

			ST7567_SetBooster,
			1,

			ST7567_StartLineSet | 0,
			ST7567_DisplayON,
			ST7567_AllPixelON | 0,
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();

	ST7567_writeDisplayData();
}

void ST7567_powerSave()
{
	uint8_t array[] = {
			ST7567_DisplayOFF,
			ST7567_AllPixelON | 1,
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();
}

void ST7567_wakeUp()
{
	uint8_t array[] = {
			ST7567_DisplayON,
			ST7567_AllPixelON | 0,
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();
}

void ST7567_refresh()
{
	uint8_t array[] = {
			ST7567_NOP,
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();
}

// TODO: update DRAM function
void ST7567_writeDisplayData()
{
	for (int i = 0; i < 8; i++)
	{
		ST7567_setPos(0, i);
		ST7567_writeData(ST7567_GDRAM[i], 128);
	}

	uint8_t array[] = {
			ST7567_DisplayON,
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();

}

void ST7567_setPos(const uint8_t x, const uint8_t y)
{
	uint8_t array[] = {
			ST7567_StartLineSet | 0,
			ST7567_PageAddressSet | y,
			ST7567_SetColumnAddressHigh | ((x >> 4) & 0x0F),
			ST7567_SetColumnAddressLow | (x & 0x0F),
	};

	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, array, sizeof(array), 1000);
	ST7567_CS_DESELECT();
}

void ST7567_writeText(const char * const str, const uint8_t startLine)
{
	if (ptr_font8x8 == NULL)
		return;

	uint16_t textLength = strlen(str);
	for (int i = 0; i < textLength; ++i)
	{
		for (int j = 0; j < 8; j++)
		{
			ST7567_GDRAM[startLine][i * 8 + j] = ptr_font8x8[(str[i] * 8) + j];
		}
	}

	ST7567_writeDisplayData();
}

void ST7567_sendCommand(uint8_t * const command, const uint8_t size)
{
	ST7567_A0_RESET();
	ST7567_CS_SELECT();
	HAL_SPI_Transmit(ST7567_spi, command, size, 1000);
	ST7567_CS_DESELECT();
}

void ST7567_setFont8x8(const uint8_t* const font)
{
	ptr_font8x8 = font;
}

