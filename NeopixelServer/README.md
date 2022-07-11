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

# Dynamic User Interface Generation

## ViewMetaData
**string name:**
Name of the metadata item (same as name of associated data)

**string[] names:**
If non-null, item value is an array, and 'names' are names for each array member

**string label:**
Suggested label for this item

**string type:**
Type of item being described. One of:
* bool - A boolean value
* select - A value that hase a specified set of textual values represented by their index
* text - A String value
* composite - A set of ViewMetadata objs that combine to represent a compisite entity
* value - A constant value to be displayed rather than connected to a control
* opts - Enumeration of a set of 'option' values

**string opts:**
In the case of type = 'select', the name of ViewMetaData item that enumerates the select options

**string[] values:**
Option value definition in the case of type = 'opts'

**string value:**
In the case of type = 'value', its constant value

**ViewMetadata[] fields:**
In the case of type = 'composite', the ViewMetadata items that comprise the composite item

# Hardware

1/8 inch plastic
PVC FRP Cap Moulding
