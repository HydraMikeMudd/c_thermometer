# c_thermometer

<img src="images/setup.jpg" alt="Thermometer Setup" width="400"/>

## Overview

This repo is for a small project to create a thermometer using some hobbiest parts. It will include part information, setup instructions, and code for application/interfacing with hardware. There are two ways to run this project: with the specified E-Ink display or only the BME280 sensor to standard output.

**Dependencies:**
- **Headless mode** (sensor only): Requires only I2C support, no additional libraries needed.
- **Display mode**: Requires the Waveshare E-Paper driver and lgpio library for GPIO control.


## Parts

* Raspberry Pi Zero 2 with Header (including accessories like power supply, SD card, case, etc.)
* Inland 2.13 Inch E-Ink LCD Display Screen
* SparkFun BME280 Temperature, Humidity, and Pressure Sensor
* Female-to-Female Jumper Wires


## E-Ink LCD Setup

This part is a clone of a similar Waveshare e-Paper HAT. Thus, to set up the device, the following wiki instructions can be followed: https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_Manual. Important note: the SDI Pin is the same the DIN pin on the documentation. The rest of the pins should have similar naming.

For the Raspberry Pi pin out diagram, see the following: https://pinout.xyz/


## BME280 Setup

| BME280 Pin | Controller Pin | Description |
| :--- | :--- | :--- |
| VCC | 3.3V (Pin 1) | Power |
| GND | GND (Pin 6) | Ground |
| SDA | GPIO 2 (Pin 3) | I2C Data |
| SCL | GPIO 3 (Pin 5) | I2C Clock |


For the BME280 to work, raspi-config will need to be updated to enable I2C through the Interfacing Options. After rebooting, install i2c-tools and verify the I2C address for the BME280.

**BME280 Address Detection:**
The BME280 sensor can have two possible I2C addresses: 0x76 or 0x77. By default, the software will auto-detect the correct address by trying 0x76 first, then 0x77. If you know your sensor's specific address, you can set `BME280_ADDR_OVERRIDE` in `include/main_config.h` to either 0x76 or 0x77 to skip auto-detection. Set it to -1 to enable auto-detection.

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

**Example installation commands:**

```bash
sudo apt-get update
sudo apt install gpiod libgpiod-dev i2c-tools

# For BCM2835 library
wget http://www.airspayce.com/mikem/bcm2835/bcm2835-1.71.tar.gz
tar zxvf bcm2835-1.71.tar.gz
cd bcm2835-1.71/
sudo ./configure && sudo make && sudo make check && sudo make install
# For more information, please refer to the official website: http://www.airspayce.com/mikem/bcm2835/
```


## How to Build

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/HydraMikeMudd/c_thermometer.git
    cd c_thermometer
    ```

2.  **Compile the code:**
    ```bash
    make display
    # OR if you don't want to compile with display functionality:
    make headless
    ```

3.  **Run the application:**
    ```bash
    ./c_therm
    ```

Building for headless mode will output temperature (°C/°F), humidity (%), and pressure (hPa) readings to the terminal instead of the E-Ink display. Unlike the display mode, headless mode does not require the gpiod or BCM2835 libraries as it uses the Linux I2C interface directly.

**Note:** The application includes proper resource cleanup and error handling. If sensor initialization or communication fails, all allocated resources will be properly released before the program exits.

## Configuration

You can adjust the delay or timeout behavior of the thermometer by modifying values defined main_config.h file:

* **`PRINTOUT_DELAY`**: Time in milliseconds between screen refreshes (default: `2000`). In headless mode, this uses nanosleep for millisecond-accurate delays.
* **`I2C_BUS`**: Default BME280 bus is /dev/i2c-1. If different pins were used, modify this value.
* **`ITERATION_TIMEOUT`**: Default timeout after 7200 iterations of refreshes. Each refresh runs for approximately PRINTOUT_DELAY milliseconds.
* **`BME280_ADDR_OVERRIDE`**: Set to -1 for auto-detection (tries 0x76 first, then 0x77), or set to 0x76/0x77 to force a specific I2C address.

For modifying the default BME280 config settings, see the bme280_driver.h file.

**Pressure Units:** The sensor outputs pressure in hectopascals (hPa), which is equivalent to millibars (mbar). Standard atmospheric pressure at sea level is approximately 1013.25 hPa.


## Additional Resources

- BME280 Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf
- e-Paper Driver is copied from Waveshare's Repo: https://github.com/waveshareteam/e-Paper

## 📄 License

Distributed under the MIT License. See `LICENSE` for more information.
