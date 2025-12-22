#include "bme280_driver.h"
#include <time.h>


// Initialize the I2C device and reset BME280 sensor
int bme280_connect(i2c_device_t *device) {
	// Initialize I2C device
	if (i2c_init_device(device) != 0) {
		return -1;
	}

	// Reset BME280 sensor
	if (i2c_register_write(device, BME280_RESET_REG, BME280_RESET_CMD) != 0) {
		return -1;
	}

	// Sleep for 5 ms to allow sensor to reset
	struct timespec req = {0};
	req.tv_sec = 0;
	req.tv_nsec = 5 * 1000000L; // 5 ms
	nanosleep(&req, NULL);

	return 0;
}

// Read temp, pressure, and first humidity calibration block
int bme280_read_block_1_calib(i2c_device_t *device, bme280_calib_data_t *calib) {
	uint8_t buffer[BME280_BLOCK_1_CALIB_LENGTH];
	if (i2c_register_read(device, BME280_BLOCK_1_CALIB_START, buffer, BME280_BLOCK_1_CALIB_LENGTH) != 0) {
		return -1;
	}

	calib->dig_T1 = (uint16_t)(buffer[1] << 8 | buffer[0]);
	calib->dig_T2 = (int16_t)(buffer[3] << 8 | buffer[2]);
	calib->dig_T3 = (int16_t)(buffer[5] << 8 | buffer[4]);
	calib->dig_P1 = (uint16_t)(buffer[7] << 8 | buffer[6]);
	calib->dig_P2 = (int16_t)(buffer[9] << 8 | buffer[8]);
	calib->dig_P3 = (int16_t)(buffer[11] << 8 | buffer[10]);
	calib->dig_P4 = (int16_t)(buffer[13] << 8 | buffer[12]);
	calib->dig_P5 = (int16_t)(buffer[15] << 8 | buffer[14]);
	calib->dig_P6 = (int16_t)(buffer[17] << 8 | buffer[16]);
	calib->dig_P7 = (int16_t)(buffer[19] << 8 | buffer[18]);
	calib->dig_P8 = (int16_t)(buffer[21] << 8 | buffer[20]);
	calib->dig_P9 = (int16_t)(buffer[23] << 8 | buffer[22]);
	calib->dig_H1 = buffer[25];

	return 0;
}

// Read second humidity calibration block
int bme280_read_block_2_calib(i2c_device_t *device, bme280_calib_data_t *calib) {
	uint8_t buffer[BME280_BLOCK_2_CALIB_LENGTH];
	if (i2c_register_read(device, BME280_BLOCK_2_CALIB_START, buffer, BME280_BLOCK_2_CALIB_LENGTH) != 0) {
		return -1;
	}

	calib->dig_H2 = (int16_t)(buffer[1] << 8 | buffer[0]);
	calib->dig_H3 = buffer[2];
	calib->dig_H4 = (int16_t)((buffer[3] << 4) | (buffer[4] & 0x0F));
	calib->dig_H5 = (int16_t)((buffer[5] << 4) | (buffer[4] >> 4));
	calib->dig_H6 = (int8_t)buffer[6];

	return 0;
}

// Configure BME280 sensor with desired settings and read calibration data
int bme280_config(i2c_device_t *device, bme280_calib_data_t *calib) {
	// Getting calibration data
	if (bme280_read_block_1_calib(device, calib) != 0) {
		return -1;
	}

	if (bme280_read_block_2_calib(device, calib) != 0) {
		return -1;
	}

	// Configuring sensor
	if (i2c_register_write(device, BME280_CTRL_HUM_REG, BME280_CTRL_HUM_VAL) != 0) {
		return -1;
	}

	if (i2c_register_write(device, BME280_CTRL_MEAS_REG, BME280_CTRL_MEAS_VAL) != 0) {
		return -1;
	}

	if (i2c_register_write(device, BME280_CONFIG_REG, BME280_CONFIG_VAL) != 0) {
		return -1;
	}

	return 0;
}


// Returns temperature in °C * 100
int32_t bme280_compensate_temp(int32_t adc_T, bme280_calib_data_t *calib, int32_t *t_fine) {
	int32_t var1, var2, T;
	var1 = ((((adc_T >> 3) - ((int32_t)calib->dig_T1 << 1))) * ((int32_t)calib->dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((int32_t)calib->dig_T1)) * ((adc_T >> 4) - ((int32_t)calib->dig_T1))) >> 12) * ((int32_t)calib->dig_T3)) >> 14;
	(*t_fine) = var1 + var2;
	T = ((*t_fine) * 5 + 128) >> 8;
	return T;
}

