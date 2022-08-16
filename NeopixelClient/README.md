# Neopixel Client

The Neopixel Client is a .NET MAUI applicaton using a BLE interface on Android and a HTTP 
interface on Windows desktop (For now, the third party BLE service used does not support Windows desktop)

<img src="/assets/NET-MAUI.png">

I wanted to do a BLE Android App to support the Neopixel server, when I saw a .NET MAUI announcement, I decided 
that it would be a good experiment vs. using Android Studio or Xamarin. It also offered the possibility of single source 
Android, IOS, and / or MAC OS support.

BUT for the experiment, I also wanted to use pure C#, so this example does not utilize any XAML or BLAZOR sources.

I also wanted to dynamically create the device interface and representation from device metadata. The interface is implemented as **IDynamicViewDevice**, viewed and controlled using the **DynamicView** class.

## Operation and design elements
The application is a single **MainPage** that presents available NeopixelServer device(s) and provides the ability to view / control the device(s). 

In the case of BLE (Android), the available devices are presented using a **CollectionView** of **NeopixelBleDevice**s via a **BleTemplate**. The **CollectionView** is populated by the **NeoPixelBleScanner**. The **BleTemplate** provides a **Connect** button as part of each device view. 

In the case of HTTP (Windows), the available device is accessed via a IP Address text box and a **Connect** button.

When a **Connect** button is pressed, it's label changes to "**Connecting ...**" while attempting to connect to the device. 
"Connecting" establishes a connection with the selected BLE device **device.ConnectAsync()**, or with a new **NeopixelHttpDevice()** at the specified IP Address, respectively.

Once a device is connected, it's metadata is retrieved **mdata = device.GetMetaDataAsync()**, and a view / control page is created and displayed using **CreateDynamicView(mdata, device)**. The device view includes a **Disconnect** button that returns to the collection view.

## Quick Start
This application is a client of an **esp32Neopxel** device. If an esp32Neopixel device is not available, go to [Neopixel Server](/NeopixelServer) and create one of them first.

At the time of this writing, .NET MAUI requires Visual Studio 2022 Preview. That is sort of a moving target, so I recommend going directly to the [.NET MAUI documentation](https://docs.microsoft.com/en-us/dotnet/maui) to install the most appropriate development tools.

Once the desired version of Visual Studio has been installed, use it to open **NeopixelClient.sln**.

### Windows Target
Select "Windows Machine" for the target, then build and run. When the app launches, it will show a suggested IP address. Change the IP address to that of your NeoPixelServer, and press connect ... that's about it

### Android Target
You will need an android device (platform version 23.0 or newer), connected in developer mode. It will appear in the targets list under "Android Local Devices". Select that device for the target, then build and run. When the app launches, it will begin BLE scanning and present a list of available devices. Press connect on the device of interest, and you should be excactly where you ended with the Windows Target.

# Design 
If you are ready to modify the software for your own needs or to follow this path into .NET MAUI, look [here for more information about the design](DESIGN.md)
