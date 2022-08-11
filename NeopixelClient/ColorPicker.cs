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
        #region Event Definitions
        public delegate void ColorChanged(object sender, ColorChangedEventArgs e);
        public class ColorChangedEventArgs : EventArgs
        {
            public Color color;
            public ColorChangedEventArgs(Color color)
            {
                this.color = color;
            }
        }
        #endregion

        #region Variables and Porperties
        public event ColorChanged changed;
        Label lblSelected;
        Label lblOriginal;

        public Color SelectedColor
        {
            get { return this.lblSelected.BackgroundColor; }
            set
            {
                this.lblSelected.BackgroundColor = value;
                this.lblSelected.Text = "Selected\n" + value.ToArgbHex();
                this.lblSelected.TextColor = SugestedTextColor(value);
            }
        }

        public String Key { get; private set; }

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
        #endregion

        #region Constructor / Helpers
        /// <summary>
        /// Create a ColorPicker Popup labeled 'label' with 'color', remembering 
        /// parameter 'key'. Anchor the picker at 'anchor'
        /// </summary>
        /// <param name="label"></param>
        /// <param name="color"></param>
        /// <param name="key"></param>
        /// <param name="anchor"></param>
        public ColorPicker(String label, Color color, string key, View anchor)
        {
            int width = 200;
            int height = 240;
            Key = key;
            this.Anchor = anchor;
            this.Size = new Size(width, height);

            Grid grid = new Grid();
            grid.RowDefinitions.Add(new RowDefinition { Height = 45 });
            grid.RowDefinitions.Add(new RowDefinition());
            grid.RowDefinitions.Add(new RowDefinition { Height = 35 });
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = width / 2 } );
            grid.ColumnDefinitions.Add(new ColumnDefinition { Width = width / 2 } );
            grid.Padding = new Thickness(0, 3);

            
            lblOriginal = new Label
            {
                Text = label + "\n" + color.ToArgbHex(),
                Margin = 3,
                HorizontalOptions = LayoutOptions.Fill,
                VerticalOptions = LayoutOptions.Fill,
                HorizontalTextAlignment = TextAlignment.Center,
            };
            grid.Add(lblOriginal, 0, 0);
            this.lblOriginal.BackgroundColor = color;
            this.lblOriginal.TextColor = SugestedTextColor(color);

            lblSelected = new Label
            {
                Text = "Selected",
                Margin = 3,
                HorizontalOptions = LayoutOptions.Fill,
                VerticalOptions = LayoutOptions.Fill,
                HorizontalTextAlignment = TextAlignment.Center,
            };
            grid.Add(lblSelected, 1, 0);

            Layout pickLayout = new VerticalStackLayout
            {
                HorizontalOptions = LayoutOptions.Center,
                VerticalOptions = LayoutOptions.Center,
            };
            pickLayout.Add(ColorGrid(width));
            grid.Add(pickLayout, 0, 1);
            Grid.SetColumnSpan(pickLayout, 2);

            Button cancel = new Button
            {
                Text = "Cancel",
                Margin = new Thickness(4, 1),
            };
            cancel.Clicked += async (sender, args) =>
            {
                Close(false);
            };
            grid.Add(cancel, 0, 2);
            Button ok = new Button
            {
                Text = "OK",
                Margin = new Thickness(4, 1),
            };
            ok.Clicked += async (sender, args) =>
            {
                changed?.Invoke(this, new ColorChangedEventArgs(SelectedColor));
                Close(true);
            };
            grid.Add(ok, 1, 2);
            Content = grid;

            this.SelectedColor = color;
        }

        /// <summary>
        /// Create a grid of buttons each with a specified color, intended to fit in 'width'
        /// </summary>
        /// <param name="width"></param>
        /// <returns></returns>
        Grid ColorGrid(int width)
        {
            int rows = 4;
            int cols = 5;
            int pad = 2;

            int cellsize = (int)((Double)width / cols) - 2 * pad;

            Grid grid = new Grid();
            grid.Padding = pad;

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
            foreach (KeyValuePair<String, Color> entry in colorDict)
            {
                if (x < cols)
                {
                    Button btn = new Button
                    {
                        BackgroundColor = entry.Value,
                        CornerRadius = 0,
                    };
                    btn.Clicked += ColorBtn_Selected;
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
            return grid;
        }
        #endregion

        #region Static Text color utility
        // https://www.w3.org/TR/WCAG20/#relativeluminancedef
        /// <summary>
        /// Calculate the luminance component for one channel (r, g, or b)
        /// </summary>
        /// <param name="channel"></param>
        /// <returns></returns>
        static Double ChannelLum(Byte channel)
        {
            Double dChan = channel / 255.0;
            if (dChan <= 0.03928)
            {
                return dChan / 12.92;
            }
            else
            {
                return Math.Pow((dChan + 0.055) / 1.055, 2.4);
            }
        }

        /// <summary>
        /// Compute the suggested text color for the specified background
        /// </summary>
        /// <param name="background"></param>
        /// <returns></returns>
        public static Color SugestedTextColor(Color background)
        {
            Byte r, g, b;
            background.ToRgb(out r, out g, out b);
            Double lum = 0.2126 * ChannelLum(r) + 0.7152 * ChannelLum(g) + 0.0722 * ChannelLum(b);

            return lum > 0.175 ? Colors.Black : Colors.White;
        }
        #endregion

        #region Event handlers
        private void ColorBtn_Selected(object sender, EventArgs e)
        {
            Button btn = sender as Button;
            this.SelectedColor = btn.BackgroundColor;
        }
        #endregion
    }
}
