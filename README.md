# c_thermometer

## Overview

This repo is for a small project to create a thermostat using some hobbiest parts. It will include part information, setup instructions, and code for application/interfacing with hardware.

## Parts

* Raspberry Pi Zero 2 with Header (including accesories like power supply, SD card, case, etc.)
* Inland 2.13 Inch E-Ink LCD Display Screen
* Inland DHT11 Temperature and Humidity Sensor
* Female-to-Female Jumper Wires

## E-Ink LCD Setup

This part is a clone of a similar Waveshare e-Paper HAT. Thus, to set up the device, the following wiki instructions can be followed: https://www.waveshare.com/wiki/2.13inch_e-Paper_HAT_Manual. Important note: the SDI Pin is the same the DIN pin on the documentation. The rest of the pins should have similar naming.

For the Raspiberry Pi pin out diagram, see the following: https://pinout.xyz/

## DHT11 Hardware Setup

The following pin connections should be sufficient:

- V -> 5v Power (Pin 2 or 4 on Pi)
- G -> Ground (Pin 6 on Pi)
- S -> GPIO 4 (Pin 7 on Pi)

## Resources

DHT11 Datasheet: https://www.mouser.com/datasheet/2/758/DHT11-Technical-Data-Sheet-Translated-Version-1143054.pdf

## TODO

New dht11.h driver does read the GPIO correctly. However, timing is off. Sometimes, I am able to get a reading on initialization of script but uncommon. dht11_read_response needs updating to more reliable method. Verified sensor worked with python script.

## Notes

Added code to main function to rise the process priority to highest level. This seemed to help some but only for first iteration normally. Changing the wait period between polling didn't seem to help. A larger threshold for 1 seems to work most consistently so far.
