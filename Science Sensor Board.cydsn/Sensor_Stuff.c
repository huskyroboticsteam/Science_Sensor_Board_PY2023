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
#include <project.h>
#include "main.h"
#include "cyapicallbacks.h"
#include "Sensor_Stuff.h"

char txData[TX_DATA_SIZE];

int32 ReadSensorTemperature() {
    // TODO
    return 0;
}

int32 ReadSensorHumidity() {
    // TODO
    return 0;
}

int32 ReadSensorCO() {
    // TODO
    return 0;
}

int32 ReadSensorCO2() { 
    int16 data[3];
    uint32 err = readSCD41(data);
    if (err) {
        Print("Error reading CO2\r\n");
    } else {
        Print("CO2: ");
        PrintInt(data[0]);
        Print(", Temp: ");
        PrintInt(data[1]);
        Print(", Humidity: ");
        PrintInt(data[2]);
        Print("\r\n");
    }
    return data[0];
}

int32 ReadSensorCH4() {
    // TODO
    return 0;
}

int32 ReadSensorO2() {
    // TODO
    return 0;
}

uint32 initializeSensors() {
    // writeReg0(SCD41_ADDR, SCD41_REG_wake_up);
    // writeReg0(SCD41_ADDR, SCD41_REG_stop_periodic_measurement); //Starting periodic sensor for CO2
    CyDelay(30);
    return writeReg0(SCD41_ADDR, SCD41_REG_start_periodic_measurement); //Starting periodic sensor for CO2
    // return 0;
}

// read 16 bytes from a 16 bit address
uint32 readReg16(uint8 addr, uint16 reg, uint16* val) {
    uint32 err;
    uint8 data[2];
    I2C_I2CMasterClearStatus(); //clear the garbage

	err = I2C_I2CMasterSendStart(addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        return err;
    }
	I2C_I2CMasterWriteByte(reg, TIMEOUT);
	I2C_I2CMasterSendStop(TIMEOUT);
	
	err = I2C_I2CMasterSendRestart(addr, I2C_I2C_READ_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        return err;
    }
	I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, data, TIMEOUT);
    I2C_I2CMasterReadByte(I2C_I2C_NAK_DATA, data+1, TIMEOUT);
    I2C_I2CMasterSendStop(TIMEOUT);
    
    *val = ((uint16) data[0] << 8) | data[1];
	return 0;
}

uint32 readRegN(uint8 addr, uint16 reg, uint8* data, uint8 cnt) {
    uint32 err;
    I2C_I2CMasterClearStatus(); //clear the garbage
    
	err = I2C_I2CMasterSendStart(addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        Print("Error with SendStart");
        PrintInt(err);
        return err;
    }
	I2C_I2CMasterWriteByte(reg >> 8, TIMEOUT);
    I2C_I2CMasterWriteByte(reg & 0xFF, TIMEOUT);
    // CyDelay(1);
    
	err = I2C_I2CMasterSendRestart(addr, I2C_I2C_READ_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        Print("Error with SendRestart");
        PrintInt(err);
        return err;
    }
    for (int i = 0; i < cnt-1; i++)
        I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, data+i, TIMEOUT);
    I2C_I2CMasterReadByte(I2C_I2C_NAK_DATA, data+cnt-1, TIMEOUT);
    I2C_I2CMasterSendStop(TIMEOUT);
    
	return 0;
}

uint32 readSCD41(int16 data[3]) {
    uint8 raw_data[9] = {};
    uint32 err = readRegN(SCD41_ADDR, SCD41_REG_read_measurement, raw_data, 9);
    if (err) return 1;
    
    uint8 crc_CO2 = sensirion_common_generate_crc(raw_data, 2);
    uint8 crc_temp = sensirion_common_generate_crc(raw_data, 2);
    uint8 crc_RH = sensirion_common_generate_crc(raw_data, 2);
    if (crc_CO2 != raw_data[2] | crc_temp != raw_data[5] | crc_RH != raw_data[8]) {
        Print("CRC mismatch\r\n");
        return 1;
    }
    
    data[0] = (int16) (raw_data);
    data[1] = (int16) (raw_data+3);
    data[2] = (int16) (raw_data+6);
    
	return 0;
}

