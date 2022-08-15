# Neopixel Client (.NET MAUI) Design

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
* HttpDynamicViewDevice.cs
* NeopixelStatus.cs
* NeopixeHttpDevice.cs
* NeopixelBleDevice.cs

### Platforms, Properties, Resources

## Nuget Packages
* triaxis.BluetoothLE
* CommunityToolkit.Maui

<h2 id="starting-app">Starting a New Pure C# Application</h2>
I'm sure there must be, or eventually will be a better way, but I found the the easiest way to start a new app is to

* Use the 
new project wizard to create a new .NET MAUI application, then delete all of the .xaml and .xaml.cs files in the root directory

* Asd a new App.cs c,lass that implements a trivial App and replace

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

* Add a new MainPage class that implements the main ContentPage and replace

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
