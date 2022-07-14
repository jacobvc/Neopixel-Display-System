# Neopixel Server Design

## Architecture
The design is ESP32 Arduino based. This has several implications:
* It is developed using the Arduino IDE
* There are a (very) few expressif libraries that are unavailable
* The are a few available Arduino libraries that would not be available from the expressif SDK
* Rather than a single (main()) entry point, there are two; setup() and loop()
* The entry points are in a file with a .ino extension (esp32Neopixel.ino) in a directory of the same name
* Additional source files in the same directory (,ino or .cpp) are included in the project

By convention, "functionalities" are implemented in a single file, supported by a single header file, and they export the functions: <functionality>Setup() and <functionality>Tick() which are invoked by the entry point setup() and loop() respectively. To the extent feasible, interdependency is avoided, permitting a functionality to be removed by removing those two function calls.
 
## Functionalities and source files
The server functionalities

* Configuration: config.h - The physical configuration. This file specifies the Neopixel Pannel size and the number of panels in use, in both horizontal and vertical dimensions, as well as GPIO port usage and the user inteface names for the virtual LEDs and the physical outputs.
* esp32Neopixel.ino, neopixel.h - The 'main' application. Implements setup() and loop(), ad dispatches the corresponding functions for each of the functionalities.
* neopixeldisplay, assets/ -
* neopixelparams - 
* neopixelble
* neopixelwifi, WebPageDevelopment/ and assets/webcontent.h

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
