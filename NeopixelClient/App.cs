using Microsoft.Maui;
#if ANDROID
using triaxis.BluetoothLE;
#endif
#if WINDOWS
using Microsoft.UI;
using Microsoft.UI.Windowing;
using Windows.Graphics;
#endif
namespace DynamicView
{
    public class App : Application
    {
        // Has to appear as a formal parameter (but does not need to be referenced) in order for the
        //   builder.Services.AddSingleton<BluetoothScanner>();
        // to instantiate.
#if ANDROID
        public App(NeopixelBleScanner scanner)
        {
            MainPage = new MainPage(scanner);
        }
#else
        public App()
        {
#if WINDOWS
            Microsoft.Maui.Handlers.WindowHandler.Mapper.AppendToMapping(nameof(IWindow), (handler, view) =>
            {
                var mauiWindow = handler.VirtualView;
                var nativeWindow = handler.PlatformView;
                nativeWindow.Activate();
                IntPtr windowHandle = WinRT.Interop.WindowNative.GetWindowHandle(nativeWindow);
                WindowId windowId = Microsoft.UI.Win32Interop.GetWindowIdFromWindow(windowHandle);
                AppWindow appWindow = Microsoft.UI.Windowing.AppWindow.GetFromWindowId(windowId);
                appWindow.Resize(new SizeInt32(630, 500));
            });
#endif
           MainPage = new MainPage();
        }
#endif
    }
}