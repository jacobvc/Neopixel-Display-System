# Neopixel Client

The Neopixel Client is a .NET MAUI applicaiton using a BLE interface on Android and a HTTP 
interface on Windows desktop (For now, the third party BLE service used does not support Windows desktop)

<img src="/assets/NET-MAUI.png">

I wanted to do a BLE Android App to support the Neopixel server, when I saw a .NET MAUI announcement, I decided 
that it would be a good experiment vs. using Android Studio or Xamarin. It also offered the possibility of single source 
Android, IOS, and / or MAC OS support.

BUT for the experiemnt, I also wanted to do use pure C#, so this example does not utilize any XAML or BLAZOR sources.

## Quick Start
This application is a client of an **esp32Neopxel** device. If one is not available, go to [Neopixel Server](/NeopixelServer) and create one of them first.

At the time of this writing, .NET MAUI requires Visual Studio 2022 Preview. That is sort of a moving target, so I recommend going directly to the [.NET MAUI documentation](https://docs.microsoft.com/en-us/dotnet/maui) to install the most appropriate development tools.

Once the desired version of Visual Studio has been installed, use it to open **NeopixelClient.sln**

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
