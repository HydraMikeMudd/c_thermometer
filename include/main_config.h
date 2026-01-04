#ifndef MAIN_CONFIG_H
#define MAIN_CONFIG_H

// User Settings
#define PRINTOUT_DELAY 2000 // Delay between printouts in milliseconds
#define I2C_BUS "/dev/i2c-1" // I2C bus device
#define TEXT_X_START 20 // X coordinate for text start in pixels
#define TEXT_Y_START 20 // Y coordinate for text start in pixels
#define ITERATION_TIMEOUT 7200 // Number of iterations before timeout. Each iteration is PRINTOUT_DELAY milliseconds

// BME280 I2C Address Override
// Set to -1 for auto-detection (tries 0x76 first, then 0x77)
// Set to 0x76 or 0x77 to force a specific address
#define BME280_ADDR_OVERRIDE -1

#endif
