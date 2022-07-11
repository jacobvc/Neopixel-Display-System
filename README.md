# Neopixel-Display-System

The Neopixel display panel<br><img src="/assets/NeopixelPanel.png" width="420" height="60">

The Neopixel Server Web Page<br><img src="/assets/webpage.png" width="280" height="60">

The OBS Studio Script<br><img src="/assets/obsstudio_script.png" width="178 height="100">

The Neopixel Client Application<br><img src="/assets/NET-MAUI.png" width="316" height="120">


The Neopixel Display System is basically a small ecosystem emerging from the convergence of an experiment to drive a Neopixel Matrix display using an ESP32 processor, and an interest / need to remotely control an external audio ouptut from [OBS Studio](https://obsproject.com/).

## Components
At this time, the key components of the system are the NeopixelServer, an OBS Studio plugin, and a .NET MAUI Android / WIndows application.

## NeoPixel Server

![Server Panel](/assets/NeopixelPanel.png)

The NeopixelServer is an aplication running on an ESP32 that operates a Neopixel
Matrix display and some physical I/O (mechanical relays and virtual LEDs). 

The NeopixelServer is an aplication running on an ESP32 that operates a Neopixel 
Matrix display and some physical I/O (mechanical relays and virtual LEDs). 

The Neopixel Server is developed using the Arduino for ESP32 development tools

It hosts a WiFi based web server and provides its own web page to control its 
capabilities. The web server provides its metadata and status data as JSON objects
and control is implemented using Http GET requests. **WARNIING** the ESP32 does
not support 5G networks.

<img src="/assets/webpage.png">

It is also a Bluetooth Central Device that uses the same messaging EXCEPT control
is implemented using JSON objects rather than a GET construct.

The features consist primarily of Displaying text in choice of color (or rainbow)
with specified alignment (left, center, right, scroll), displaying a selected builtin
bitmap image, again with specified alignment, seting one of four virtual LEDs to on, 
off, or blink, and turning a mechanical relay or analog switch on or off.


## OBS Studio Script

The OBS Studio script is a python script that allows control of any / all of the provided features as a result of activation and / or deactivation of any scene. The user interface 

<img src="/assets/obsstudio_script.png">

## Neopixel Client

The Neopixel Client is a .NET MAUI applicaiton using a BLE interface on Android and a HTTP interface on WIndows (The BLE library does not support Windows)
<img src="/assets/NET-MAUI.png">

