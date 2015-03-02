/*
 * HMC5883 Library v1.0.1
 * Author: Dr Anastasios Kanakis | electronix79
*/
    
#ifndef _HMC5883L_H_
#define _HMC5883L_H_

#include "main.h"

#define HMC58X3_ADDR			((0x3C)/2)		// 7 bit address of the HMC58X3

#define HMC_POS_BIAS			1
#define HMC_NEG_BIAS			2

#define HMC58X3_R_CONFA			0
#define HMC58X3_R_CONFB			1
#define HMC58X3_R_MODE			2
#define HMC58X3_R_XM			3
#define HMC58X3_R_XL			4
#define HMC58X3_R_YM			(7)
#define HMC58X3_R_YL			(8)
#define HMC58X3_R_ZM			(5)
#define HMC58X3_R_ZL			(6)

#define HMC58X3_R_STATUS		9
#define HMC58X3_R_IDA			10
#define HMC58X3_R_IDB			11
#define HMC58X3_R_IDC			12

#define	DataRate_0_75Hz			0				// 0.75Hz mode
#define	DataRate_1_5Hz			1				// 1.5Hz mode
#define	DataRate_3_0Hz			2				// 3Hz mode
#define	DataRate_7_5Hz			3				// 7.5Hz mode
#define	DataRate_15Hz			4				// 15Hz mode (default)
#define	DataRate_30Hz			5				// 30Hz mode
#define	DataRate_75Hz			6				// 75Hz mode

#define	GainMode_0_88Ga			0				// Gain at 0.88Ga, (counts/milli-gauss 1620)
#define	GainMode_1_3Ga			1				// Gain at 1.3Ga, (counts/milli-gauss 1300) (default)
#define	GainMode_1_9Ga			2				// Gain at 1.9Ga, (counts/milli-gauss 970)
#define	GainMode_2_5Ga			3				// Gain at 2.5Ga, (counts/milli-gauss 780)
#define	GainMode_4_0Ga			4				// Gain at 4.0Ga, (counts/milli-gauss 530)
#define	GainMode_4_7Ga			5				// Gain at 4.7Ga, (counts/milli-gauss 460)
#define	GainMode_5_6Ga			6				// Gain at 5.6Ga, (counts/milli-gauss 390)
#define	GainMode_8_1Ga			7				// Gain at 8.1Ga, (counts/milli-gauss 390)

#define	Mode_Continues			0				// Continues-conversion mode
#define	Mode_Single				1				// Single-conversion mode
#define	Mode_Idle				2				// Idle mode
#define	Mode_Sleep				3				// Sleep mode

#define	DataRate_Shift			2				// 2 bits shift left
#define	GainMode_Shift			5				// 5 bits shift left
#define	SampleAve_Shift			7				// 7 bits shift left

#define	MS1_MS0_Normal			0				// Normal measurement configuration (default).
#define	MS1_MS0_Pos				1				// Positive bias configuration.
#define	MS1_MS0_Neg				2				// Negative bias configuration.

// Select number of samples averaged (1 to 8) per measurement output.
#define	MA1_MA0_00				0				// =1 Normal measurement configuration (default).
#define	MA1_MA0_01				1				// =2
#define	MA1_MA0_10				2				// =4
#define	MA1_MA0_11				3				// =8

#ifdef CALIBRATION_NEW

typedef struct
{
	float x_scale, y_scale, z_scale, x_max, y_max, z_max;
} HMC58X3_Calib;

void HMC58X3L_Calibration(void);

#endif

void HMC58X3_getID(char id[3]);	
void HMC58X3_getlastValues(int16_t *x, int16_t *y, int16_t *z);
void HMC58X3_mgetValues(float *);
void HMC5883L_SetUp(void);

#endif

