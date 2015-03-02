/*
 * I2C Library for GY-86 PCB and all similar sensor boards v1.0.1
 * Author: Dr Anastasios Kanakis | electronix79
*/

#include "i2c.h"
#include <project.h>

uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data);

uint8_t I2C_ReadOneByte(uint8_t I2C_Addr, uint8_t addr)
{
	uint8_t res = 0;

	if (I2C_MASTER_I2CMasterSendStart(I2C_Addr, I2C_MASTER_I2C_WRITE_XFER_MODE) != I2C_MASTER_I2C_MSTR_NO_ERROR) return 0;
	if (I2C_MASTER_I2CMasterWriteByte(addr) != I2C_MASTER_I2C_MSTR_NO_ERROR) return 0;
	if (I2C_MASTER_I2CMasterSendRestart(I2C_Addr, I2C_MASTER_I2C_READ_XFER_MODE) != I2C_MASTER_I2C_MSTR_NO_ERROR) return 0;
	res = (uint8_t)(I2C_MASTER_I2CMasterReadByte(I2C_MASTER_I2C_NAK_DATA));
	if (I2C_MASTER_I2CMasterSendStop() != I2C_MASTER_I2C_MSTR_NO_ERROR) return 0;

	return res;
}

uint8_t IICreadBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t *data)
{
	uint8_t count = 0;

    I2C_MASTER_I2CMasterSendStart(dev, I2C_MASTER_I2C_WRITE_XFER_MODE);
	I2C_MASTER_I2CMasterWriteByte(reg);
	I2C_MASTER_I2CMasterSendRestart(dev, I2C_MASTER_I2C_READ_XFER_MODE);
	for(count = 0; count < length; count++)
	{
		if(count != length - 1)
			data[count] = (uint8_t)(I2C_MASTER_I2CMasterReadByte(I2C_MASTER_I2C_ACK_DATA));
		else
			data[count] = (uint8_t)(I2C_MASTER_I2CMasterReadByte(I2C_MASTER_I2C_NAK_DATA));
	}
	I2C_MASTER_I2CMasterSendStop();
	
    return count;
}

uint8_t IICreadByte(uint8_t dev, uint8_t reg, uint8_t *data)
{
	*data = I2C_ReadOneByte(dev, reg);
	return 1;
}

uint8_t IICwriteBit(uint8_t dev, uint8_t reg, uint8_t bitNum, uint8_t data)
{
	uint8_t b;

	IICreadByte(dev, reg, &b);
	b = (data != 0) ? (b | (1 << bitNum)) : (b & ~(1 << bitNum));
	
	return IICwriteByte(dev, reg, b);
}

uint8_t IICwriteBits(uint8_t dev, uint8_t reg, uint8_t bitStart, uint8_t length, uint8_t data)
{
	uint8_t b;

	if (IICreadByte(dev, reg, &b) != 0)
	{
		uint8_t mask = (0xFF << (bitStart + 1)) | 0xFF >> ((8 - bitStart) + length - 1);

		data <<= (8 - length);
		data >>= (7 - bitStart);
		b &= mask;
		b |= data;
		
		return IICwriteByte(dev, reg, b);
	}
	else
		return 0;
}

uint8_t IICwriteByte(uint8_t dev, uint8_t reg, uint8_t data)
{
	return IICwriteBytes(dev, reg, 1, &data);
}

uint8_t IICwriteBytes(uint8_t dev, uint8_t reg, uint8_t length, uint8_t* data)
{
	uint8_t count = 0;

    I2C_MASTER_I2CMasterSendStart(dev, I2C_MASTER_I2C_WRITE_XFER_MODE);
	I2C_MASTER_I2CMasterWriteByte(reg);
	for(count = 0; count < length; count++)
	{
		I2C_MASTER_I2CMasterWriteByte(data[count]);
	}
	I2C_MASTER_I2CMasterSendStop();
    
	return 1;
}

