/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include "main.h"
#include "cyapicallbacks.h"
#include "Sensor_Stuff.h"

// LED stuff
volatile uint8_t CAN_time_LED = 0;
volatile uint8_t ERROR_time_LED = 0;

// UART stuff
char txData[TX_DATA_SIZE];

int main(void)
{ 
    Initialize();
   
    sprintf(txData, "\r\nHello2\r\n");
    Print(txData);
    
    int err;
    
    // uint16 temp;
    // readReg16crc(SCD41_ADDR, 0x3682, &temp);
    // Print("Device SN: ");
    // PrintInt(temp);
    // Print("\r\n");
    
    
    for(;;)
    {
        
        err = 0;
        
        err = ReadSensorCO2();
      
        //if (err) DisplayErrorCode(err);
        
        if (DBG_UART_SpiUartGetRxBufferSize()) {
            DebugPrint(DBG_UART_UartGetByte());
        }
        
        CyDelay(1000);
        
    }
}

void Initialize(void) {
    uint32 err;
    CyGlobalIntEnable; /* Enable global interrupts. LED arrays need this first */
    
    sprintf(txData, "\r\nHello\r\n");
    Print(txData);
    
    //address = 0; // TODO replace with science sensor address
    
    DBG_UART_Start();
    I2C_Start();
    
    //sprintf(txData, "Address: %x \r\n", address);
    //Print(txData);
    
    CyDelay(30); // ensure sensors are initialized
    err = initializeSensors();
    if (err) {
        Print("Failed sensor init: ");
        PrintInt(err);
        Print("\r\n");
    }
}

void DebugPrint(char input) {
    uint16 val = -1;
    switch(input) {
        case 't':
            sprintf(txData, "Temp: %li", ReadSensorTemperature());
            break;
        case 'h':
            sprintf(txData, "Moist: %li", ReadSensorHumidity());
            break;
        case 'c':
            sprintf(txData, "CO: %li", ReadSensorCO());
            break;
        case 'd':
            sprintf(txData, "CO2: %li", ReadSensorCO2());
            break;
        case 'o':
            sprintf(txData, "O2: %li", ReadSensorO2());
            break;
        case 'm':
            sprintf(txData, "CH4: %li", ReadSensorCH4());
            break;
        case ' ':
            readReg16crc(SCD41_ADDR, SCD41_REG_get_data_ready_status, &val);
            sprintf(txData, "Data Ready: %X", val);
            // writeReg0(SCD41_ADDR, SCD41_REG_perform_forced_recalibration);
            // sprintf(txData, "Cal");
            break;
        default:
            sprintf(txData, "what");
            break;
    }
    Print(txData);
    Print("\r\n");
}
/*
void DisplayErrorCode(uint8_t code) {    
    ERROR_time_LED = 0;
    LED_ERR_Write(ON);
    
    sprintf(txData, "Error %X\r\n", code);
    Print(txData);

    switch(code)
    {
        case ERROR_INVALID_TTC:
            Print("Cannot Send That Data Type!\n\r");
            break;
        default:
            //some error
            break;
    }
}
*/

/* [] END OF FILE */
