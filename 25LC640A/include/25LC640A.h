/*
 * 25LC640A.h
 *
 *  Created on: Jan 25, 2024
 *      Author: Kolya
 */

#ifndef INC_25LC640A_H_
#define INC_25LC640A_H_

#include "main.h"

#define _25LC640A_MEMORY_SIZE 0x2000

typedef enum
{
	_25LC640A_READ = 0x03,
	_25LC640A_WRITE = 0x02,
	_25LC640A_WRDI = 0x04,
	_25LC640A_WREN = 0x06,
	_25LC640A_RDSR = 0x05,
	_25LC640A_WRSR = 0x01,
} _25LC640A_Instruction;

typedef enum
{
	_25LC640A_WIP = 0,
	_25LC640A_WEL = 1,
	_25LC640A_BP0 = 2,
	_25LC640A_BP1 = 3,
	_25LC640A_WPEN = 7,
} _25LC640A_StatusRegisterBitPosition;

/*
 * Array protection
 * _______________________________
 * | BP0 | BP1 | Protected range |
 * |-----|-----|-----------------|
 * |  0  |  0  |      none       |
 * |  0  |  1  |  0x1800-0x1FFF  |
 * |  1  |  0  |  0x1000-0x1FFF  |
 * |  1  |  1  |      full       |
 * |-----|-----|-----------------|
 *
 */

typedef enum
{
	_25LC640A_ProtectionBlock_NONE = (0x00 << 2) | (1 << 7),
	_25LC640A_ProtectionBlock_1_4 = (0x01 << 2) | (1 << 7),
	_25LC640A_ProtectionBlock_1_2 = (0x02 << 2) | (1 << 7),
	_25LC640A_ProtectionBlock_FULL = (0x03 << 2) | (1 << 7),
} _25LC640A_ProtectionBlock;

void _25LC640A_init(SPI_HandleTypeDef * const spi);

void _25LC640A_read(const uint16_t startAddress, uint8_t * const buffer, const uint16_t size);

void _25LC640A_writeByte(const uint16_t startAddress, const uint8_t value);
void _25LC640A_write(const uint16_t startAddress, const uint8_t * const data, const uint16_t size);

void _25LC640A_writeEnable();
void _25LC640A_writeDisable();

void _25LC640A_readStatusRegister(uint8_t * const status);
void _25LC640A_protectBlocks(const _25LC640A_ProtectionBlock protect);

uint8_t _25LC640A_isBusy();

#endif /* INC_25LC640A_H_ */
