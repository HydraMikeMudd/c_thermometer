# c_thermometer

## Overview

This repo is for a small project to create a thermometer using some hobbiest parts. It will include part information, setup instructions, and code for application/interfacing with hardware. There are two ways to run this project: with the specified E-Ink display or only the BME280 sensor to standard output.


## Parts

* Raspberry Pi Zero 2 with Header (including accesories like power supply, SD card, case, etc.)
* Inland 2.13 Inch E-Ink LCD Display Screen
* SparkFun BME280 Temperature, Humidity, and Pressure Sensor
* Female-to-Female Jumper Wires


## E-Ink LCD Setup

This part is a clone of a similar Waveshare e-Paper HAT. Thus, to set up the device, the following wiki instructions can be followed: https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_Manual. Important note: the SDI Pin is the same the DIN pin on the documentation. The rest of the pins should have similar naming.

For the Raspberry Pi pin out diagram, see the following: https://pinout.xyz/


## BME280 Setup

| BME280 Pin | Controller Pin | Description |
| :--- | :--- | :--- |
| VCC | [3.3V / Pin 1] | Power |
| GND | [GND / Pin 6] | Ground |
| SDA | [GPIO 2 / Pin 3] | I2C Data |
| SCL | [GPIO 3 / Pin 5] | I2C Clock |


For the BME280 to work, raspi-config will need to be updated to enable I2C through the Interfacing Options. After rebooting, install i2c-tools and verify the I2C address for the BME280 matches the one specified in the bme280_driver.h file.


The config used for the BME280 is the following:
- Temperature oversampling x2
- Humidity oversampling x1
- Pressure oversampling x16
- Normal mode
- Standby time 500 ms
- IIR filter on, coefficient 16

For more config recommendations, see the BME280 datasheet.


## Software Prerequisites

Before building, ensure you have the following installed:

* **C Compiler**: `gcc` or your specific cross-compiler.
* **Libraries**:
    * gpiod (If using the E-Ink display)
    * BCM2835 (If using the E-Ink display)
    * I2C/SPI tools (if using Linux: `i2c-tools`)


## How to Build

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/HydraMikeMudd/c_thermometer.git
    cd c_thermometer
    ```

2.  **Compile the code:**
    ```bash
    make TARGET=therm_with_display
    # OR if you don't want to complile with display functionality:
    # make
    ```

3.  **Run the application:**
    ```bash
    ./c_therm
    ```

## Configuration

You can adjust the delay or timeout behavior of the thermometer by modifying values defined at top of therm.c or therm_with_display.c files:

* **`PRINTOUT_DELAY`**: Time in milliseconds between screen refreshes (default: `2000`).
* **`I2C_BUS`**: Default BME280 bus is /dev/i2c-1. If different pins were used, modify this value.
* **`ITERATION_TIMEOUT`**: Default timeout after 7200 iterations of refreshes.

For modifying the default BME280 config settings, see the bme280_driver.h file.


## Additional Resources

BME280 Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf


## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
