# Neopixel Client (.NET MAUI) Design
 The design is a pure C# .NET MAUI application (incorporating no XAML or BLAZOR sources), supporting both (either) BLE and HTTP device interfaces.
 
 The starting point was informed by [this Melissa Houghton article](https://melissahoughton-dev.medium.com/getting-started-net-maui-dc3522df6de3).
 
 This section is a high level wallk through the source files and their functionalities.

## Source Files and Functionalities

### Boilerplate
* NeopixelClient.sln - The project solution, maintained by Visual Studio

Nothing interesting here
* NeopixelClient.csproj - The project, maintained by Visual Studio

Generally not edited manually. This is where things like NuGet packages and platform versions 
are maintained.
* MauiProgram.cs

This is the main entry point. It creates  MauiApp builder, adds services and configurations, and registers "App"
as THE application. Generally adding a nuget package will give instructions on how to manually edit the builder.
* App.cs

The application ... Potentially performs some configuration, then launches new MainPage().

In this case, for Windows, the application window size is set to a constant.

Note that App.cs is not created by the MAUI new project wizard. For more info on it's origin, see [Starting a New Pure C# Application](#starting-app).

### Entry Page
The entry page is the first thing the user sees. 
* Mainpage.cs - Implements the MainPage class, derived from Content Page. 

This page presents available NeopixelServer device(s) and provides the ability to view / control the device(s). 

In the case of BLE (Android), the available devices are presented using a **CollectionView** of **NeopixelBleDevice**s via a **BleTemplate**. The **CollectionView** is populated by the **NeoPixelBleScanner**. The **BleTemplate** provides a **Connect** button as part of each device view. 

In the case of HTTP (Windows), the available device is accessed via a IP Address text box and a **Connect** button.

When a **Connect** button is pressed, it's label changes to "**Connecting ...**" while attempting to connect to the device. 
"Connecting" establishes a connection with the selected BLE device **device.ConnectAsync()**, or with a new **NeopixelHttpDevice()** at the specified IP Address, respectively.

Once a device is connected, it's metadata is retrieved **mdata = device.GetMetaDataAsync()**, and a view / control page is created and displayed using **CreateDynamicView(mdata, device)**. The device view includes a **Disconnect** button that returns to the collection view.


Note that MainPage.cs is not created by the MAUI new project wizard. For more info on it's origin, see [Starting a New Pure C# Application](#starting-app).

### Dynamic View
The **DynamicView** class creates an interactive view of a device that implements the **IDynamicDevice** interface.

* DynamicView.cs - THE DynamicView Class
     * DynamicView(IDynamicViewDevice device, ContentPage page) - Constructor: Instantiates DynamicView object for 'device' to be displayed on 'page'.

     * Create(ViewMetadata[] metadata) - Create a view of 'this device' from 'metadata' objects.

       Process each member of the metadata array is processed. Metadata objects of type 'opts' are used to build an options dictionary, and each metadata object of other types is used to createView().The options dictionary is used to populate all of the options associated with the created view.

     * CreateView(StackBase ctl, String key, ViewMetadata item)

       Perform a createView() for each 'field' in metadata 'item' of type 'composite' or 'collapsible', a createControl() for each name in a metadata 'item' containing an array of 'names', and a createControl() for all other metadata 'item's

     * CreateControl(StackBase ctl, String key, String metaType, String label, String optionsName)

       Create a "switch" for metaType 'bool, a button (linked to ColorPicker) for metaType 'color', a Picker for metaType 'select', and aEntry for all other metaType's. Add the created control to 'ctl'. Each control is assigned a 'changed' event handler that ultimately calls device.Submit().

     * Status_updated() 
       
       Event handler for IDynamicViewDevice.Updated. Update the controls mapped to each value received in the status update.

### Color Picker
I couldn't easily find a useful colorpicker provider. I decided to do something minimal to have at least a basic colorpicker (using CommunityToolkit.Maui).


* ColorPicker.cs - THE ColorPicker Class
     * ColorPicker(String label, Color color, string key, View anchor) - Constructor

       Create a 'Popup' containing a Label with text: 'label' and background 'color', a Label to display new selected color, a grid of colored buttons representing a predefined list of color options, and OK / Cancel buttons.

       Update the selected color label when a colored button is pressed, and fire the SelectedColor event when the OK button is pressed,

     * SuggestedTextColor(Color background) - Static helper function

       Calculate a text color (black or white) with good visibility when displayed on 'background'.

### Devices
* IDynamicViewDevice.cs

  The IDynamicViewDevice interface

    * ConnectAsync() - Connect to the device

      Attempt to connect to the device. Returns boolean success
    * Task<ViewMetadata[]> GetMetaDataAsync() - Get the device metadata

      Get the device data description as an array of ViewMetadata.
    * GetStatusAsync() - Request device status

      Ask the device to update it's status
    * StatusUpdated Updated - Event

      Deliver device status to application by firing this event.
    * Submit(String param, String value)

      Submit data modification to device and 'param' name / 'value' pair
    
* HttpDynamicViewDevice.cs - Generic IDynamicViewDevice utilizing HTTP transport
* NeopixelStatus.cs - The status object produced by NeoPixelServer Devices
* NeopixeHttpDevice.cs - NeoPixelServer specific IDynamicViewDevice utilizing HTTP transport
* NeopixelBleDevice.cs - NeoPixelServer specific IDynamicViewDevice utilizing BLE transport

### Platforms, Properties, Resources

## Nuget Packages
* triaxis.BluetoothLE
* CommunityToolkit.Maui

<h2 id="starting-app">Starting a New Pure C# Application</h2>
I'm sure there must be, or eventually will be a better way, but I found the the easiest way to start a new app is to

* Use the 
new project wizard to create a new .NET MAUI application, then delete all of the .xaml and .xaml.cs files in the root directory

* Add a new App.cs class and replace

       internal class App
       {
       }

with

       public class App : Application
       {
          public App()
          {
              MainPage = new MainPage();
          }
       }

* Add a new MainPage class replace

       internal class MainPage
       {
       }

with

        public MainPage()
        {
            var layout = new VerticalStackLayout
            {
                VerticalOptions = LayoutOptions.Center,
                HorizontalOptions = LayoutOptions.Center,
            };
            var label = new Label
            {
                Text = "My first pure C# MAUI App",
                FontSize = 48,
            };
            layout.Add(label); 
            Content = layout;
        }
