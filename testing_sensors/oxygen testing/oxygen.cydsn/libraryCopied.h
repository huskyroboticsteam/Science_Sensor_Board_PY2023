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

#pragma once 

#include <project.h>
#include <stdbool.h>
#include "I2C.h"

// Constants
#define GET_KEY_REGISTER       0x0A  // Register for reading the key value
#define USER_SET_REGISTER      0x08  // Register for user calibration
#define AUTUAL_SET_REGISTER    0x09  // Register for automatic calibration
#define OXYGEN_DATA_REGISTER   0x03  // Register for reading oxygen data
#define COLLECT_MAX            10    // Maximum number of data points for averaging
#define TIMEOUT 20

// Oxygen sensor struct
typedef struct {
    uint8_t _addr;                   // I2C address of the sensor
    float _Key;                      // Calibration key
    float oxygenData[COLLECT_MAX];   // Buffer for storing oxygen data
} DFRobot_OxygenSensor;

/*
 
Initialize the oxygen sensor by checking the I2C connection.
@param sensor Pointer to the oxygen sensor structure
@param addr   I2C address of the sensor
@return true if the sensor is successfully initialized, false otherwise*/
bool OxygenSensor_Begin(DFRobot_OxygenSensor* sensor, uint8_t addr);

/*
 
Read the calibration key from the sensor's memory.
@param sensor Pointer to the oxygen sensor structure*/
void OxygenSensor_ReadFlash(DFRobot_OxygenSensor* sensor);

/**
 
Write a value to a specific register on the sensor.
@param sensor Pointer to the oxygen sensor structure
@param reg    Register address to write to
@param data   Data to write to the register*/
void OxygenSensor_I2CWrite(DFRobot_OxygenSensor* sensor, uint8_t reg, uint8_t data);

void OxygenSensor_Calibrate(DFRobot_OxygenSensor* sensor, float vol, float mv);

float OxygenSensor_GetOxygenData(DFRobot_OxygenSensor* sensor, uint8_t collectNum);

float OxygenSensor_GetAverage(float data[], uint8_t len);

/* [] END OF FILE */
