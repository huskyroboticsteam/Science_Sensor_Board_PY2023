#include "project.h"
#include <stdio.h>
#include <string.h>

#define GET_KEY_REGISTER       0x01  // Define appropriate register values
#define USER_SET_REGISTER      0x02
#define AUTUAL_SET_REGISTER    0x03
#define OXYGEN_DATA_REGISTER   0x04

#define COLLECT_MAX            10    // Maximum number of data points for averaging

typedef struct {
    uint8_t _addr;
    float _Key;
    float oxygenData[COLLECT_MAX];
} DFRobot_OxygenSensor;

/**
 * Initialize the oxygen sensor by checking the I2C connection.
 */
bool OxygenSensor_Begin(DFRobot_OxygenSensor* sensor, uint8_t addr) {
    uint8_t status = 0;
    sensor->_addr = addr;
    
    // Check the I2C connection
    status = I2C_MasterSendStart(addr, I2C_WRITE_XFER_MODE);
    I2C_MasterSendStop();

    return (status == I2C_MSTR_NO_ERROR);
}

/**
 * Read the calibration key from the sensor's memory.
 */
void OxygenSensor_ReadFlash(DFRobot_OxygenSensor* sensor) {
    uint8_t value = 0;

    // Send GET_KEY_REGISTER command
    I2C_MasterSendStart(sensor->_addr, I2C_WRITE_XFER_MODE);
    I2C_MasterWriteByte(GET_KEY_REGISTER);
    I2C_MasterSendStop();

    CyDelay(50);

    // Request 1 byte of data
    I2C_MasterSendStart(sensor->_addr, I2C_READ_XFER_MODE);
    value = I2C_MasterReadByte(I2C_NAK_DATA);
    I2C_MasterSendStop();

    if (value == 0) {
        sensor->_Key = 20.9 / 120.0;
    } else {
        sensor->_Key = (float)value / 1000.0;
    }
}

/**
 * Write a value to a specific register on the sensor.
 */
void OxygenSensor_I2CWrite(DFRobot_OxygenSensor* sensor, uint8_t reg, uint8_t data) {
    I2C_MasterSendStart(sensor->_addr, I2C_WRITE_XFER_MODE);
    I2C_MasterWriteByte(reg);
    I2C_MasterWriteByte(data);
    I2C_MasterSendStop();
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
    uint8_t rxbuf[3] = {0};
    uint8_t i = 0;
    static uint8_t collected = 0;

    if (collectNum > COLLECT_MAX) collectNum = COLLECT_MAX;

    OxygenSensor_ReadFlash(sensor);

    for (i = collectNum - 1; i > 0; i--) {
        sensor->oxygenData[i] = sensor->oxygenData[i - 1];
    }

    // Send OXYGEN_DATA_REGISTER command
    I2C_MasterSendStart(sensor->_addr, I2C_WRITE_XFER_MODE);
    I2C_MasterWriteByte(OXYGEN_DATA_REGISTER);
... (30 lines left)