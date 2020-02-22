# Four Encoders, three push buttons in three modes joystick

This Sketch is meant to be used with an Arduino Pro Micro or any other Arduino with an ATmega32U4 chipset, that may act as an USB Joystick Device.

The sketch uses the following libraries, that must be installed from "tools" menu within the Arduino IDE:

* Joystick library from  Matthew Heironimus, https://github.com/MHeironimus/ArduinoJoystickLibrary
* Bounce2 library from Thomas Fredericks, https://github.com/thomasfredericks/Bounce2
* Keypad Library from Mark Stanley and Alexander Brevig, https://www.arduinolibraries.info/libraries/keypad

The sketch also uses the Encoder code from Ben Buxton 
More info: http://www.buxtronix.net/2011/10/rotary-encoders-done-properly.html

The base idea for this sketch is from TOPMO 3 
https://www.xsimulator.net/community/threads/diy-arduino-buttonbox.8302/

To identify the pin numbers, that are used in the sketch, on your Arduino refer to this pin layout https://learn.sparkfun.com/tutorials/pro-micro--fio-v3-hookup-guide/hardware-overview-pro-micro

## Project Goal
I wanted to build a simple - but versatile - input device for computer games (especially flight simulators) that doesn't need any special software plugins on the PC side to make it work in any type of game you like, that supports custom joystick configuration. 

Given you have an Arduino Pro Micro with already soldered pin headers, the hardware should be easy to assemble, mostly without the need to solder anything. With the exception of the status LED everything else is connectible via jumper wires and insulating screw joints.

I wanted to get a maximum number of joystick commands from a single Arduino, without the need to use more complex electronic devices like multiplexer boards, busses etc. Therefore I use a single status LED to indicate one of three modes instead of occupying two more didgital outputs of the Arduino with two more LEDs, so that every mode would be indicated by a single LED. 


## Description of Hardware

My prototype consists of these parts:

* An Arduino Pro Micro with soldered pin headers 
* A couple of jumper wires like these (female/female and female/male) https://www.amazon.de/Female-Female-Male-Female-Male-Male-Steckbr%C3%BCcken-Drahtbr%C3%BCcken-bunt/dp/B01EV70C78
* Four momentary push button like these: https://www.amazon.de/gp/product/B076SYC21Q/ref=ppx_yo_dt_b_asin_title_o04_s00?ie=UTF8&psc=1
* Four cheap encoders with push buttons https://www.amazon.de/gp/product/B07DM2GN2Z/ref=ppx_yo_dt_b_asin_title_o06_s00?ie=UTF8&psc=1

One of the push buttons acts as a mode switch, that allows to use every encoder and button in three different modes. That means, that i.e. a single enoder may emulate up to 9 different joystick buttons.

All in all my prototype emulates up to 12 encoders and 21 buttons.

A status LED (connected to a digital output pin via a 220 Ohm resistor) shows the currently selected mode. The mode button cycles through all three modes.

* LED off: Mode 1
* LED on: Mode 2
* LED blinking: Mode 3

