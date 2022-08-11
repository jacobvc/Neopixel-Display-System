# Neopixel Client

The Neopixel Client is a .NET MAUI applicaiton using a BLE interface on Android and a HTTP 
interface on Windows desktop (For now, the third party BLE service used does not support Windows desktop)

<img src="/assets/NET-MAUI.png">

I ewanted to do a BLE Android App to support the Neopixel server, when I saw a .NET MAUI announcement, and I decided 
that it would be a good experieent vs. Android Studio or Xamarin. It also offered the possibility of a singke source 
opportunity to support IOS and / or MAC OS.

BUT for the experiemnt, I also wanted to do something in pure C#, so this example does not utilize any XAML od BLAZOR sources.

## Starting a New Application
I'm sure there must be, or eventually will be a better way, but I found the the easiest way to start a nen app is to

* Use the 
new project wizard to create a new .NET MAUI application, then delete all of the .xaml and .xaml.cs files in the root directory

* create a new App.cs file that implements a trivial App

       public class App : Application
       {
          public App()
          {
              MainPage = new MainPage();
          }
       }

* Create a new MainPage.cs that implements the main ContentPage

       public partial class MainPage : ContentPage
       {
           public MainPage()
           {
           }
       }