// read 16 bytes from a 16 bit address, with crc
uint32 readReg16crc(uint8 addr, uint16 reg, uint16* val) {
    uint8 data[3];
    uint32 err;
    I2C_I2CMasterClearStatus(); //clear the garbage
    
    I2C_I2C_MSTR_ERR_LB_NAK;
    I2C_I2C_MSTR_NOT_READY;
    
	err = I2C_I2CMasterSendStart(addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        return err;
    }
	I2C_I2CMasterWriteByte(reg >> 8, TIMEOUT);
    I2C_I2CMasterWriteByte(reg & 0xFF, TIMEOUT);
    CyDelay(1);
    
    // err = I2C_I2CMasterWriteBuf(addr, (uint8*) &reg, 2, I2C_I2C_MODE_COMPLETE_XFER);
    // if (err) return err;
    // err = I2C_I2CMasterReadBuf(addr, data, 3, I2C_I2C_MODE_COMPLETE_XFER);
    // if (err) return err;
    
	err = I2C_I2CMasterSendRestart(addr, I2C_I2C_READ_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        return err;
    }
    I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, data, TIMEOUT);
	I2C_I2CMasterReadByte(I2C_I2C_ACK_DATA, data+1, TIMEOUT);
    I2C_I2CMasterReadByte(I2C_I2C_NAK_DATA, data+2, TIMEOUT);
    I2C_I2CMasterSendStop(TIMEOUT);
    
    uint8 crc = sensirion_common_generate_crc(data, 2);
    if (crc != data[2]) {
        Print("CRC mismatch, expected ");
        PrintInt(crc);
        Print(" got ");
        PrintInt(data[2]);
        Print("\r\n");
    }
    
    *val = ((uint16) data[0] << 8) | data[1];
	return 0;
}

// write no bytes to a register
uint32 writeReg0(uint8 addr, uint16 reg) {
    I2C_I2CMasterClearStatus(); //clear the garbage
    
    uint32 err = I2C_I2CMasterSendStart(addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
    if (err) {
        I2C_I2CMasterSendStop(TIMEOUT);
        return err;
    }
    I2C_I2CMasterWriteByte(reg >> 8, TIMEOUT);
    I2C_I2CMasterWriteByte(reg & 0xFF, TIMEOUT);
    I2C_I2CMasterSendStop(TIMEOUT);
    return 0;
}

uint32 writeReg16(uint8 addr, uint16 reg, uint16 val) {    
    uint8_t data[2] = {val >> 8, val & 0xFF};
    uint8_t crc = sensirion_common_generate_crc(data, 2);
    
    I2C_I2CMasterClearStatus(); //clear the garbage
    
    I2C_I2CMasterSendStart(addr, I2C_I2C_WRITE_XFER_MODE, TIMEOUT);
	I2C_I2CMasterWriteByte(reg >> 8, TIMEOUT);
    I2C_I2CMasterWriteByte(reg & 0xFF, TIMEOUT);
    
    // I2C_I2CMasterWriteBuf(addr, (uint8*) &reg, 16, I2C_I2C_MODE_COMPLETE_XFER);
    
    I2C_I2CMasterWriteByte(data[0], TIMEOUT);
    I2C_I2CMasterWriteByte(data[1], TIMEOUT);
    I2C_I2CMasterWriteByte(crc, TIMEOUT);
    
    // I2C_I2CMasterWriteBuf(addr, (uint8*) &reg, 16, I2C_I2C_MODE_COMPLETE_XFER);
    
    return I2C_I2CMasterSendStop(TIMEOUT);
}

#define CRC8_POLYNOMIAL 0x31
#define CRC8_INIT 0xff
uint8_t sensirion_common_generate_crc(const uint8_t* data, uint16_t count) {
    uint16_t current_byte;
    uint8_t crc = CRC8_INIT;
    uint8_t crc_bit;
    /* calculates 8-Bit checksum with given polynomial */
    for (current_byte = 0; current_byte < count; ++current_byte) {
    crc ^= (data[current_byte]);
    for (crc_bit = 8; crc_bit > 0; --crc_bit) {
        if (crc & 0x80)
            crc = (crc << 1) ^ CRC8_POLYNOMIAL;
        else
            crc = (crc << 1);
        }
    }
    return crc;
}
/* [] END OF FILE */
