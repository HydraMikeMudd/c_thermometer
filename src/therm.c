#include "bme280_driver.h"
#include "main_config.h"
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <errno.h>

volatile int keep_running = 1;

void handle_signal(int signal) {
	(void)signal; // Suppress unused parameter warning
	keep_running = 0;
}

int main(void) {
	int rc = 0;
	int bus_fd = -1;
	i2c_device_t bme280_device = {0};
	bme280_calib_data_t calib_data;
	int device_connected = 0;
	
	printf("Initializing I2C bus...\n");
	bus_fd = i2c_bus_init(I2C_BUS);
	if (bus_fd < 0) {
		printf("Failed to initialize I2C bus\n");
		rc = 1;
		goto cleanup;
	}

	printf("Connecting to BME280 device...\n");
	bme280_device.bus_fd = bus_fd;
	bme280_device.device_address = 0x00; // Will be set by bme280_connect
	
	int result = bme280_connect(&bme280_device);
	if (result < 0) {
		printf("Failed to connect to BME280 device. Error Code: %d\n", result);
		rc = 1;
		goto cleanup;
	}
	device_connected = 1;

	printf("Configuring BME280 device...\n");
	if (bme280_config(&bme280_device, &calib_data) < 0) {
		printf("Failed to configure BME280 device\n");
		rc = 1;
		goto cleanup;
	}

	signal(SIGINT, handle_signal);

	while (keep_running) {
		printf("Reading sensor data...\n");
		bme280_data_t sensor_data;
		if (bme280_read_data(&bme280_device, &calib_data, &sensor_data) < 0) {
			printf("Failed to read data from BME280 device\n");
			rc = 1;
			goto cleanup;
		}
		float pressure_hpa = sensor_data.pressure / 100.0f;
		printf("Temperature: %.2f °C or %.2f °F, Humidity: %.2f %%\n", sensor_data.temperature, sensor_data.temperature_f, sensor_data.humidity);
		printf("Pressure: %.2f hPa\n", pressure_hpa);
		
		// Use nanosleep for millisecond precision
		struct timespec sleep_time, remaining;
		sleep_time.tv_sec = PRINTOUT_DELAY / 1000;
		sleep_time.tv_nsec = (PRINTOUT_DELAY % 1000) * 1000000L;
		
		// Handle interrupted sleep
		while (nanosleep(&sleep_time, &remaining) == -1) {
			if (errno != EINTR) {
				break; // Exit on errors other than interruption
			}
			sleep_time = remaining; // Continue sleeping with remaining time
		}
	}

cleanup:
	// Put sensor to sleep if it was connected
	if (device_connected) {
		bme280_sleep(&bme280_device);
	}
	
	// Close I2C bus if it was opened
	if (bus_fd >= 0) {
		i2c_bus_close(bus_fd);
	}
	
	printf("Exiting program.\n");
	return rc;
}
