#ifndef I2C_DRIVER_H
#define I2C_DRIVER_H

#include <stdint.h>


typedef struct {
	int bus_fd;
	uint8_t device_address;
} i2c_device_t;


int i2c_bus_init(char *bus_name);
int i2c_connect_device(i2c_device_t *device);
int i2c_register_write(i2c_device_t *device, uint8_t reg, uint8_t value);
int i2c_register_read(i2c_device_t *device, uint8_t reg, uint8_t *buffer, int length);
int i2c_repeated_start_read(i2c_device_t *device, uint8_t reg, uint8_t *buffer, int length);
int i2c_bus_close(int bus_fd);

#endif
