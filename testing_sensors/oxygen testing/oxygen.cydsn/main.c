#include <project.h>
#include <stdio.h>
#include "libraryCopied.h"

#define Oxygen_IICAddress 0x73   // Replace with your sensor's I2C address

DFRobot_OxygenSensor oxygenSensor;

/*
Make sure to configure PSoC Creator settings to print floats 
*/

int main(void) {
    CyGlobalIntEnable;

    UART_Start();
    I2C_Start();
    
    

    UART_UartPutString("Initializing Oxygen Sensor...\r\n");

    if (!OxygenSensor_Begin(&oxygenSensor, Oxygen_IICAddress)) {
        UART_UartPutString("I2C device connection failed!\r\n");
        while (1) CyDelay(1000);  // Halt here if initialization fails
    }

    UART_UartPutString("I2C connection successful!\r\n");

    for (;;) {
        float oxygenData = OxygenSensor_GetOxygenData(&oxygenSensor, 10);
        char buffer[50];
        sprintf(buffer, "Oxygen concentration: %.2f %%vol\r\n", oxygenData);
        UART_UartPutString(buffer);

        CyDelay(1000);  // Delay for 1 second
    }
}