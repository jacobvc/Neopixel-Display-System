using Microsoft.Maui;
using Microsoft.Maui.Controls;
using Microsoft.Maui.Controls.Shapes;
using Microsoft.Maui.Graphics;

namespace DynamicView
{
    public partial class MainPage : ContentPage
    {
        StackBase initialView;

        public MainPage(
#if ANDROID
            NeopixelBleScanner scanner
#endif
            )
        {
            DynamicViewDeviceCollection Devices
#if ANDROID
                = scanner.Devices;
#else
                = new DynamicViewDeviceCollection();
#endif
            BackgroundColor = Colors.LightGrey;
            initialView = new VerticalStackLayout()
            {
                Spacing = 20,
                HorizontalOptions = LayoutOptions.Center,

            };

            //Devices.Add(new NeopixelHttpDevice("192.168.12.208"));

            CollectionView cv = new CollectionView()
            {
                ItemsSource = Devices,
                EmptyView = "Scanning for Neopixel Devices",
                //ItemsUpdatingScrollMode = ItemsUpdatingScrollMode.KeepLastItemInView,
            };
            cv.ItemTemplate = BleTemplate();

            //DataTemplate viewCell = new DataTemplate(typeof(StatusCell));
            //ListView listView = new ListView()
            //{

            //};
            //listView.ItemTemplate = viewCell;

            initialView.Add(cv);
#if !ANDROID
            var row = new HorizontalStackLayout();
            initialView.Add(row);
            row.Add(new Label
            {
                Text = "NeoPixel URL ",
                Padding = 16,
                HorizontalOptions = LayoutOptions.Center,
            });
            var url = new Entry
            {
                Text = "192.168.12.208",
                HorizontalOptions = LayoutOptions.Center,
                TextColor = Colors.Black
            };
            row.Add(url);
            SemanticProperties.SetHeadingLevel((BindableObject)initialView.Children[initialView.Children.Count - 1], SemanticHeadingLevel.Level1);

            var button = new Button
            {
                Text = "Connect",
                HorizontalOptions = LayoutOptions.Center,
                TextColor = Colors.White,
                BackgroundColor = Color.FromArgb("#2b0b98"),
                Padding = new Thickness(10, 7)
            };
            button.Clicked += async (sender, args) =>
            {
                NeopixelHttpDevice transport = new NeopixelHttpDevice(url.Text);

                ViewMetadata[] mdata = await transport.GetMetaDataAsync();

                CreateDynamicView(mdata, transport);
            };
            row.Add(button);
#endif
            Content = initialView;
        }

        //public class StatusCell : ViewCell { }
        DataTemplate BleTemplate()
        {
            DataTemplate tpl = new DataTemplate(() =>
            {

                StackBase grid = new VerticalStackLayout()
                {
                    Spacing = 10,
                    HorizontalOptions = LayoutOptions.Start,

                };
                Border border = new Border
                {
                    Stroke = DynamicView.BorderColor,
                    Background = Colors.Cornsilk,
                    StrokeThickness = 4,
                    Padding = new Thickness(16, 8),
                    HorizontalOptions = LayoutOptions.Center,
                    WidthRequest = 350,
                    StrokeShape = new RoundRectangle
                    {
                        CornerRadius = new CornerRadius(10, 10, 10, 10)
                    },
                    Content = grid
                };

                //Image image = new Image { Aspect = Aspect.AspectFill, HeightRequest = 60, WidthRequest = 60 };
                //image.SetBinding(Image.SourceProperty, "ImageUrl");
                //grid.Add(image);

                Label nameLabel = new Label { FontAttributes = FontAttributes.Bold, FontSize = 16 };
                nameLabel.SetBinding(Label.TextProperty, "DeviceName");
                grid.Add(nameLabel);

                Label locationLabel = new Label { FontAttributes = FontAttributes.Italic, FontSize = 16, VerticalOptions = LayoutOptions.End };
                locationLabel.SetBinding(Label.TextProperty, "Address");
                grid.Add(locationLabel);

                Label rssiLabel = new Label { FontAttributes = FontAttributes.Italic, FontSize = 16, VerticalOptions = LayoutOptions.End };
                rssiLabel.SetBinding(Label.TextProperty, "Rssi");
                grid.Add(rssiLabel);

                var button = new Button
                {
                    Text = "Connect",
                    HorizontalOptions = LayoutOptions.Center,
                    //TextColor = Colors.White,
                    //BackgroundColor = Color.FromArgb("#2b0b98"),
                    Padding = new Thickness(15),
                    BindingContext = grid,
                };
                grid.Add(button);
                button.Clicked += async (sender, args) =>
                {
                    IDynamicViewDevice device = (sender as Element).Parent.BindingContext as IDynamicViewDevice;
                    Button btn = sender as Button;
                    btn.Text = "Connecting ...";
                    if (await (device).ConnectAsync())
                    {
                        ViewMetadata[] mdata = await device.GetMetaDataAsync();
                        btn.Text = "Connect";
                        CreateDynamicView(mdata, device);
                    }
                    else
                    {
                        btn.Text = "Connect";
                    }
                };
                return border;
            });
            return tpl;
        }

        async void CreateDynamicView(ViewMetadata[] mdata, IDynamicViewDevice device)
        {
            if (mdata.Length > 0)
            {
                DynamicView view = new DynamicView(device, this);

                StackBase layout = new VerticalStackLayout()
                {
                    Spacing = 20,
                    HorizontalOptions = LayoutOptions.Center,
                };

                layout.Add(view.Create(mdata));

                var button = new Button
                {
                    Text = "Disconnect",
                    HorizontalOptions = LayoutOptions.Center,
                    TextColor = Colors.White,
                    BackgroundColor = Color.FromArgb("#2b0b98"),
                    Padding = new Thickness(14, 10)
                };
                button.Clicked += (sender, args) =>
                {
                    Content = initialView;
                };
                layout.Add(button);
                ScrollView sv = new ScrollView()
                {
                    Padding = new Thickness(0, 8),
                    Orientation = ScrollOrientation.Horizontal,
                    Content = layout
                };
                Content = sv;
            }
            else
            {
                await DisplayAlert("Alert", "Could not connect to device", "OK");
            }
        }
    }
}