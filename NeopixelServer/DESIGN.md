# Neopixel Server Design

## Architecture
The design is ESP32 Arduino based. This has several implications:
* It is developed using the Arduino IDE
* There are a (very) few expressif libraries that are unavailable
* The are a few available Arduino libraries that would not be available from the expressif SDK
* Rather than a single (main()) entry point, there are two; setup() and loop()
* The entry points are in a file with a .ino extension (esp32Neopixel.ino) in a directory of the same name
* Additional source files in the same directory (.ino or .cpp) are included in the project

By convention, "functionalities" are implemented in a single file, supported by a single header file, and they export the functions: <functionality>Setup() and <functionality>Tick() which are invoked by the entry point setup() and loop() respectively. To the extent feasible, interdependency is avoided, permitting a functionality to be removed by removing those two function calls.
 
## Functionalities and source files
The server functionalities

* Configuration: config.h - The physical configuration. This file specifies the Neopixel Panel size and the number of panels in use, in both horizontal and vertical dimensions, as well as GPIO port usage and the user inteface names for the virtual LEDs and the physical outputs.
* esp32Neopixel.ino, neopixel.h - The 'main' application. Implements setup() and loop(), ad dispatches the corresponding functions for each of the functionalities.
* neopixeldisplay, assets/ - Implementation of display functionality. See below
* neopixelparams - Implementation of All parameter access and communications. See Below
* neopixelble - The bluetooth BLE interface. See below
* neopixelwifi, WebPageDevelopment/ and assets/webcontent.h - The WiFi / Webserver interface. See Below

### Display Functionality
neopixeldisplay implements the NeopixelDisplay object, which encapsulates all of the display functionality.
#### Text
Provide the ability to display arbitrary length text with specified aligment and color. Color is communicated as the index of a set of options, where zero is reserved for "OFF" and one is reserved for "Rainbow". Alignment may be specified as left, center, right, or scroll. If the text does not fit on the display and alignment is not "scroll', the text "rocks" to display all of the text.
 
 Note that when a bitmap is displayed, it overrides the text color to "OFF"
 
 Get accessors are implemented for Text, Color, and Alignment to make them available for status reporting.
 
    String& GetText();
    int ShowText(String text);
    int ShowText(String text, int colorIndex);
    int ShowText(String text, int colorIndex, Alignment alignment);
    Alignment GetTextAlignment();
    void SetTextAlignment(Alignment alignment);
    void SetTextColor(int value);
    int GetTextColor();
#### Graphics
Provide the ability to display internally stored bitmaps. The selected bitmap is communicated as the index of a set of options, where zero is reserved for "OFF" Alignment may be specified as left, center, right, scroll, or bounce.
 
 Get accessors are provided for Bitmap and Alignment to make them available for status reporting.

 Note that when text displayed, it overrides bitmap "OFF"
 
    int GetBitmap();
    int ShowBitmap(uint16_t which, Alignment alignment);
    int ShowBitmap(uint16_t which);
    Alignment GetBmpAlignment();
    void SetBmpAlignment(Alignment alignment);

##### Assets
 The assets subdirectory contains header files with the data for builtin bitmaps. The data in these header files is assumed to be in RGB (5-6-5 format.
#### Virtual LEDs
Provide the ability to display virtual (2 x 2 pixel) LED indicators in the first two columns of the display. Each one may have a mode of "ON", OFF", or "BLINK". The LEDs may each have one or two colors configured. The two colors swap position every second when the display is "on".
 
    void LedConfig(int index, int color1, int color2);
    LedDef *GetLeds();
    void SetLed(int index, LedMode mode);
### Parameter Access
#### Parameter Getters
#### Parameter Setters
##### Outputs
#### Parameter Parser
#### Metadata (JSON)
#### Status (JSON)
### Bluetooth BLE
#### Read Metadata
#### Read Status
#### Set Parameter(s)
### WiFi / Webserver
#### WiFi
#### Webserver
#### Get Metadata
#### Get Status
#### Set Parameter(s)
#### Web Page Development
##### Dynamic View
 
 
# Metadata Based User Interface Generation

## ViewMetaData Object
**string name:**
Name of the metadata item (same as name of associated Status data)

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
