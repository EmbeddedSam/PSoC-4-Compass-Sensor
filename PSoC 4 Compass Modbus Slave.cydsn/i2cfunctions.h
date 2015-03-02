/* ========================================
 *
 * Copyright Samuel Walsh, 2014
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF Samuel Walsh.
 *
 * ========================================
*/

/* [] END OF FILE */

#include <project.h>

void I2CReadBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *value);

void I2CReadByte(uint8_t devAddr, uint8_t regAddr, uint8_t *value);

void I2CReadBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t *value);

void I2CReadBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t *value);
	
void I2CWriteBytes(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint8_t *value);

void I2CWriteByte(uint8_t devAddr, uint8_t regAddr, uint8_t value);

void I2CWriteBits(uint8_t devAddr, uint8_t regAddr, uint8_t bitStart, uint8_t length, uint8_t value);

void I2CWriteBit(uint8_t devAddr, uint8_t regAddr, uint8_t bitNum, uint8_t value);

void I2CWriteWords(uint8_t devAddr, uint8_t regAddr, uint8_t length, uint16_t *value);

void I2CWriteWord(uint8_t devAddr, uint8_t regAddr, uint16_t value);