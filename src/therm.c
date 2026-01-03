#include "bme280_driver.h"
#include "main_config.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

volatile int keep_running = 1;

void handle_signal(int signal) {
	keep_running = 0;
}

int main(void) {
	
	printf("Initializing I2C bus...\n");
	int bus_fd = i2c_bus_init(I2C_BUS);
	if (bus_fd < 0) {
		printf("Failed to initialize I2C bus\n");
		return 1;
	}

	printf("Connecting to BME280 device...\n");
	i2c_device_t bme280_device;
	bme280_device.bus_fd = bus_fd;
	bme280_device.device_address = BME280_I2C_ADDR;
	
	int result = bme280_connect(&bme280_device);
	if (result < 0) {
		printf("Failed to connect to BME280 device. Error Code: %d\n", result);
		return 1;
	}

	printf("Configuring BME280 device...\n");
	bme280_calib_data_t calib_data;
	if (bme280_config(&bme280_device, &calib_data) < 0) {
		printf("Failed to configure BME280 device\n");
		return 1;
	}

	signal(SIGINT, handle_signal);


	while (keep_running) {
		printf("Reading sensor data...\n");
		bme280_data_t sensor_data;
		if (bme280_read_data(&bme280_device, &calib_data, &sensor_data) < 0) {
			printf("Failed to read data from BME280 device\n");
			return 1;
		}
		printf("Temperature: %.2f °C or %.2f °F, Humidity: %.2f %%\n", sensor_data.temperature, sensor_data.temperature_f, sensor_data.humidity);
		printf("Pressure: %.2f Pa\n", sensor_data.pressure);
		sleep(PRINTOUT_DELAY/1000);
	}


	bme280_sleep(&bme280_device);
	i2c_bus_close(bus_fd);
	printf("Exiting program.\n");

	return 0;
}
