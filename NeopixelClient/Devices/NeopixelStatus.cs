using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace DynamicView
{
#pragma warning disable IDE1006 // Naming Styles
    /// <summary>
    /// Status values for Neopixel Device
    /// </summary>
    public class NeopixelStatus
    {
        public int textalign { get; set; } = 0;
        public int bmpalign { get; set; } = 0;
        public int txtclr { get; set; } = 0;
        public string text { get; set; } = "";
        public int bmp { get; set; } = 0;
        public int[] outputs { get; set; } = new int[0];
        public int[] leds { get; set; } = new int[0];
        public int[] ledcolora { get; set; } = new int[0];
        public int[] ledcolorb { get; set; } = new int[0];
        public double panelsw { get; set; } = 0;
        public double panelsh { get; set; } = 0;
        public string wxh { get; set; } = "";
    }
#pragma warning restore IDE1006 // Naming Styles
}
