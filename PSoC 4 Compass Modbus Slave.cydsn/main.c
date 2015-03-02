/* ========================================
 *
 * Copyright Sam Walsh, 2015
 * All Rights Reserved.
 *
 * main.c
 *
 * ========================================
*/
#include <project.h>
#include <interrupts.h>
#include <modbus.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include "HMC5883L-Compass.h"
#include "hmc5883l.h"

#define  forever    1
#define  forwards   1
#define  backwards  0

/* Function Prototypes */
void updateEncoder(void);
void updateModbusPackets(void);
float calculateSpeed(void);
int calculatePID(float, float);
void scaleModbusPIDConstants(void);

/* Modbus Variables */
extern unsigned int  holdingReg[50];
extern unsigned char coils[50];
extern unsigned char received[125];
extern unsigned char response[125]; //Enough to return all holding-r's
extern uint8 modbusMessage;

/* Typedef modbus packet */
struct ModbusData {
   int encoderCount;
   int16 encoderHigh, encoderLow;
   int16 speedRPSScaler;
   int16 speedRPS;
   int16 speedRPM;
   uint16 motorCurrentScaler;
   int16 motorCurrent;
   float PIDScaler;
   float Kp,Ki,Kd; 
   int16 setpointSpeedRPM;
};
struct ModbusData mb;

/* Compass Variables */
extern volatile uint8 speedInterruptFlag;
int   interruptCount = 0;
uint8 pidspeed;
int16 cx,cy,cz;


/* Main loop */
int main()
{   
    /*Clear received array */
    memset(&received[0], 0, sizeof(received));
       
    /* Start the SCB UART, Timer and its interrupt */
    ModbusUART_Start();
    MessageReceived_StartEx(messageReceived_isr);  
    writeEnable_Write(0); // receive mode
    
    I2C_MASTER_Start(); //Start communicating to HMC5883L
    
    /* Start the encoder interrupt */
    SpeedInterrupt_StartEx(speed_isr);
    SpeedTimer_Start();

    int i = 0;
    
    CyGlobalIntEnable; /* comment this line to disable global interrupts. */
    
    /* Setup Scaling factors for Modbus */ 
    mb.speedRPSScaler = 1000;
    mb.motorCurrentScaler = 100;
    //mb.PIDScaler = 1000;
    scaleModbusPIDConstants();
    
    // uint8 ida = 0;
    // HMC5883L_Init();
    // HMC5883L_initialize();
    //ida = HMC5883L_getIDA();
    
    HMC5883L_SetUp();
    HMC58X3L_Calibration();
    
    float values[4] = {0,0,0,0};
    
    while(forever)
    {       
        if(modbusMessage)
        {
          processMessage();
          HMC58X3_mgetValues(&values[0]);
        }
        
        //HMC58X3_getlastValues(&cx,&cy,&cz);
        
        
        holdingReg[0] = (int16)values[0];
        holdingReg[1] = (int16)values[1];
        holdingReg[2] = (int16)values[2];
        
        
        if(speedInterruptFlag)
        {          


        }
         
        updateModbusPackets();  
    }
}

void scaleModbusPIDConstants(void)
{
    //Need to actually read the holding registers and do the divide but for now lets
    //leave this as just setting the values.
    if(holdingReg[8] == 0)
    {
        //scaler hasnt been set go with defaults
        mb.PIDScaler = 1000;
        mb.Kp = 0.9;
        mb.Ki = 0.2;
        mb.Kd = 0;
    }
    else
    {
        //scaler hasnt been set go with defaults
        mb.PIDScaler = holdingReg[8];
        mb.Kp = holdingReg[9]/mb.PIDScaler;
        mb.Ki = holdingReg[10]/mb.PIDScaler;
        mb.Kd = holdingReg[11]/mb.PIDScaler;
    }

}


int calculatePID(float currentValue, float setpoint)
{
    static float integral   = 0;
    static float derivative = 0;
    static float lastError = 0;

    float error, fcontrol = 0.0;
    int   control = 0;
    
    error = setpoint - currentValue;
    integral = (integral + error);
    integral = 0.6 * integral; //damping integral 
	derivative = (error - lastError);
	fcontrol =  (mb.Kp * error); 
	fcontrol += (mb.Kd * derivative);
	fcontrol += (mb.Ki * integral);
    control = (int)fcontrol;
    if(control > 255)
    {
        control = 255;
    }
    else if (control < 0)
    {
        control = 0;
    }
    return control;   
}


void updateModbusPackets(void)
{
    holdingReg[0] = mb.encoderLow;
    holdingReg[1] = mb.encoderHigh;
    holdingReg[2] = mb.speedRPSScaler;
    holdingReg[3] = mb.speedRPS;
    holdingReg[4] = mb.speedRPM;
    //holdingReg[5] = duty cycle
    //holdingReg[6] = direction
    holdingReg[7] = mb.motorCurrent;
    //holdingReg[8]  = PIDScaler
    //holdingReg[9]  = Kp
    //holdingReg[10] = Ki
    //holdingReg[11] = Kd
    //holdingReg[12] = setpointRPM
    holdingReg[13] = pidspeed;
    //holdingReg[14] = disable

}


