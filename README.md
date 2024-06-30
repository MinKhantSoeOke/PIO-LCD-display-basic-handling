# General Purpose I/O and LCD Handling on Atmel AT91SAM7

**by**: Min Khant Soe Oke, Kaung Sithu

## Overview

This project demonstrates how to program the PIO controller embedded in the Atmel AT91SAM7 microcontroller and how to handle an LCD display on the SAM7-EX256 evaluation board. The exercise includes reading the state of a joystick and buttons connected to the PIO ports and displaying information on the LCD screen.

## Dependencies

**To run this project, you need to have the following dependencies installed:**

* Atmel AT91SAM7X256 microcontroller
* Olimex SAM7-EX256 evaluation board
* IAR Embedded Workbench
* Basic knowledge of C programming

## Features

**PIO Handling**
- Reading the state of the joystick and buttons connected to the PIO ports.
- Detecting rising and falling edges of input signals to determine user actions.

**LCD Display Handling**
- Configuring the LCD display for use with the microcontroller.
- Displaying alphanumeric characters and graphics on the LCD screen.

## How to Use

1. **Start the Main Program**:
   - Open a terminal and run `./main`.

2. **Read Joystick and Button States**:
   - The program reads the state of the joystick and buttons connected to the PIO ports and calculates the rising and falling edges.

3. **Update and Display Counter**:
   - The counter is updated based on joystick and button actions and displayed on the LCD screen.

4. **Implement Additional Features**:
   - Follow the exercise instructions to add more features, such as handling additional joystick actions and displaying counter limits.

## Authors

- Min Khant Soe Oke
- Kaung Sithu
