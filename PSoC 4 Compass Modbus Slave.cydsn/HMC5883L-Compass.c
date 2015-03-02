/*
 * HMC5883 Library v1.0.1
 * Author: Dr Anastasios Kanakis | electronix79
*/

#include "HMC5883L-Compass.h"
#include <project.h>

float HMC5883_lastx, HMC5883_lasty, HMC5883_lastz;
int16_t  HMC5883_FIFO[3][11];

uint8_t HMC5883_IS_newdata(void);
void HMC58X3_writeReg(uint8_t reg, uint8_t val);
void HMC58X3_newValues(int16_t x, int16_t y, int16_t z);
void HMC58X3_getRaw(int16_t *x, int16_t *y, int16_t *z);
void HMC58X3_setGain(uint8_t gain);
void HMC58X3_setMode(uint8_t mode);
void HMC58X3_setDOR(uint8_t DOR);
void HMC58X3_init(uint8_t setmode);
void HMC58X3_FIFO_init(void);

#ifdef CALIBRATION_NEW

extern char *BufferPrint;					// Used for print to UART

HMC58X3_Calib Mag_Calibration;				// Global variables for HMC58X3

// Read the maximum value from Positive and Negative Bias
void HMC58X3L_Calibration(void)
{
	float x, y, z, mx = 0, my = 0, mz = 0, max = 0;
	int16_t mag_x, mag_y, mag_z, i;
	int8_t flag_mag = 0;
	uint8_t temp_buf[6] = {0};
	
	Mag_Calibration.x_scale = 1;
	Mag_Calibration.y_scale = 1;
	Mag_Calibration.z_scale = 1;

	HMC58X3_writeReg(HMC58X3_R_MODE, Mode_Idle);													// Idle mode

	do
	{
		++flag_mag;
		if (flag_mag == MS1_MS0_Pos)
		{
			HMC58X3_writeReg(HMC58X3_R_CONFA, ((DataRate_75Hz << DataRate_Shift) | MS1_MS0_Pos));	// 1 samples averaged, 75Hz frequency, Positive bias configuration
			HMC58X3_writeReg(HMC58X3_R_CONFB, (GainMode_1_3Ga << GainMode_Shift));					// 1.3Ga gain
		}
		else if (flag_mag == MS1_MS0_Neg)
		{
			HMC58X3_writeReg(HMC58X3_R_CONFA, ((DataRate_75Hz << DataRate_Shift) | MS1_MS0_Neg));	// 1 samples averaged, 75Hz frequency, Negative bias configuration
			HMC58X3_writeReg(HMC58X3_R_CONFB, (GainMode_1_3Ga << GainMode_Shift));					// 1.3Ga gain
		}

		HMC58X3_writeReg(HMC58X3_R_MODE, Mode_Single);												// Single mode
		CyDelay(15);																				// Delay of 0.015sec (75Hz) before read again raw data from HMC58X3 sensor
		// Attention!!! Because first measurement after a gain change provide the same value as before change, then in order to fix this read just make one dummy reading.
		IICreadBytes(HMC58X3_ADDR, HMC58X3_R_XM,6, temp_buf);										// Ignore this reading!

		for (i = 0; i < 50; i++)
		{
			HMC58X3_writeReg(HMC58X3_R_MODE, Mode_Single);											// Single mode
			CyDelay(15);																			// Delay more than ~0.013sec for (75Hz) before read again raw data from HMC58X3 sensor

			IICreadBytes(HMC58X3_ADDR, HMC58X3_R_XM,6, temp_buf);

			mag_x = (((uint16_t)temp_buf[0]) << 8) | ((uint16_t)temp_buf[1]);
			mag_z = (((uint16_t)temp_buf[2]) << 8) | ((uint16_t)temp_buf[3]);
			mag_y = (((uint16_t)temp_buf[4]) << 8) | ((uint16_t)temp_buf[5]);

			x = ((float)mag_x) / Mag_Calibration.x_scale;
			y = ((float)mag_y) / Mag_Calibration.y_scale;
			z = ((float)mag_z) / Mag_Calibration.z_scale;

			if ((x > 0) && (x > mx))
				mx = x;
			else if ((x < 0) && ((-x) > mx))
				mx = (-x);

			if ((y > 0) && (y > my))
				my = y;
			else if ((y < 0) && ((-y) > my))
				my = (-y);

			if ((z > 0) && (z > mz))
				mz = z;
			else if ((z < 0) && ((-z) > mz))
				mz = (-z);
		}
	}while (flag_mag < 2);			// Loop only twice!

	// Find the maximum value from any of 3 axis, all below variables are positive
	if (mx > max) max = mx;
	if (my > max) max = my;
	if (mz > max) max = mz;

	Mag_Calibration.x_max = mx;
	Mag_Calibration.y_max = my;
	Mag_Calibration.z_max = mz;

	// Calculate Scales
	if (mx != 0)
		Mag_Calibration.x_scale = max / mx;
	if (my != 0)
		Mag_Calibration.y_scale = max / my;
	if (mz != 0)
		Mag_Calibration.z_scale = max / mz;
}