// Returns pressure in Pa * 256
uint32_t bme280_compensate_press(int32_t adc_P, bme280_calib_data_t *calib, int32_t t_fine) {
	int64_t var1, var2, p;
	var1 = ((int64_t)t_fine) - 128000;
	var2 = var1 * var1 * (int64_t)calib->dig_P6;
	var2 = var2 + ((var1 * (int64_t)calib->dig_P5) << 17);
	var2 = var2 + (((int64_t)calib->dig_P4) << 35);
	var1 = ((var1 * var1 * (int64_t)calib->dig_P3) >> 8) + ((var1 * (int64_t)calib->dig_P2) << 12);
	var1 = (((((int64_t)1) << 47) + var1)) * ((int64_t)calib->dig_P1) >> 33;

	if (var1 == 0) {
		return 0; // avoid exception caused by division by zero
	}

	p = 1048576 - adc_P;
	p = (((p << 31) - var2) * 3125) / var1;
	var1 = (((int64_t)calib->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
	var2 = (((int64_t)calib->dig_P8) * p) >> 19;

	p = ((p + var1 + var2) >> 8) + (((int64_t)calib->dig_P7) << 4);
	return (uint32_t)p;
}

// Returns humidity in %RH * 1024
uint32_t bme280_compensate_hum(int32_t adc_H, bme280_calib_data_t *calib, int32_t t_fine) {
	int32_t v_x1_u32r;
	v_x1_u32r = (t_fine - ((int32_t)76800));
	v_x1_u32r = (((((adc_H << 14) - (((int32_t)calib->dig_H4) << 20) - (((int32_t)calib->dig_H5) * v_x1_u32r)) + ((int32_t)16384)) >> 15) * (((((((v_x1_u32r * ((int32_t)calib->dig_H6)) >> 10) * (((v_x1_u32r * ((int32_t)calib->dig_H3)) >> 11) + ((int32_t)32768))) >> 10) + ((int32_t)2097152)) * ((int32_t)calib->dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((int32_t)calib->dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	return (uint32_t)(v_x1_u32r >> 12);
}

// Convert Celsius to Fahrenheit
float bme280_c_to_f(float c) {
	return (c * 9.0f / 5.0f) + 32.0f;
}

// Read raw data from BME280 and apply compensation formulas, adding results to data struct
int bme280_read_data(i2c_device_t *device, bme280_calib_data_t *calib, bme280_data_t *data) {
	uint8_t buffer[BME280_REG_DATA_LENGTH];
	if (i2c_register_read(device, BME280_REG_DATA_START, buffer, BME280_REG_DATA_LENGTH) != 0) {
		return -1;
	}

	int32_t adc_P = (int32_t)((((uint32_t)(buffer[0]) << 12) | ((uint32_t)(buffer[1]) << 4) | ((uint32_t)(buffer[2]) >> 4)));
	int32_t adc_T = (int32_t)((((uint32_t)(buffer[3]) << 12) | ((uint32_t)(buffer[4]) << 4) | ((uint32_t)(buffer[5]) >> 4)));
	int32_t adc_H = (int32_t)(((uint32_t)(buffer[6]) << 8) | ((uint32_t)(buffer[7])));

	int32_t t_fine;
	int32_t temp = bme280_compensate_temp(adc_T, calib, &t_fine);
	uint32_t press = bme280_compensate_press(adc_P, calib, t_fine);
	uint32_t hum = bme280_compensate_hum(adc_H, calib, t_fine);

	data->temperature = temp / 100.0f;
	data->pressure = press / 256.0f;
	data->humidity = hum / 1024.0f;
	data->temperature_f = bme280_c_to_f(data->temperature);

	return 0;
}

// Put BME280 sensor into sleep mode
int bme280_sleep(i2c_device_t *device) {
	// Set sensor to sleep mode by writing 0x00 to CTRL_MEAS register
	if (i2c_register_write(device, BME280_REG_CTRL_MEAS, 0x00) != 0) {
		return -1;
	}

	return 0;
}
