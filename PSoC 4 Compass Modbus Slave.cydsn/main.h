#ifndef _MAIN_H_
#define _MAIN_H_

#include "stdint.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <project.h>

#define CALIBRATION_NEW         // Used for MPU6050 and HMC5883

#define ON						1u
#define OFF                 	0u

typedef union
{
    float value;
    uint8_t byte[4];
} float_in_bytes;

typedef union
{
    int16_t value;
    uint8_t byte[2];
} uint16_in_bytes;

#include "i2c.h"
#include "HMC5883L-Compass.h"


#endif

