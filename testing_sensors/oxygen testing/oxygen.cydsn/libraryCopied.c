#include <project.h>
#include <stdio.h>
#include <string.h>
#include "libraryCopied.h"
#include "cyapicallbacks.h"

#define GET_KEY_REGISTER       0x0A  // Define appropriate register values
#define USER_SET_REGISTER      0x08
#define AUTUAL_SET_REGISTER    0x09
#define OXYGEN_DATA_REGISTER   0x03

#define COLLECT_MAX            10    // Maximum number of data points for averaging

/**
 * Initialize the oxygen sensor by checking the I2C connection.
 */
bool OxygenSensor_Begin(DFRobot_OxygenSensor* sensor, uint8_t addr) {
    uint8_t status = 0;
    sensor->_addr = addr;
    
    I2C_I2CMasterClearStatus();
    // Check the I2C connection
    status = I2C_I2CMasterSendStart(addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (status) {
        char debugMsg[50];
        sprintf(debugMsg, "Oxygen Begin: I2C Send Start Error Code: 0x%08X\r\n", status);
        UART_UartPutString(debugMsg);
        I2C_I2CMasterSendStop(TIMEOUT);
        return status;
    }
    
    I2C_I2CMasterSendStop(TIMEOUT);

    return (status == I2C_I2C_MSTR_NO_ERROR);
}

/**
 * Read the calibration key from the sensor's memory.
 */
void OxygenSensor_ReadFlash(DFRobot_OxygenSensor* sensor) {
    uint8 value = 0;
    uint8_t err = 0;
    I2C_I2CMasterClearStatus();

    // Send GET_KEY_REGISTER command
    err = I2C_I2CMasterSendStart(sensor->_addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    
    if (err) {
        char debugMsg[50];
        sprintf(debugMsg, "Oxygen Flash: I2C Send Start Error Code: 0x%08X\r\n", err);
        UART_UartPutString(debugMsg);
        I2C_I2CMasterSendStop(TIMEOUT);
        return;
    }
    
    I2C_I2CMasterWriteByte(GET_KEY_REGISTER, TIMEOUT);
    
    err = I2C_I2CMasterSendRestart(sensor->_addr, I2C_I2C_READ_XFER_MODE, TIMEOUT);
    if (err) {
        char debugMsg[50];
        sprintf(debugMsg, "Oxygen Flash: I2C Send Restart Error Code: 0x%08X\r\n", err);
        UART_UartPutString(debugMsg);
        I2C_I2CMasterSendStop(TIMEOUT);
        return;
    }
    
    I2C_I2CMasterReadByte(I2C_I2C_NAK_DATA, &value, TIMEOUT);
    I2C_I2CMasterSendStop(TIMEOUT);

    if (value == 0) {
        sensor->_Key =  20.9 / 120.0;
    } else {
        sensor->_Key = value / 1000.0;
    }
    
}

/**
 * Write a value to a specific register on the sensor.
 */
void OxygenSensor_I2CWrite(DFRobot_OxygenSensor* sensor, uint8_t reg, uint8_t data) {
    uint8_t err = 0;
    I2C_I2CMasterClearStatus();
    err = I2C_I2CMasterSendStart(sensor->_addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        return;
    }
    I2C_I2CMasterWriteByte(reg, TIMEOUT);
    I2C_I2CMasterWriteByte(data, TIMEOUT);
    I2C_I2CMasterSendStop(TIMEOUT);
}

/**
 * Calibrate the oxygen sensor.
 */
void OxygenSensor_Calibrate(DFRobot_OxygenSensor* sensor, float vol, float mv) {
    uint8_t keyValue;

    if (mv < 0.000001 && mv > -0.000001) {
        keyValue = vol * 10;
        OxygenSensor_I2CWrite(sensor, USER_SET_REGISTER, keyValue);
    } else {
        keyValue = (vol / mv) * 1000;
        OxygenSensor_I2CWrite(sensor, AUTUAL_SET_REGISTER, keyValue);
    }
}

/**
 * Get the oxygen concentration from the sensor.
 */
float OxygenSensor_GetOxygenData(DFRobot_OxygenSensor* sensor, uint8_t collectNum) {
    uint8 rxbuf[10] = {0};
    uint8_t i = 0;
    static uint8_t collected = 0;

    if (collectNum > COLLECT_MAX) collectNum = COLLECT_MAX;

    OxygenSensor_ReadFlash(sensor);

    for (i = collectNum - 1; i > 0; i--) {
        sensor->oxygenData[i] = sensor->oxygenData[i - 1];
    }
    
    uint32 err;
    // Send OXYGEN_DATA_REGISTER command
    I2C_I2CMasterClearStatus();
    err = I2C_I2CMasterSendStart(sensor->_addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        char debugMsg[50];
        sprintf(debugMsg, "Oxygen Data: I2C Send Start Error Code: 0x%08X\r\n", err);
        UART_UartPutString(debugMsg);
        return err;
    }
    I2C_I2CMasterWriteByte(OXYGEN_DATA_REGISTER, TIMEOUT);
  
    err = I2C_I2CMasterSendRestart(sensor->_addr, I2C_I2C_READ_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        UART_UartPutString("Oxygen Data: I2CMasterSendRestart not working!\r\n");
        return err;
    }
    
    // Request 3 bytes of oxygen data
    for (i = 0; i < 3; i++) {
       I2C_I2CMasterReadByte((i < 2) ? I2C_I2C_ACK_DATA : I2C_I2C_NAK_DATA, &rxbuf[i], TIMEOUT);
    }
    
    

    I2C_I2CMasterSendStop(TIMEOUT);

    sensor->oxygenData[0] = (sensor->_Key) * ((float)rxbuf[0] + ((float)rxbuf[1] / 10.0) + ((float)rxbuf[2] / 100.0));
   
    if (collected < collectNum) collected++;
    return OxygenSensor_GetAverage(sensor->oxygenData, collected);
}

/**
 * Calculate the average of collected data points.
 */
float OxygenSensor_GetAverage(float data[], uint8_t len) {
    uint8_t i = 0;
    float sum = 0.0;

    for (i = 0; i < len; i++) {
        sum += data[i];
    }

    return sum / (float)len;
}