#endif

uint8_t HMC5883_IS_newdata(void)
{
	if(Pin_GY86_DRDY_Read() == ON)
		return 1;
	else
		return 0;
}

void HMC58X3_writeReg(uint8_t reg, uint8_t val)
{
	IICwriteByte(HMC58X3_ADDR, reg, val);
}

void  HMC58X3_newValues(int16_t x, int16_t y, int16_t z)
{
	uint8_t i;
	int32_t sum = 0;

	for(i = 1; i < 10; i++)
	{
		HMC5883_FIFO[0][i - 1] = HMC5883_FIFO[0][i];
		HMC5883_FIFO[1][i - 1] = HMC5883_FIFO[1][i];
		HMC5883_FIFO[2][i - 1] = HMC5883_FIFO[2][i];
	}

	HMC5883_FIFO[0][9] = x;
	HMC5883_FIFO[1][9] = y;
	HMC5883_FIFO[2][9] = z;

	sum = 0;
	for(i = 0; i < 10; i++)
	{
		sum += HMC5883_FIFO[0][i];
	}
	HMC5883_FIFO[0][10] = sum / 10;

	sum = 0;
	for(i = 0; i < 10; i++)
	{
		sum += HMC5883_FIFO[1][i];
	}
	HMC5883_FIFO[1][10] = sum / 10;

	sum = 0;
	for(i = 0; i < 10; i++)
	{
		sum += HMC5883_FIFO[2][i];
	}
	HMC5883_FIFO[2][10] = sum / 10;
}

void HMC58X3_getRaw(int16_t *x, int16_t *y, int16_t *z)
{
	uint8_t vbuff[6];
	vbuff[0] = vbuff[1] = vbuff[2] = vbuff[3] = vbuff[4] = vbuff[5] = 0;

	IICreadBytes(HMC58X3_ADDR, HMC58X3_R_XM,6, vbuff);
	HMC58X3_newValues(((int16_t)vbuff[0] << 8) | vbuff[1], ((int16_t)vbuff[4] << 8) | vbuff[5], ((int16_t)vbuff[2] << 8) | vbuff[3]);
	*x = HMC5883_FIFO[0][10];
	*y = HMC5883_FIFO[1][10];
	*z = HMC5883_FIFO[2][10];
}

void HMC58X3_setGain(uint8_t gain)
{ 
	if (gain > 7)
		return;
	HMC58X3_writeReg(HMC58X3_R_CONFB, gain << 5);
}

void HMC58X3_setMode(uint8_t mode)
{
	if (mode > 2)
		return;
	HMC58X3_writeReg(HMC58X3_R_MODE, mode);
	CyDelayUs(100);
}

void HMC58X3_setDOR(uint8_t DOR)
{
	if (DOR > 6)
		return;
	HMC58X3_writeReg(HMC58X3_R_CONFA, DOR << 2);
}

void HMC58X3_init(uint8_t setmode)
{
	if (setmode)
		HMC58X3_setMode(0);

	HMC58X3_writeReg(HMC58X3_R_CONFA, 0x70);	// 8 samples averaged, 15Hz frequency, Normal measurement configuration (no artificial bias)
	HMC58X3_writeReg(HMC58X3_R_CONFB, 0xA0);	// 4.7Ga gain
	HMC58X3_writeReg(HMC58X3_R_MODE, 0x00);		// Continuous mode
}

void HMC58X3_FIFO_init(void)
{
	int16_t temp[3];
	uint8_t i;

	for(i = 0; i < 50; i++)
	{
		HMC58X3_getRaw(&temp[0], &temp[1], &temp[2]);
		CyDelayUs(200);
	}
}

void HMC58X3_getID(char id[3])
{
	id[0] = I2C_ReadOneByte(HMC58X3_ADDR, HMC58X3_R_IDA);
	id[1] = I2C_ReadOneByte(HMC58X3_ADDR, HMC58X3_R_IDB);
	id[2] = I2C_ReadOneByte(HMC58X3_ADDR, HMC58X3_R_IDC);
}

void HMC58X3_getlastValues(int16_t *x, int16_t *y, int16_t *z)
{
	*x = HMC5883_FIFO[0][10];
	*y = HMC5883_FIFO[1][10];
	*z = HMC5883_FIFO[2][10];
}

void HMC58X3_mgetValues(float *arry)
{
	int16_t xr, yr, zr;
	
	HMC58X3_getRaw(&xr, &yr, &zr);
	arry[0] = HMC5883_lastx = (float)(xr);
	arry[1] = HMC5883_lasty = (float)(yr);
	arry[2] = HMC5883_lastz = (float)(zr);
}

void HMC5883L_SetUp(void)
{
	HMC58X3_init(0);						// Don't set mode yet, we'll do that later on.
	HMC58X3_setMode(0);						// Continues-conversion mode
	HMC58X3_setDOR(6);						// 75Hz
	HMC58X3_FIFO_init();
}

