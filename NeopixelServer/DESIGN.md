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
 
### Functionalities and source files
The server functionalities include:

* config.h - The physical configuration. This file specifies the Neopixel Panel size and the number of panels in use, in both horizontal and vertical dimensions, as well as GPIO port usage and the user inteface names for the virtual LEDs and the physical outputs.
* esp32Neopixel.ino, neopixel.h - The 'main' application. Implements setup() and loop(), ad dispatches the corresponding functions for each of the functionalities.
* neopixeldisplay, assets/ - Implementation of display functionality. See below
* neopixelparams - Implementation of All parameter access and representation. See Below
* neopixelble - The bluetooth BLE interface. See below
* neopixelwifi, WebPageDevelopment/ and assets/webcontent.h - The WiFi / Webserver interface. See Below

### Display Functionality: class NeopixelDisplay
neopixeldisplay.cpp implements the NeopixelDisplay object, which encapsulates the display functionality. The display functionality consists of "Text", "Graphics", and "Virtual LEDs". As the display module, it also implements the Arduino integration functions:
 
    void NeoDisplaySetup(int gpio, int matrixW, int matrixH, int panelsW, int panelsH);
    void NeoDisplay10msTick();

#### Text
Provides the ability to display arbitrary length text with specified aligment and color. Color is communicated as the index of a set of options, where zero is reserved for "OFF" and one is reserved for "Rainbow". Alignment may be specified as left, center, right, or scroll. If the text does not fit on the display and alignment is not "scroll', the text "rocks" to display all of the text.
 
 Note that when a graphic is displayed, the text color is overriden to "OFF"
 
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
Provides the ability to display internally stored bitmaps. The selected bitmap is communicated as the index of a set of options, where zero is reserved for "OFF" Alignment may be specified as left, center, right, scroll, or bounce.
 
 Get accessors are provided for Bitmap and Alignment to make them available for status reporting.

 Note that when text displayed, the bitmap is overriden to "OFF"
 
    int GetBitmap();
    int ShowBitmap(uint16_t which, Alignment alignment);
    int ShowBitmap(uint16_t which);
    Alignment GetBmpAlignment();
    void SetBmpAlignment(Alignment alignment);

#### Virtual LEDs
Provide the ability to display virtual (2 x 2 pixel) LED indicators in the first two columns of the display. Each one may have a mode of "ON", OFF", or "BLINK". The LEDs may each have one or two colors configured. The two colors swap position every second when the display is "on".
 
    void LedConfig(int index, int color1, int color2);
    LedDef *GetLeds();
    void SetLed(int index, LedMode mode);

#### Assets
The assets subdirectory has header files containing the embedded webpage file contents (webcontent.h/webcontent.min,h) and header files containing the data for builtin bitmaps. The data in the bitmap header files is in RGB (5-6-5) format.

### Preference Functionality
Peferences are persistent configuration values that are stored in ESP32 nonvolatile memory. The preferences implementation
maintains the preferences values in global memory variables for normal access, and saves the preferences to persistent memory upon change. Preference values can be accessed as parameters.
 
Preferences are implemented in neopixelparams.cpp. 
 
    void GetPreferences();  - Get preferences from persistent memory into global variables
    void SavePreferences(); - Save global preference variables into persistent memory
 
### Output Functionality
The general pattern for parameter implementation is to implement the functionality and to connect it to parameters using an accessor. In the case of outputs, the values are values of GPIO signals, and the hardware itself is treated as the implementation. That is, GPIO read / write are the accessors to the actual hardware implementation Outputs are implemented in neopixelparams.cpp. 
 
### Parameter Access
Parameters are named properties that are externally visible and potentially modifiable from an external interface.
The Parameter interface makes parameter definitions visible via metadata generation, distributes parameter values
via status generation, and supports chaning parameters via parameter parsing.
 
Parameter access is implemented in neopixelparams.cpp. As the parameters module, it also implements the Arduino integration functions:
 
    void NeoParamSetup();
    void NeoParamTick();
 
#### Metadata Generation (JSON)
Metadata is generated by calling JsonEncodeNeopixelMetaData() at startup, and after any parameters are modified that would change the MetaData content.
 
    void JsonEncodeNeopixelMetaData();
 
 The JSON encoded metadata string is stored in a global variable, metadataJson, and is available to be
 used by a communications interface (webserver / BLE) when metadata is requested / delivered.

