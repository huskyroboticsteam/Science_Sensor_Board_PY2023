#include "project.h"
#include <stdio.h>
#include <stdlib.h>

char buffer[16]; // Buffer for UART output

int main(void)
{
    CyGlobalIntEnable; // Enable global interrupts
    
    UART_Start(); // Start UART
    ADC_Start();  // Start ADC
    ADC_StartConvert(); // Start ADC conversion
    for(;;)
    {
         UART_UartPutString(buffer);
        if(ADC_IsEndConversion(ADC_WAIT_FOR_RESULT)) // Wait for conversion to complete
        {
            int16 value = ADC_GetResult16(0); // Get ADC result from channel 0
            sprintf(buffer, "%d\r\n", value); // Format the value as a string
            UART_UartPutString(buffer); 
        }
        CyDelay(100); // Delay 100 ms
    }
}