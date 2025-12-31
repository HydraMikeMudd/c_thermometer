#ifndef BME280_DRIVER_H
#define BME280_DRIVER_H

#include "i2c_protocol.h"

#define BME280_I2C_ADDR 0x77
#define BME280_RESET_REG 0xE0
#define BME280_RESET_CMD 0xB6
#define BME280_CTRL_HUM_REG 0xF2
#define BME280_CTRL_HUM_VAL 0x01  // Humidity oversampling x1
#define BME280_CTRL_MEAS_REG 0xF4
#define BME280_CTRL_MEAS_VAL 0x57 // Temp x2, Pressure x16, Normal mode
#define BME280_CONFIG_REG 0xF5
#define BME280_CONFIG_VAL 0x90 // Standby 500ms, IIR Filter to 16
#define BME280_BLOCK_1_CALIB_START 0x88
#define BME280_BLOCK_1_CALIB_LENGTH 26
#define BME280_BLOCK_2_CALIB_START 0xE1
#define BME280_BLOCK_2_CALIB_LENGTH 7
#define BME280_REG_CTRL_HUM 0xF2
#define BME280_REG_CTRL_MEAS 0xF4
#define BME280_REG_CONFIG 0xF5
#define BME280_REG_DATA_START 0xF7
#define BME280_REG_DATA_LENGTH 8


typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2, dig_T3;
    uint16_t dig_P1;
    int16_t  dig_P2, dig_P3, dig_P4, dig_P5, dig_P6, dig_P7, dig_P8, dig_P9;
    uint8_t  dig_H1;
    int16_t  dig_H2;
    uint8_t  dig_H3;
    int16_t  dig_H4, dig_H5;
    int8_t   dig_H6;
} bme280_calib_data_t;

typedef struct {
	float temperature;
	float pressure;
	float humidity;
	float temperature_f;
} bme280_data_t;


int bme280_connect(i2c_device_t *device);
int bme280_read_block_1_calib(i2c_device_t *device, bme280_calib_data_t *calib);
int bme280_read_block_2_calib(i2c_device_t *device, bme280_calib_data_t *calib);
int bme280_config(i2c_device_t *device, bme280_calib_data_t *calib);
int32_t bme280_compensate_temp(int32_t adc_T, bme280_calib_data_t *calib, int32_t *t_fine);
uint32_t bme280_compensate_press(int32_t adc_P, bme280_calib_data_t *calib, int32_t t_fine);
uint32_t bme280_compensate_hum(int32_t adc_H, bme280_calib_data_t *calib, int32_t t_fine);
float bme280_c_to_f(float c);
int bme280_read_data(i2c_device_t *device, bme280_calib_data_t *calib, bme280_data_t *data);
int bme280_sleep(i2c_device_t *device);

#endif
