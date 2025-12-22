# c_thermometer

## Overview

This repo is for a small project to create a thermostat using some hobbiest parts. It will include part information, setup instructions, and code for application/interfacing with hardware.

## Parts

* Raspberry Pi Zero 2 with Header (including accesories like power supply, SD card, case, etc.)
* Inland 2.13 Inch E-Ink LCD Display Screen
~~* Inland DHT11 Temperature and Humidity Sensor~~
* SparkFun BME280 Temperature, Humidity, and Pressure Sensor
* Female-to-Female Jumper Wires

## E-Ink LCD Setup

This part is a clone of a similar Waveshare e-Paper HAT. Thus, to set up the device, the following wiki instructions can be followed: https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_Manual. Important note: the SDI Pin is the same the DIN pin on the documentation. The rest of the pins should have similar naming.

For the Raspiberry Pi pin out diagram, see the following: https://pinout.xyz/

~~## DHT11 Hardware Setup~~

~~The following pin connections should be sufficient:~~

~~- V -> 5v Power (Pin 2 or 4 on Pi)
- G -> Ground (Pin 6 on Pi)
- S -> GPIO 4 (Pin 7 on Pi)~~

## Resources

~~DHT11 Datasheet: https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf~~
BME280 Datasheet: https://www.bosch-sensortec.com/media/boschsensortec/downloads/datasheets/bst-bme280-ds002.pdf

## TODO

~~New dht11.h driver does read the GPIO correctly. However, timing is off. Sometimes, I am able to get a reading on initialization of script but uncommon. dht11_read_response needs updating to more reliable method. Verified sensor worked with python script.~~
Due to DHT11 is unreliable on Linux OS, implementing driver for BME280 sensor instead. BME280 uses I2C protocol which is more reliable on Raspberry Pi. I have made some initial implementation of the i2c protocol and the bme280 driver. Need to test driver once I have access to hardware again. Also, need to implement the SPI protocol for the e-ink display and implement the e-ink driver.

## Notes

The config used for the bme280 is the following:
- Temperature oversampling x2
- Humidity oversampling x1
- Pressure oversampling x16
- Normal mode
- Standby time 500 ms
- IIR filter on, coefficient 16
