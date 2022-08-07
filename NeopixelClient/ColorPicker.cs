using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DynamicView
{
    public partial class ColorPicker : Grid
    {
        BoxView bvColorPicker;
        Image imgColorPicker;
        Picker pickerColorPicker;

        public Color Color
        {
            get { return this.bvColorPicker.Color; }
            set { this.bvColorPicker.Color = value; }
        }

        public ColorPicker()
        {
            RowDefinitions.Add(new RowDefinition { Height = 38 });
            ColumnDefinitions.Add(new ColumnDefinition { Width = 38 });
            ColumnDefinitions.Add(new ColumnDefinition { Width = 38 });
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
            imgColorPicker = new Image
            {
            };
            this.Add(imgColorPicker, 1, 0);
            //this.SetColumnSpan(imgColorPicker, 2);
            pickerColorPicker = new Picker
            {
                IsVisible = false,
            };
            pickerColorPicker.SelectedIndexChanged += pickerColorPicker_SelectedIndexChanged;
            this.SetColumnSpan(pickerColorPicker, 2);
            this.Add(pickerColorPicker, 0, 0);



            // creating the TapGestureRecognizer
            TapGestureRecognizer tapImgColorPicker = new TapGestureRecognizer();
            imgColorPicker.GestureRecognizers.Add(tapImgColorPicker);
            tapImgColorPicker.Tapped += TapImgColorPicker_Tapped;

            // set the image from embedded resource
            imgColorPicker.Source = ImageSource.FromFile("colorpicker/ColorPicker.png");
           
            // populate picker with available colors
            foreach (string colorName in colorDict.Keys)
            {
                pickerColorPicker.Items.Add(colorName);
            }
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
            pickerColorPicker.IsVisible = true;
            pickerColorPicker.Focus();
        }

        void pickerColorPicker_SelectedIndexChanged(object sender, EventArgs e)
        {
            string colorName = pickerColorPicker.Items[pickerColorPicker.SelectedIndex];
            this.Color = colorDict[colorName];
            pickerColorPicker.IsVisible = false;
        }
    }
}
