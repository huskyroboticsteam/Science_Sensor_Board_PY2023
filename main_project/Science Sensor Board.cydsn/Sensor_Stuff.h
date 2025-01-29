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

int32 ReadSensorTemperature();
int32 ReadSensorHumidity();
int32 ReadSensorCO();
int32 ReadSensorCO2();
int32 ReadSensorCH4();
int32 ReadSensorO2();

uint32 initializeSensors();

uint32 readReg16(uint8 addr, uint16 reg, uint16* val);
uint32 readReg16crc(uint8 addr, uint16 reg, uint16* val);
uint32 writeReg0(uint8 addr, uint16 reg);
uint32 writeReg16(uint8 addr, uint16 reg, uint16 val);

uint8_t sensirion_common_generate_crc(const uint8_t* data, uint16_t count);

uint32 readSCD41(int16 data[3]);

#define TIMEOUT 20

#define SCD41_ADDR 0x62

#define SCD41_REG_start_periodic_measurement 0x21b1
#define SCD41_REG_read_measurement 0xec05
#define SCD41_REG_stop_periodic_measurement 0x3f86

#define SCD41_REG_set_temperature_offset 0x241d
#define SCD41_REG_get_temperature_offset 0x2318
#define SCD41_REG_set_sensor_altitude 0x2427
#define SCD41_REG_get_sensor_altitude 0x2322
#define SCD41_REG_set_ambient_pressure 0xe000
#define SCD41_REG_get_ambient_pressure 0xe000

#define SCD41_REG_perform_forced_recalibration 0x362f
#define SCD41_REG_set_automatic_self_calibration_enabled 0x2416
#define SCD41_REG_get_automatic_self_calibration_enabled 0x2313

#define SCD41_REG_start_low_power_periodic_measurement 0x21ac
#define SCD41_REG_get_data_ready_status 0xe4b8

#define SCD41_REG_persist_settings 0x3615
#define SCD41_REG_get_serial_number 0x3682
#define SCD41_REG_perform_self_test 0x3639
#define SCD41_REG_perform_factory_reset 0x3632
#define SCD41_REG_reinit 0x3646

#define SCD41_REG_measure_single_shot 0x219d
#define SCD41_REG_measure_single_shot_rht_only 0x2196
#define SCD41_REG_power_down 0x36e0
#define SCD41_REG_wake_up 0x36f6
#define SCD41_REG_set_automatic_self_calibration_initial_period 0x2445
#define SCD41_REG_get_automatic_self_calibration_initial_period 0x2340
#define SCD41_REG_set_automatic_self_calibration_standard_period 0x244e
#define SCD41_REG_get_automatic_self_calibration_standard_period 0x234b

/* [] END OF FILE */
