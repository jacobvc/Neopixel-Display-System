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
### Entry Page
The entry page is the first thing the user sees. 
* Mainpage.cs - Implements the MainPage class, derived from Content Page. 

### Dynamic View
* DynamicView.cs

### Color Picker
* ColorPicker.cs

### Devices
* IDynamicViewDevice.cs
* HttpDynamicViewDevice.cs
* NeopixelStatus.cs
* NeopixeHttplDevice.cs
* NeopixelBleDevice.cs

### Platforms, Properties, Resources

## Nuget Packeges
* triaxis.BluetoothLE
* CommunityToolkit.Maui

## Starting a New Application
I'm sure there must be, or eventually will be a better way, but I found the the easiest way to start a nen app is to

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
