# Assignment 1

Date: 10/04/2020


## Project Description:

Using different initialization conditions (using preprocessor directives), I change between the project for
Parts A, B, and C. I set the onboard LED to a PWM with a period of 10 ms using STM32CubeMX. I change
this value from an array for Parts A and B.

Part C represents an ignition button for a PWM-driven wheel motor (represented here by LED
brightness). When the button is pressed, the PWM gradually changes up to the max “speed” (PWM
value). When the button is pressed again, the PWM gradually reduces the motor’s speed (PWM value)
until the motor comes to a halt (at a value of 0).

## Part A
Goal: PWM with 10ms period at 50% DC.

## Part B
Goal: Make the LED change brightness using PWM going from a low to a high Duty Cycle. The LED mostly
started to turn on around a DC of 40-50, so I had the LED jump almost immediately to a visible value to
make decrease the user’s wait time.

## Part C
Goal: Emulate a motor that drives when a button is pressed and smoothly comes to a stop when the
button is pressed again. The motor’s speed is represented by the onboard LED’s brightness.

## Github Link
https://github.com/TTU-Jon/MPSystems/tree/master/Project1/Assignment1
