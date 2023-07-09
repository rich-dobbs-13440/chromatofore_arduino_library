# Chromatofore Filament Changer Library ![Build Status](https://github.com/rich-dobbs-13440/chromatofore)

This is a library for a low cost filament changer for inexpensive 3d printers.  It is targeted at fully automated
swapping of filament at layers and between prints.  It is not expected to be robust enough for full multicolor 
prints.

The initial implementation is targeted at a Ender 3 V2 printer using Octopi as the print server, and
will be tested using an Arduino Mega 2560 and an Arduino Uno R4 as the SBC.    

The system will be highly scalable, with small configurations using all available PWM pins on the dedicated
Arduino, and larger systems using I2C to communicate with multiplexer daughter boards and only
requiring 2 pins. At this time, the system will be tested with up to 9 filaments available for printing,
although there is no reason that the system couldn't scale to to much larger number of filaments.
The per filament cost is expected to be less than the cost of a roll of filament. 

## Functionality

To use the Chromatofore library in an Arduino sketch, include Chromatofore.h.  For the initial
implementation, you should start with the example file, and modify it for your specific 
physical configuration.  

The classes available in the library include

```c
Chromatofore();  // The filament changer
EarwigFilamentActuator();  // A single filament actuator
IServo();  // An abstract interface for servos
StandardServo(); // A concrete implementation for IServo for servo motors attatched directly to an Arduino that uses Servo.h.
IC2Servo(); // A concrete implementation that attach to daughter boards that uses Adafruit_PWMServoDriver.h

```


# Installation

Use the Arduino library manager to install the following libraries:

Adafruit_PWM_Servo_Driver_Library

//#include <Wire.h>
//#include <Adafruit_PWMServoDriver.h>


Written by Rich Dobbs.  Check license.txt for more information. 

All text above must be included in any redistribution.