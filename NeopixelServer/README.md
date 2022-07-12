# Neopixel Server

The NeopixelServer is an aplication running on an ESP32 that operates a Neopixel 
Matrix display and some physical I/O (mechanical relays and virtual LEDs). 

The Neopixel Server is developed using the Arduino for ESP32 development tools

It hosts a WiFi based web server and provides its own web page to control its 
capabilities. The web server provides its metadata and status data as JSON objects
and control is implemented using Http GET requests. **WARNIING** the ESP32 does
not support 5G networks.

It is also a Bluetooth Central Device that uses the same messaging EXCEPT control
is implemented using JSON objects rather than a GET construct.

The features consist primarily of Displaying text in choice of color (or rainbow)
with specified alignment (left, center, right, scroll), displaying a selected builtin
bitmap image, again with specified alignment, seting one of four virtual LEDs to on, 
off, or blink, and turning a mechanical relay or analog switch on or off.

# Quick Start
The system is designed to be completely incremental. That is, all you really need is the
server itself and a web browser. The minimum server is an ESP32 Dev Module and one or more
WS2812 8x32 led matrix displays (Compile option, 2 is default). The display(s) need to be 
connected to GROUND, 5V (VIN), and D32. When more than one is used each additional one is 
simply plugged into the 'out' side of the prior. See more about hardware below.

The first step is to install the development environment and build the server.

* [Install the Arduino IDE](https://www.arduino.cc/en/software) (Note that either veersion 1.0 or 2.0 can be used)
* [Install ESP32 support in the Arduino IDE](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
* Connect and test the ESP32 device (should have done this in the previous step)
* Install the required libraries
* Build and run the project

The required libraries include:
* ArduinoJson-6.x
* Adafruit_GFX
* Adafruit_NeoMatrix
* ESPAsyncWebServer

The implementation uses WiFi Smartconfig. [Install the ESP32 Smart Config App](https://play.google.com/store/apps/details?id=com.khoazero123.iot_esptouch_demo) and configure it. When it successfully connects to WiFi, it scrolls the IP address on the display.

Open that IP address in a browser to control the device.

# Hardware

1/8 inch plastic
PVC FRP Cap Moulding

# Design 
[Design](DESIGN.md)
