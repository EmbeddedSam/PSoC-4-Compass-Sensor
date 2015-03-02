/*
 * I2C Library for GY-86 PCB and all similar sensor boards v1.0.1
 * Author: Dr Anastasios Kanakis | electronix79
*/
    
#ifndef _IOI2C_H_
#define _IOI2C_H_

#include "main.h"

uint8_t I2C_ReadOneByte(uint8_t I2C_Addr, uint8_t addr);
uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data);
uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data);
uint8_t IICwriteBits(uint8_t dev, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data);
uint8_t IICwriteByte(uint8_t dev, uint8_t reg, uint8_t data);
uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data);

#endif

