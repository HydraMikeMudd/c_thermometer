#include "bme280_driver.h"
#include "main_config.h"
#include "DEV_Config.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"
#include "Debug.h"
#include <stdlib.h>
#include "EPD_2in13_V4.h"
#include <time.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>


volatile int keep_running = 1;

void handle_signal(int signal) {
	(void)signal; // Suppress unused parameter warning
	keep_running = 0;
}


int main(void)
{
	int rc = 0;
	int bus_fd = -1;
	UBYTE *BlackImage = NULL;
	i2c_device_t bme280_device = {0};
	bme280_calib_data_t calib_data;
	int device_connected = 0;
	int epd_initialized = 0;
	int module_initialized = 0;
	
	// E-Ink Initialization
    Debug("EPD_2in13 Thermometer\r\n");
    if(DEV_Module_Init()!=0){
        rc = 1;
        goto cleanup;
    }
	module_initialized = 1;

    Debug("e-Paper Init and Clear...\r\n");
	EPD_2in13_V4_Init();
	epd_initialized = 1;

	struct timespec start={0,0}, finish={0,0}; 
    clock_gettime(CLOCK_REALTIME,&start);
    EPD_2in13_V4_Clear();
	clock_gettime(CLOCK_REALTIME,&finish);
    Debug("%ld S\r\n",finish.tv_sec-start.tv_sec);	

    UWORD Imagesize = ((EPD_2in13_V4_WIDTH % 8 == 0)? (EPD_2in13_V4_WIDTH / 8 ): (EPD_2in13_V4_WIDTH / 8 + 1)) * EPD_2in13_V4_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        Debug("Failed to apply for black memory...\r\n");
        rc = 1;
        goto cleanup;
    }
    Debug("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);
	Paint_Clear(WHITE);

	// BME280 Initialization
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

	// -- MAIN ITERATION -- 
	Paint_NewImage(BlackImage, EPD_2in13_V4_WIDTH, EPD_2in13_V4_HEIGHT, 90, WHITE);  
    Paint_SelectImage(BlackImage);

	// Writing static text
    Paint_DrawString_EN(TEXT_X_START, TEXT_Y_START, "Temp: ", &Font24, BLACK, WHITE);
    Paint_DrawString_EN(TEXT_X_START, (2 * TEXT_Y_START) + Font24.Height, "Hum:  ", &Font24, BLACK, WHITE);	
    Paint_DrawString_EN(TEXT_X_START + (Font24.Width * 11), TEXT_Y_START, " F", &Font24, BLACK, WHITE);
    Paint_DrawString_EN(TEXT_X_START + (Font24.Width * 11), (2 * TEXT_Y_START) + Font24.Height, " %", &Font24, BLACK, WHITE);

    int counter = 0;

	char temp_buffer[10]; // Increased buffer size for edge values
	char humidity_buffer[10]; // Increased buffer size for edge values

	// Clearing only number values and writing new sensor values
	while (keep_running && counter < ITERATION_TIMEOUT) {
        Paint_ClearWindows(TEXT_X_START + (Font24.Width * 6), TEXT_Y_START, TEXT_X_START + (Font24.Width * 11), (2 * TEXT_Y_START) + (Font24.Height * 2), WHITE);
		
		// Getting sensor data
		bme280_data_t sensor_data;
		if (bme280_read_data(&bme280_device, &calib_data, &sensor_data) < 0) {
			printf("Failed to read data from BME280 device\n");
			rc = 1;
			goto cleanup;
		}
		
		float pressure_hpa = sensor_data.pressure / 100.0f;
		printf("Temperature: %.2f °F, Humidity: %.2f %%, Pressure: %.2f hPa\n", sensor_data.temperature_f, sensor_data.humidity, pressure_hpa);
		
    	snprintf(temp_buffer, sizeof(temp_buffer), "%.2f", sensor_data.temperature_f);
    	snprintf(humidity_buffer, sizeof(humidity_buffer), "%.2f", sensor_data.humidity);

    	Paint_DrawString_EN(TEXT_X_START + (Font24.Width * 6), TEXT_Y_START, temp_buffer, &Font24, BLACK, WHITE);
    	Paint_DrawString_EN(TEXT_X_START + (Font24.Width * 6), (2 * TEXT_Y_START) + Font24.Height, humidity_buffer, &Font24, BLACK, WHITE);

		counter++;
		EPD_2in13_V4_Display_Partial(BlackImage);
        DEV_Delay_ms(PRINTOUT_DELAY);
    }

cleanup:
	// CLEANING UP
	if (epd_initialized) {
		Debug("Clear...\r\n");
		EPD_2in13_V4_Init();
		EPD_2in13_V4_Clear();
		
		Debug("Goto Sleep...\r\n");
		EPD_2in13_V4_Sleep();
		DEV_Delay_ms(2000);//important, at least 2s
	}
	
	if (BlackImage != NULL) {
		free(BlackImage);
		BlackImage = NULL;
	}
	
	if (module_initialized) {
		// close 5V
		Debug("close 5V, Module enters 0 power consumption ...\r\n");
		DEV_Module_Exit();
	}

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