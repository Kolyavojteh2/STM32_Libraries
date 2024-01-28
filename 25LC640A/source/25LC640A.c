/*
 * 25LC640A.c
 *
 *  Created on: Jan 25, 2024
 *      Author: Kolya
 */

#include "25LC640A.h"
#include <string.h>

#ifndef MIN_INTEGER
#define MIN_INTEGER(a, b) (a < b ? a : b)
#endif // MIN_INTEGER(a, b)

#if defined(_25LC640A_CS_PIN) && defined(_25LC640A_CS_PORT)
#define _25LC640A_CS_SELECT() (_25LC640A_CS_PORT->BSRR = (_25LC640A_CS_PIN << 16))
#define _25LC640A_CS_DESELECT() (_25LC640A_CS_PORT->BSRR = _25LC640A_CS_PIN)
#else
#define _25LC640A_CS_SELECT()
#define _25LC640A_CS_DESELECT()
#endif // defined 25LC640A_CS

#if defined(_25LC640A_HOLD_PIN) && defined(_25LC640A_HOLD_PORT)
#define _25LC640A_HOLD_ENABLE() (_25LC640A_HOLD_PORT->BSRR = (_25LC640A_HOLD_PIN << 16))
#define _25LC640A_HOLD_DISABLE() (_25LC640A_HOLD_PORT->BSRR = _25LC640A_HOLD_PIN)
#else
#define _25LC640A_HOLD_ENABLE()
#define _25LC640A_HOLD_DISABLE()
#endif // defined 25LC640A_HOLD

#if defined(_25LC640A_WP_PIN) && defined(_25LC640A_WP_PORT)
#define _25LC640A_WP_ENABLE() (_25LC640A_WP_PORT->BSRR = (_25LC640A_WP_PIN << 16))
#define _25LC640A_WP_DISABLE() (_25LC640A_WP_PORT->BSRR = _25LC640A_WP_PIN)
#else
#define _25LC640A_WP_ENABLE()
#define _25LC640A_WP_DISABLE()
#endif // defined 25LC640A_WP

static SPI_HandleTypeDef *_25LC640A_spi_handler = NULL;

void _25LC640A_init(SPI_HandleTypeDef * const spi)
{
	_25LC640A_spi_handler = spi;

	_25LC640A_HOLD_DISABLE();
	//_25LC640A_HOLD_ENABLE();
	_25LC640A_WP_DISABLE();
	//_25LC640A_WP_ENABLE();
}

void _25LC640A_read(const uint16_t startAddress, uint8_t * const buffer, const uint16_t size)
{
	uint8_t commonBuffer[3 + size];
	commonBuffer[0] = _25LC640A_READ;
	commonBuffer[1] = ((startAddress >> 8) & 0xFF);
	commonBuffer[2] = (startAddress & 0xFF);

	_25LC640A_CS_SELECT();
	HAL_SPI_TransmitReceive(_25LC640A_spi_handler, commonBuffer, commonBuffer, sizeof(commonBuffer), 1000);
	_25LC640A_CS_DESELECT();

	memcpy(buffer, commonBuffer + 3, size);
}

void _25LC640A_writeByte(const uint16_t startAddress, const uint8_t value)
{
	static uint8_t buffer[] = { _25LC640A_WRITE, };
	buffer[1] = ((startAddress >> 8) & 0xFF);
	buffer[2] = (startAddress & 0xFF);
	buffer[3] = value;

	_25LC640A_writeEnable();

	_25LC640A_CS_SELECT();
	HAL_SPI_Transmit(_25LC640A_spi_handler, buffer, sizeof(buffer),	1000);
	_25LC640A_CS_DESELECT();
}

void _25LC640A_write(const uint16_t startAddress, const uint8_t * const data, const uint16_t size)
{
	static uint8_t buffer[3 + 32] = { _25LC640A_WRITE, };
	buffer[1] = ((startAddress >> 8) & 0xFF);
	buffer[2] = (startAddress & 0xFF);

	memcpy(buffer + 3, data, MIN_INTEGER(size, 32));

	uint16_t realBufferSize = MIN_INTEGER(size, 32) + 3;

	_25LC640A_writeEnable();

	_25LC640A_CS_SELECT();
	HAL_SPI_Transmit(_25LC640A_spi_handler, buffer, realBufferSize, 1000);
	_25LC640A_CS_DESELECT();
}

void _25LC640A_writeEnable()
{
	static uint8_t buffer[] = { _25LC640A_WREN, };

	_25LC640A_CS_SELECT();
	HAL_SPI_Transmit(_25LC640A_spi_handler, buffer, sizeof(buffer), 1000);
	_25LC640A_CS_DESELECT();
}

void _25LC640A_writeDisable()
{
	static uint8_t buffer[] = { _25LC640A_WRDI, };

	_25LC640A_CS_SELECT();
	HAL_SPI_Transmit(_25LC640A_spi_handler, buffer, sizeof(buffer), 1000);
	_25LC640A_CS_DESELECT();
}

void _25LC640A_readStatusRegister(uint8_t * const status)
{
	static uint8_t buffer[2] = { _25LC640A_WRSR, };

	_25LC640A_CS_SELECT();
	HAL_SPI_Transmit(_25LC640A_spi_handler, buffer, sizeof(buffer), 1000);
	_25LC640A_CS_DESELECT();

	*status = buffer[1];
}

void _25LC640A_protectBlocks(const _25LC640A_ProtectionBlock protect)
{
	static uint8_t buffer[2] = { _25LC640A_WRSR, };
	buffer[1] = protect;

	_25LC640A_writeEnable();

	_25LC640A_CS_SELECT();
	HAL_SPI_Transmit(_25LC640A_spi_handler, buffer, sizeof(buffer), 1000);
	_25LC640A_CS_DESELECT();
}

uint8_t _25LC640A_isBusy()
{
	static uint8_t status;
	_25LC640A_readStatusRegister(&status);

	return (status & (1 << _25LC640A_WIP));
}
