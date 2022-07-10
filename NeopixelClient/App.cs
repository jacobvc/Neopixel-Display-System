using Microsoft.Maui;
#if ANDROID
using triaxis.BluetoothLE;
#endif

namespace DynamicView
{
    public class App : Application
    {
        // Has to appear as a formal parameter (but does not need to be referenced) in order for the
        //   builder.Services.AddSingleton<BluetoothScanner>();
        // to instantiate.
        public App(NeopixelBleScanner scanner)
        {
            MainPage = new MainPage(scanner);
        }
#else
        public App()
        {
            MainPage = new MainPage();
        }
#endif
    }
}