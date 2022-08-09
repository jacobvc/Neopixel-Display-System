using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DynamicView
{
    public partial class ColorPicker : Grid
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

        public Color Color
        {
            get { return this.bvColorPicker.Color; }
            set { this.bvColorPicker.Color = value; /* this.pickerColorPicker.BackgroundColor = value; */ }
        }

        public Layout Picker
        {
            get; private set;
        }

        Layout ColorGrid()
        {
            int rows = 4;
            int cols = 4;
            int cellsize = 35;
            int pad = 4;

            Layout layout = new AbsoluteLayout
            {
                Margin = new Thickness(10),
                //WidthRequest = 200,
                //HeightRequest = 200,
                IsVisible = false,
            };

            Grid grid = new Grid();
            grid.Padding = pad;
            layout.Add(grid);
            AbsoluteLayout.SetLayoutBounds(grid, new Rect(10, 10, cols * (cellsize + 2 * pad + 2),
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

        public ColorPicker()
        {
            Picker = ColorGrid();

            RowDefinitions.Add(new RowDefinition { Height = 28 });
            RowDefinitions.Add(new RowDefinition());
            ColumnDefinitions.Add(new ColumnDefinition { Width = 58 });
            //ColumnDefinitions.Add(new ColumnDefinition { Width = 38 });
            this.Padding = new Thickness(0, 3);

            Grid boxgrid = new Grid
            {
            };
            this.Add(boxgrid, 0, 0);
            bvColorPicker = new BoxView
            {
                BackgroundColor = Colors.Gray,
                //HorizontalOptions = LayoutOptions.FillAndExpand,
                //VerticalOptions = LayoutOptions.FillAndExpand,
            };
            boxgrid.Add(bvColorPicker);

            this.Add(Picker, 0, 1);
            // create the TapGestureRecognizer
            TapGestureRecognizer tapImgColorPicker = new TapGestureRecognizer();
            bvColorPicker.GestureRecognizers.Add(tapImgColorPicker);
            tapImgColorPicker.Tapped += TapImgColorPicker_Tapped;
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

        private void TapImgColorPicker_Tapped(object sender, EventArgs e)
        {
            Picker.IsVisible = !Picker.IsVisible;
        }

        private void Btn_Clicked(object sender, EventArgs e)
        {
            Button btn = sender as Button;
            changed?.Invoke(this, new ColorChangedEventArgs(btn.BackgroundColor));
            this.Color = btn.BackgroundColor;
            Picker.IsVisible = false;
        }
    }
}
