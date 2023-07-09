# Chromatofore Filament Changer Library ![Build Status](https://github.com/rich-dobbs-13440//chromatofore_arduino_library

This is a library for a low cost filament changer for inexpensive 3d printers.  It is targeted at fully automated
swapping of filament at layers and between prints.  It is not expected to be robust enough for full multicolor 
prints.

The initial implementation is targeted at a Ender 3 V2 printer using OctoPi as the print server, and
will be tested using an Arduino Mega 2560 and an Arduino Uno R4 as the SBC.    

The system will be highly scalable, with small configurations using all available PWM pins on the dedicated
Arduino, and larger systems using I2C to communicate with multiplexer daughter boards and only
requiring 2 pins. At this time, the system will be tested with up to 9 filaments available for printing,
although there is no reason that the system couldn't scale to to much larger number of filaments.
The per filament cost is expected to be less than the cost of a roll of inexpensive filament. 

For the 3d printed parts for system, go to https://github.com/rich-dobbs-13440/chromatofore.  

For integrating Chromatofore with Octopi, the method is TBD.  

# Installation

Use the Arduino library manager to install the following libraries:

* Adafruit PWM Servo DriverLibrary

Download the code from https://github.com/rich-dobbs-13440/chromatofore_arduino_library using the "Download Zip" option".  

Install the library using the Sketch / Include Library / Add ZIP Library... menu choice in the Arduino IDE, then select 
the downloaded file.

## Usage

First, you will need to print out the parts as specified in https://github.com/rich-dobbs-13440//chromatofore. 
Assemble the Chromatore Controller using the Arduino, daughter boards, the 3d printed case and mount them on 
your printer using screws, nuts, and breadboard jumpers. 
Next, assemble the 3d parts into actuators using purchased servos, screws, and nuts. 
Attach the actuators to the controller.  Modify an example sketch to match your specific controller
and the actuator attachments, and upload it to your controller.

The integration of Chromatofore with Octopi and your slicer is TBD.



## API

The classes available in the library include:

```c
Chromatofore();  // The filament changer
EarwigFilamentActuator();  // A single filament actuator
IServo();  // An abstract interface for servos
ArduinoPWMServo(); // A concrete implementation for IServo for servo motors attatched directly to an Arduino  that uses Servo.h.
I2CServo(); // A concrete implementation that attach to daughter boards that uses Adafruit_PWMServoDriver.h

```

 










Written by Rich Dobbs.  Check license.txt for more information. 

All text above must be included in any redistribution.