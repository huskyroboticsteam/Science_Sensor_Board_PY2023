#include "project.h"
#include <stdio.h>
#include <stdlib.h>

char buffer[16]; // Buffer for UART output

int main(void)
{
    CyGlobalIntEnable; // Enable global interrupts
    
    
    UART_Start(); // Start UART (make sure uart_rx is P7[0] and uart_tx P7[1] for pins)
    //Uart baud should be 115200 and match in putty (also change oversampling to 12)
    ADC_Start();  // Start Analog to digital conversion
    ADC_StartConvert(); // Start ADC conversion
    for(;;) //run forever
    {
        UART_UartPutString(buffer);
        if(ADC_IsEndConversion(ADC_WAIT_FOR_RESULT)) // Wait for conversion to complete
        {
            int16 value = ADC_GetResult16(0); // Get ADC result from channel 0
            sprintf(buffer, "%d\r\n", value); // Format the value as a string
            UART_UartPutString(buffer); //print the data
        }
        CyDelay(100); // Delay 100 ms
    }
}