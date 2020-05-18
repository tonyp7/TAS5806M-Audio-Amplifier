# TAS5806M Audio Amplifier

![Amplifier board](https://github.com/tonyp7/TAS5806M-Audio-Amplifier/raw/master/pictures/tas5806m-amplifier-pic1.jpg)

This repository contains hardware to create a fully fledged TAS5806M 23-W, I2S digital Input, stereo, closed-loop Class-D audio
amplifier.
In addition, it contains example software for Espressif's ESP32 to create a bluetooth audio speaker. Any micro-controller capable of I2S and I2C will be able to control the board, ESP32 is picked here due to its popularity and large documentation.

# Repository structure

* hardware: contains the PCB, schematics and BOM for the physical audio amplfiier board
* software: contains code examples to interface with the board

# License

Hardware and software is free and open source, published under the MIT license.

# Compatibility with TAS5805M

Both amplifiers are the same but in different packages allowing for different maximum power output. They share the same I2C interface and registers and as such the code included in this repository is compatible with TAS5805M audio amplifier. [See here the comment from TI on this topic](http://e2e.ti.com/support/audio/f/6/t/812496?TAS5805M-comparing-to-TAS5806M).

![Amplifier board top](https://github.com/tonyp7/TAS5806M-Audio-Amplifier/raw/master/pictures/tas5806m-amplifier-pic2-top.jpg)
