# Neopixel-Display-System

The Neopixel Display System is basically a small ecosystem emerging from the convergence of an experiment to drive a Neopixel Matrix display using an ESP32 processor, and an interest / need to remotely control an external audio ouptut from [OBS Studio](https://obsproject.com/).

## Components

The Neopixel display panel<br><img src="/assets/NeopixelPanel.png" width="420" height="60">

The Neopixel Server Web Page<br><img src="/assets/webpage.png" width="280" height="60">

The Neopixel Client Application<br><img src="/assets/NET-MAUI.png" width="316" height="120">

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
is implemented using JSON objects rather than a GET construct. The Arduino version of the
Neopixel Client utilizes the BLE interface 

The features consist primarily of Displaying text in choice of color (or rainbow)
with specified alignment (left, center, right, scroll), displaying a selected builtin
bitmap image, again with specified alignment, setting one of four virtual LEDs to on, 
off, or blink, and turning a mechanical relay or analog switch on or off.

[more ...](NeopixelServer/README.md)


## OBS Studio Script

The OBS Studio script is a python script that allows control of any / all of the provided
features as a result of activation and / or deactivation of any scene. The user interface
also supports enabling / disabling the participation of any scene.

[more ...](obsstudio_script/README.md)


<img src="/assets/obsstudio.png">

## Neopixel Client

The Neopixel Client is a .NET MAUI applicaiton using a BLE interface on Android and a HTTP 
interface on WIndows (The BLE library does not support Windows)

[more ...](NeopixelClient/README.md)

<img src="/assets/NET-MAUI.png">

# Key Concepts and Objectives
Most of my projects are more about doing something interesting and learning new skills than 
producing a particular result. I will describe a few of them here offer background on 
certain implementation decisions.

## Dynamic User Interface Generation
Design a metadata construct that can provide sufficient information for a client to construct 
a realistic user interface.

Implement metadata generation in the server applilcation.

Implement dynamic user interface generation in javascript for a web page and in C# for .NET MAUI

## Develop Arduino API focused ESP32 Wifi + BLE 'boilerplate'
Develop an ESP32 application that implements Wifi (with web server and embedded web page)
or Bluetooth BLE or both and that is designed to remove either or both if not needed.

## Develop a static embedded javascript-based web page
Implement a mechanism to launch and test the embedded web page when it is not actually 
embedded.

Implement javascript version of dynamic user interface generation. 

Embed the tested web page into the Neopixel Server.

## .NET MAUI Experimentation
Learn about the .NET MAUI environment.

Implement pure C# version of dynamic user interface generation. 

Develop application versions for both Android and Windows targets, at least one of which is BLE capable.

**WARNING** Microsoft Visual Studio Preview Version 17 or newer is required for .NET MAUI.

## Develop a OBS Studio script (python)
Learn the details of OBS Studio scripting, especially the uesr interface operation.

Develop a script based user interface that offer full accessibility to all of the Neopixel Server features.

Develop the connectivity to control the Neopixel server from the OBS Studio script.


