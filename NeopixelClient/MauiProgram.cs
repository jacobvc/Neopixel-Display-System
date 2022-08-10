using CommunityToolkit.Maui;

namespace DynamicView;

public static class MauiProgram
{
	public static MauiApp CreateMauiApp()
	{
		var builder = MauiApp.CreateBuilder();
#if ANDROID
        // DOES NOT work for Windows
        builder.Services.AddBluetoothLE();
        builder.Services.AddSingleton<NeopixelBleScanner>();
#endif
		builder.UseMauiApp<App>().UseMauiCommunityToolkit();
		builder.ConfigureFonts(fonts =>
			{
				fonts.AddFont("OpenSans-Regular.ttf", "OpenSansRegular");
				fonts.AddFont("OpenSans-Semibold.ttf", "OpenSansSemibold");
			});

		return builder.Build();
	}
}
