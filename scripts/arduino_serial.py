#!/usr/bin/env python3

"""
Arduino Serial Communication Script

Usage:
python arduino_serial.py <port> <baud_rate> <command> <timeout>

Example:
python3 arduino_serial.py "/dev/ttyACM0" 9600 5 "G10 T3 L0 B11 C12 X40 ; Remember minimum values for the B, C, and X axis angles for tool 3" 

This script allows you to communicate with an Arduino board over a serial connection. It sends a command to the Arduino and receives and displays the response.

Arguments:
- <port>: The serial port where the Arduino is connected (e.g., COM3, /dev/ttyUSB0).
- <baud_rate>: The baud rate for the serial communication.
- <timeout>: The maximum time (in seconds) to wait for a response.
- <command>: The GCODE command to send to the Arduino.


Example:
- This example sends the command "G10 T3 L0 B11 C12 X40" to an Arduino connected to COM3 with a baud rate of 9600.
- It waits for a response for a maximum of 5 seconds.

"""

import serial
import time
import argparse

def communicate_with_arduino(port, baud_rate, command, timeout):
    try:
        with serial.Serial(port, baud_rate, timeout=timeout) as ser:

            # Send the command to the Arduino
            ser.write(command.encode())
            ser.write("\n".encode())

            # Read and display the response until timeout or interrupted by the user
            start_time = time.time()
            while True:
                if ser.in_waiting > 0:
                    response = ser.readline().decode().strip()
                    print("Received:", response)

                elapsed_time = time.time() - start_time
                if elapsed_time >= timeout:
                    break

    except serial.SerialException as e:
        print("Failed to open serial port:", port)
        print("Make sure the Arduino IDE is closed and the port is correct.")
    except Exception as e:
        print("An error occurred:", str(e))

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Serial communication with Arduino")
    parser.add_argument("port", type=str, help="Serial port")
    parser.add_argument("baud_rate", type=int, help="Baud rate")
    parser.add_argument("timeout", type=float, help="Timeout (in seconds)")    
    parser.add_argument("command", type=str, help="Command to send")


    args = parser.parse_args()

    try:
        communicate_with_arduino(args.port, args.baud_rate, args.command, args.timeout)
    except KeyboardInterrupt:
        print("\nProgram interrupted by the user.")
