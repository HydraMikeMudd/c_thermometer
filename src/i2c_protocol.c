#include "i2c_protocol.h"
#include <stddef.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <linux/i2c-dev.h>


// Initialize the I2C bus and return the file descriptor
int i2c_bus_init(char *bus_name) {
	int bus_fd = open(bus_name, O_RDWR);
	return bus_fd;
}


// Calls ioctl to connect to talk to device, returns 0 if success, -1 if failure,
// and -2 if null device config provided
int i2c_connect_device(i2c_device_t *device) {
	if (device == NULL) {
		return -2;
	}

	if (ioctl(device->bus_fd, I2C_SLAVE, device->device_address) < 0) {
		return -1;
	}

	return 0;
}


// Write value to specified reg. If success, returns 0. If failure while writing, returns -1.
// If empty device config provided, returns -2
int i2c_register_write(i2c_device_t *device, uint8_t reg, uint8_t value) {
	if (device == NULL) {
		return -2;
	}

	uint8_t buffer[2];
	buffer[0] = reg;
	buffer[1] = value;
	
	int result = write(device->bus_fd, buffer, 2); 	
	if (result != 2) {
		printf("Write Failure Code: %d\n", errno);
		return -1;
	}

	return 0;
}


// Read from specified reg into provided buffer. If success, returns 0. If failure during specifying reg
// or during read of data, returns -1. If device or buffer are null, returns -2.
int i2c_register_read(i2c_device_t *device, uint8_t reg, uint8_t *buffer, int length) {
	if (device == NULL || buffer == NULL) {
		return -2;
	}

	// Tell reg we want to read
	if (write(device->bus_fd, &reg, 1) != 1) {
		return -1;
	}
	
	// Read data
	if (read(device->bus_fd, buffer, length) != length) {
		return -1;
	}

	return 0;
}


int i2c_repeated_start_read(i2c_device_t *device, uint8_t reg, uint8_t *buffer, int length) {
	if (device == NULL || buffer == NULL) {
		return -2;
	}
	// TODO
	return -1;
}


int i2c_bus_close(int bus_fd) {
	close(bus_fd);

	return 0;
}
