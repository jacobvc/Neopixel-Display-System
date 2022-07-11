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

# Design 
[Design](DESIGN.md)
# Hardware

1/8 inch plastic
PVC FRP Cap Moulding