#### Status Generation (JSON)
Status data is generated by calling JsonEncodeNeopixelData() at startup, and after any status parameters are modified.
 
    void JsonEncodeNeopixelData();
 
 The JSON encoded status string is stored in a global variable, statusJson, and is available to be
 used by a communications interface (webserver / BLE) when status is requested / delivered.
 
 **TODO** NeoBleSendStatus() Updates the BLE status characteristic and sends a notification **NOT CURRENTLY USED**
 
#### Parameter Parsing: class NeopixelParams
Parameter parsing is implemented in neopixelparams.cpp. A NeopixelParams object, 'params' is instantiated in NeoParamSetup(),
including AddParser() for each "parameter". when a modification request is received (currently via HTTP or BLE) params.parse() is invoked for each received key / value pair. After the update, a new statusJson is generated.
 
    void AddParser(String name, byte (*parse)(const String &value, int arg), int arg);
    byte parse(const String &name, const String &value);

### Bluetooth BLE
The BLE server is implemented in neopixelble.cpp. As the BLE module, it also implements the Arduino integration functions:
 
    void NeoBleSetup();
    void NeoBleTick();

 The BLE server interface implements two characteristics; MetaData read, and Status read / write / Notify
 
#### Metadata Characteristic
METADATA_1_CHARACTERISTIC_UUID and METADATA_2_CHARACTERISTIC_UUID

The Metadata characteristic is implemented in two components (literally two characteristic UUIDs) that the reader concatenates into a single record. The application implements this by sending the first 600 bytes of the global JSON encoded metadata string for (1), and the balance of that string for (2).
 
This is done to get around the ESP32 BLE implementation 600 byte characteristic limitation. 
There are several more elegant options, but this does the job so a more elegant solution was not given a high priority.
 
#### Status Characteristic
STATUS_CHARACTERISTIC_UUID

 A global JSON encoded status string is maintained by the application. the parser updates the value of the global status string whenever a status parameter is parsed, modifying a status value.
 
##### Read characteristic
 A status read is processed by sending the current value of the global status string.
##### Write characteristic
 A status write is processed by calling params.parse() for each key / value pair in the received JSON object.
##### Notify characteristic
 NeoBleSendStatus() triggers a notification with updated data each time device status is modified.
 
### WiFi / Webserver
The WIFI Webserver is implemented in neopixelble.cpp using WiFi / esp_wifi and ESPAsyncWebServer. As the WiFI module, it also implements the Arduino integration functions:
 
    void NeoWifiSetup(void);
    void NeoWifiTick(void);

#### WiFi
 wifi is provisioned using SmartConfig (supported by the ESP32 WiFi Smart Config App). The startup state machine attempts to connect using the configured parameters. If the launch attempt is insuccessful, it goes into Smart Config server mode and waits for the app to update those parameters. This operation repeats until a WiFi connection is successfully estalished.

#### Webserver
 The webserver serves three paths; root files, metadata, and status as follows:
##### Root Files
 Return the web page and other files. These "files" are embedded into the binary using "assets/webcontent.h". For 
 more information, look under [Web Page development](WebPageDevelopment) for more details.
 
 "/": the HTML web page (index.html)

 "/dynamicview.js": The javascript file referenced by the web page
 
 "/dynamicview.css": The CSS stylesheet referenced by the web page

 ##### Get Metadata
 "/metadata": return the JSON encoded Metadata
 
##### Get Status
 "/status": Return the JSON encoded status data
##### Set Parameter(s)
 "/status?key=value...": parse each key / value pair in the GET parameters then return the updated JSON encoded status.
 
## Metadata definitions
Metadata describes the data exchanged between the Neopixel server and client. It is intended to contain
sufficient information to construct a functional user interface.
 
It currently supports data of types boolean, string, and select list, as well as arrays of those types
and composites of more than one data element.
 
Metadata is transmitted as a JSON string representing an array of VewMetaData objects.

### ViewMetaData Object
A ViewMetaData object describes data, and is composed of the following fields:
 
**string name:**
Name of the metadata item (same as name of associated Status data)

**string[] names:**
If non-null, item value is an array, and 'names' are names for each array member

**string label:**
Suggested label for this item

**string type:**
Type of item being described. One of:
* bool - A boolean value
* select - A value that has a specified set of textual values represented by their index
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
