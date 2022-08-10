using Microsoft.Maui.Controls.Shapes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using CommunityToolkit.Maui.Views;

namespace DynamicView
{
    public partial class ColorPicker : Popup
    {
        public delegate void ColorChanged(object sender, ColorChangedEventArgs e);
        public class ColorChangedEventArgs : EventArgs
        {
            public Color color;
            public ColorChangedEventArgs(Color color)
            {
                this.color = color;
            }
        }
        public event ColorChanged changed;

        BoxView bvColorPicker;
        Label lblName;
        Grid bgColor;

        public Color SelectedColor
        {
            get { return this.bvColorPicker.Color; }
            set { this.bvColorPicker.Color = value; /* this.pickerColorPicker.BackgroundColor = value; */ }
        }

        public Layout Picker
        {
            get; private set;
        }
        public String Key { get; private set; }

        Layout ColorGrid()
        {
            int rows = 6;
            int cols = 3;
            int cellsize = 35;
            int pad = 0;

            Layout layout = new AbsoluteLayout
            {
                Margin = new Thickness(10),
                //WidthRequest = 200,
                //HeightRequest = 200,
                //IsVisible = false,
            };

            Grid grid = new Grid();
            grid.Padding = pad;
            layout.Add(grid);
            AbsoluteLayout.SetLayoutBounds(grid, new Rect(0, 0, cols * (cellsize + 2 * pad + 2),
                rows * (cellsize + 5)));

            for (int i = 0; i < rows; i++)
            {
                grid.RowDefinitions.Add(new RowDefinition { Height = cellsize });
            }
            for (int i = 0; i < cols; i++)
            {
                grid.ColumnDefinitions.Add(new ColumnDefinition { Width = cellsize });
            }

            int x = 0;
            int y = 0;
            foreach(KeyValuePair<String, Color> entry in colorDict)
            {
                if (x < cols)
                {
                    //grid.Add(new BoxView
                    //{
                    //    Color = entry.Value,
                    //});
                    Button btn = new Button
                    {
                        BackgroundColor = entry.Value,
                        CornerRadius = 0,
                    };
                    btn.Clicked += Btn_Clicked;
                    grid.Add(btn, x, y);
                    ++y;
                    if (y >= rows)
                    {
                        y = 0;
                        ++x;
                    }
                }
                else
                {
                    break;
                }

            }
            return layout;
        }

        public ColorPicker(String label, Color color, string key)
        {
#if true
            Picker = ColorGrid();

            Grid grid = new Grid();
            grid.RowDefinitions.Add(new RowDefinition());
            grid.RowDefinitions.Add(new RowDefinition());
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = 50 });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = 50 });
            grid.Padding = new Thickness(0, 3);

            lblName = new Label
            {
                Text = label,
                Padding = DynamicView.ContentsLabelPadding,
                HorizontalOptions = LayoutOptions.Center,
                TextColor = DynamicView.PageTextColor
            };
            grid.Add(lblName, 0, 0);
            bgColor = new Grid
            {
            };

            grid.Add(bgColor, 1, 0);
            bvColorPicker = new BoxView
            {
                BackgroundColor = Colors.Gray,
                //HorizontalOptions = LayoutOptions.FillAndExpand,
                //VerticalOptions = LayoutOptions.FillAndExpand,
            };
            bgColor.Add(bvColorPicker);

            grid.Add(Picker, 0, 1);
            Grid.SetColumnSpan(Picker, 2);

            Content = grid;
            //Layout layout = new VerticalStackLayout();
            //layout.Add(grid);

            //Border border = new Border
            //{
            //    Stroke = Brush.Default;
            //    //Background = Colors.Cornsilk;
            //    StrokeThickness = 2;
            //    Padding = new Thickness(2, 2);
            //    HorizontalOptions = LayoutOptions.Center;
            //    StrokeShape = new RoundRectangle
            //    {
            //        CornerRadius = new CornerRadius(5, 5, 5, 5)
            //    };
            //    Content = layout;
            //};
            //Add(border);
            this.SelectedColor = color;

            // create the TapGestureRecognizer
            TapGestureRecognizer tapImgColorPicker = new TapGestureRecognizer();
            bvColorPicker.GestureRecognizers.Add(tapImgColorPicker);
            tapImgColorPicker.Tapped += TapImgColorPicker_Tapped;
            Key = key;
#endif
        }

        // Dictionary to get Color from color name.
        Dictionary<string, Color> colorDict = new Dictionary<string, Color>
        {
           /* { "Default", Color.Default }, */                 { "Amber", Color.FromArgb("#FFC107") },
            { "Black", Color.FromArgb("#212121") },         { "Blue", Color.FromArgb("#2196F3") },
            { "Blue Grey", Color.FromArgb("#607D8B") },     { "Brown", Color.FromArgb("#795548") },
            { "Cyan", Color.FromArgb("#00BCD4") },          { "Dark Orange", Color.FromArgb("#FF5722") },
            { "Dark Purple", Color.FromArgb("#673AB7") },   { "Green", Color.FromArgb("#4CAF50") },
            { "Grey", Color.FromArgb("#9E9E9E") },          { "Indigo", Color.FromArgb("#3F51B5") },
            { "Light Blue", Color.FromArgb("#02A8F3") },    { "Light Green", Color.FromArgb("#8AC249") },
            { "Lime", Color.FromArgb("#CDDC39") },          { "Orange", Color.FromArgb("#FF9800") },
            { "Pink", Color.FromArgb("#E91E63") },          { "Purple", Color.FromArgb("#94499D") },
            { "Red", Color.FromArgb("#D32F2F") },           { "Teal", Color.FromArgb("#009587") },
            { "White", Color.FromArgb("#FFFFFF") },         { "Yellow", Color.FromArgb("#FFEB3B") },
        };

        public Boolean IsOpened
        {
            get { return Picker.IsVisible; }
            set
            {
                //lblName.IsVisible = !value;
                bgColor.IsVisible = !value;
                Picker.IsVisible = value;
            }
        }

        private void TapImgColorPicker_Tapped(object sender, EventArgs e)
        {
            IsOpened = !IsOpened;
        }

        private void Btn_Clicked(object sender, EventArgs e)
        {
            Button btn = sender as Button;
            changed?.Invoke(this, new ColorChangedEventArgs(btn.BackgroundColor));
            this.SelectedColor = btn.BackgroundColor;
            IsOpened = false;
        }
    }
}
