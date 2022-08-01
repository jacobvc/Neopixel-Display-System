# Neopixel Server

The NeopixelServer is an application running on an ESP32 Dev Board that operates a Neopixel 
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
WS2812 8x32 led matrix displays (Compile option, 2 wide is default). The display(s) need to be 
connected to GROUND, 5V (VIN), and D32. When more than one is used each additional one is 
simply plugged into the 'out' side of the prior. See more about hardware below.

The first step is to install the development environment and build the server.

* [Install the Arduino IDE](https://www.arduino.cc/en/software) (Note that either veersion 1.0 or 2.0 can be used)
* [Install ESP32 support in the Arduino IDE](https://randomnerdtutorials.com/installing-the-esp32-board-in-arduino-ide-windows-instructions/)
* Connect and test the ESP32 device (should have done this in the previous step)
* Install the required libraries
* Build and run the project

The required libraries include:
* ArduinoJson version 6.x (install using Arduino Library Manager)
* Adafruit_GFX (install using Arduino Library Manager)
* Adafruit_NeoMatrix (install using Arduino Library Manager)
* [ESPAsyncTcp](https://github.com/me-no-dev/AsyncTCP) Install as zip library (see below)
* [ESPAsyncWebServer](https://github.com/me-no-dev/ESPAsyncWebServer) Install as zip library (see below)

To install as a Zip Library, follow the link the the github repository, click on the green "code" button,
and choose download zip. Then in the Arduino IDE select the menu item: Sketch > Include Library > Add .zip Library,
and browse to the downloaded file location.

Configure the IDE for the board and project and build / run
* Board ESP32 Dev Module
* Partition Scheme: Minimal SPIFFS (or any config with at least 1.6MB APP)
* Serial Port

The implementation is provisioned using WiFi Smartconfig. [Install the ESP32 Smart Config App](https://play.google.com/store/apps/details?id=com.khoazero123.iot_esptouch_demo) and use it configure the access point connecton. When it successfully connects to WiFi, the IP address scrolls on the display.

Open that IP address in a browser to control the device.

# Hardware
The Neopixel Matrix is a panel with 8 x 32 pixels (LEDs) mounted on a flexible plastic substrate, 
with a 3 pin JST SM female input connector and a 3 pin JST SM male output connectors. Multiple panels are daisy chained by plugging the input connector of one to the output connector of another. Note that circuit board version of the SM connector are not avaialable, so the normal approach is to use a pigtail cable.

<img src="/assets/neopixel32.png">

One practical approach for panel construction is to use a 1/8 inch acrylic backing, and to fasten the disply to the backing using a "PVC FRP Cap Moulding", available from building supply stores. They make a very tight connection and stay in place very nicely. To use the display only, Connect the input to GROUND, VIN, and D32.

[The full hardware immplementation can be implemented using this circuit board](https://github.com/jacobvc/ESP32-Hardware-Boards/tree/main/Esp32Relays)

# Design 
If you are ready to modify the software for your own needs, look [here for more information about the design](DESIGN.md)
