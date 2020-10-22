# Pedometer Demo

Date: 10/22/2020
## Project Description:

This project is a step counter. It emulates a pedometer using the accelerometer.

## Setup
* Generate the Project using STM32CubeMX
* C/C++ Build>Settings>Tool Settings>Use Float with printf from newlib-nano (-u _printf_float)
* C/C++ General>Paths and Symbols>Includes>Add...
	>CommsDrivers/Inc
  
	>SensorDrivers/Inc
* C/C++ General>Paths and Symbols>Includes>Add Folder...
	>SensorDrivers
	>CommsDrivers
