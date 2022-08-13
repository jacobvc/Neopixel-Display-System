using CommunityToolkit.Maui.Views;
using Microsoft.Maui.Controls.Shapes;
using System;
using System.Collections.Generic;
using System.Linq;
using System.Reflection;
using System.Security.Cryptography;
using System.Text;
using System.Threading.Tasks;

namespace DynamicView
{
    public class DynamicView
    {
        #region Variables
        public static readonly Color PageTextColor = Colors.Black;
        public static readonly Thickness ContentsLabelPadding = new Thickness(3, 3);
        public static readonly Color BorderColor = Color.FromArgb("#C49B33");

        ContentPage page = null;
        IDynamicViewDevice device;
        StackBase view;

        Dictionary<string, ViewMetadata> options = new Dictionary<string, ViewMetadata>();
        Dictionary<object, String> mapping = new Dictionary<object, String>();
        Dictionary<String, object> controlMapping = new Dictionary<String, object>();

        /// <summary>
        /// State variable. Used to suppress events due to (during) data updates.
        /// </summary>
        bool updating = false;
        #endregion

        #region Constructors
        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device"></param>
        public DynamicView(IDynamicViewDevice device, ContentPage page)
        {
            this.device = device;
            this.page = page;
        }
        #endregion

        #region Create THE view
        /// <summary>
        /// Create dynamic view of device from metadata array
        /// </summary>
        /// <param name="metadata"></param>
        /// <returns></returns>
        public View Create(ViewMetadata[] metadata)
        {
            view = new VerticalStackLayout()
            {
                Spacing = 10,
                HorizontalOptions = LayoutOptions.Center,
            };
            Border border = new Border
            {
                Stroke = BorderColor,
                Background = Colors.Cornsilk,
                StrokeThickness = 4,
                Padding = new Thickness(8, 8),
                HorizontalOptions = LayoutOptions.Center,
                StrokeShape = new RoundRectangle
                {
                    CornerRadius = new CornerRadius(30, 30, 30, 30)
                },
                Content = view
            };
            // Instantiate all of the options lists before creating controls
            foreach (ViewMetadata item in metadata)
            {
                var type = item.type;
                if (type == "opts")
                {
                    options[item.name] = item;
                }
            }
            // Create item views described by metadata
            foreach (ViewMetadata item in metadata)
            {
                var type = item.type;
                if (type == "opts")
                {
                    // Already handled this. ignore
                }
                else if (type == "value")
                {
                    view.Add(new Label
                    {
                        Text = item.value,
                        Padding = ContentsLabelPadding,
                        FontSize = 32,
                        HorizontalOptions = LayoutOptions.Center,
                        TextColor = PageTextColor
                    });
                }
                else
                {
                    CreateView(view, item.name, item);
                }
            }

            device.Updated += Status_updated;
            device.GetStatusAsync();
            return border;
        }
        #endregion

        #region Create view of ViewMetadata item
        /// <summary>
        /// Create view of 'item', mapped using 'key' and attach it to 'ctl'
        /// </summary>
        /// <param name="ctl"></param>
        /// <param name="key"></param>
        /// <param name="item"></param>
        protected virtual void CreateView(StackBase ctl, String key, ViewMetadata item)
        {
            if (item.type == "composite")
            {
                var row = new HorizontalStackLayout();
                ctl.Add(row);
                for (var i = 0; i < item.fields.Length; ++i)
                {
                    CreateView(row, item.fields[i].name, item.fields[i]);
                }
            }
            else if (item.type == "collapsible")
            {
                var section = new VerticalStackLayout
                {
                    Padding = new Thickness(4, 4)
                };
                var row = new HorizontalStackLayout();
                section.Add(row);

                Border border = new Border
                {
                    Stroke = DynamicView.BorderColor,
                    Background = Colors.Cornsilk,
                    StrokeThickness = 2,
                    //Padding = new Thickness(6, 6),
                    HorizontalOptions = LayoutOptions.Fill,
                    ZIndex = 0,
                    //WidthRequest = 350,
                    StrokeShape = new RoundRectangle
                    {
                        CornerRadius = new CornerRadius(5, 5, 5, 5)
                    },
                    Content = section
                };
                ctl.Add(border);

                Label newLabel = NewDefaultLabel("Show " + item.label);
                newLabel.HorizontalOptions = LayoutOptions.Start;
                row.Add(newLabel);
                var swt = new Switch
                {
                    WidthRequest = 50,
                };
                var collapse = new VerticalStackLayout { IsVisible = false };
                swt.Toggled += (sender, e) =>
                {
                    collapse.IsVisible = e.Value;
                };
                row.Add(swt);
                section.Add(collapse);
                for (var i = 0; i < item.fields.Length; ++i)
                {
                    CreateView(collapse, item.fields[i].name, item.fields[i]);
                }

            }
            else if (item.names != null)
            {
                var row = new HorizontalStackLayout();
                ctl.Add(row);
                row.Add(NewDefaultLabel(item.label));
                for (var i = 0; i < item.names.Length; ++i)
                {
                    CreateControl(row, key + "[" + i + "]", item.type, item.names[i], item.opts);
                }
            }
            else
            {
                CreateControl(ctl, key, item.type, item.label, item.opts);
            }
        }

        /// <summary>
        /// Create an element (typically a label and a control) for data of 'metaType' using 'label'
        /// for the label and 'optionsName' for options in the case of metatype='select'. Attach it to 'ctl'
        /// </summary>
        /// <param name="ctl"></param>
        /// <param name="key"></param>
        /// <param name="metaType"></param>
        /// <param name="label"></param>
        /// <param name="optionsName"></param>
        protected virtual void CreateControl(StackBase ctl, String key, String metaType, String label, String optionsName)
        {
            if (metaType == "bool")
            {
                var row = new VerticalStackLayout()
                {
                    HorizontalOptions = LayoutOptions.Center,
#if ANDROID
                    WidthRequest = 60
#else
                    // Keep Windows On/Off label from being cropped
                    WidthRequest = 100
#endif
                };
                ctl.Add(row);
                Label newLabel = NewDefaultLabel(label);
                newLabel.HorizontalOptions = LayoutOptions.Start;
                row.Add(newLabel);
                var swt = new Switch
                {
                    //BackgroundColor = Colors.LightGrey,
                };
                controlMapping[key] = swt;
                mapping[swt] = key;
                swt.Toggled += Swt_Toggled;
                row.Add(swt);
            }
            else if (metaType == "color")
            {
                var color = new Button
                {
                    Text = label
                };
                controlMapping[key] = color;
                mapping[color] = key;
                ctl.Add(color);
                color.Clicked += async (sender, args) =>
                {
                    Button btn = sender as Button;
                    ColorPicker picker = new ColorPicker(btn.Text, btn.BackgroundColor, key, btn);
                    picker.changed += Color_changed;
                    page.ShowPopup(picker);
                };
            }
            else if (metaType == "select")
            {
                ctl.Add(NewDefaultLabel(label));
                var select = new Picker
                {
                    ItemsSource = options[optionsName].values,
                };
                controlMapping[key] = select;
                mapping[select] = key;
                select.SelectedIndexChanged += Select_SelectedIndexChanged;
                ctl.Add(select);
            }
            else
            {
                var row = new HorizontalStackLayout();
                ctl.Add(row);
                row.Add(NewDefaultLabel(label));
                var text = new Entry
                {
                    HorizontalOptions = LayoutOptions.Center,
                    WidthRequest = 110,
                };
                controlMapping[key] = text;
                mapping[text] = key;
                text.TextChanged += Text_TextChanged;
                row.Add(text);
            }
        }

        /// <summary>
        /// Create a new Label with default formatting and 'label' text
        /// </summary>
        /// <param name="label"></param>
        /// <returns></returns>
        protected virtual Label NewDefaultLabel(String label)
        {
            return new Label
            {
                Text = label,
                Padding = ContentsLabelPadding,
                //FontSize = 32,
                HorizontalOptions = LayoutOptions.Center,
                TextColor = PageTextColor
            };
        }
        #endregion

        #region Update control value
        /// <summary>
        /// Update control in dynamic view mapped to 'key', with 'value'
        /// </summary>
        /// <param name="key"></param>
        /// <param name="value"></param>
        /// <returns></returns>
        bool UpdateControl(String key, object value)
        {
            object ctl;
            bool result = true;
            if (controlMapping.TryGetValue(key, out ctl))
            {
                MainThread.BeginInvokeOnMainThread(() =>
                {
                    Switch sw = ctl as Switch;
                    if (sw != null) { sw.IsToggled = 0 != (int)value; }
                    else
                    {
                        Picker p = ctl as Picker;
                        if (p != null) { p.SelectedIndex = (int)value; }
                        else
                        {
                            Editor e = ctl as Editor;
                            if (e != null)
                            {
                                if (!e.IsFocused)
                                {
                                    e.Text = (String)value;
                                }
                            }
                            else
                            {
                                Entry en = ctl as Entry;
                                if (en != null)
                                {
                                    if (!en.IsFocused)
                                    {
                                        en.Text = (String)value;
                                    }
                                }
                                else
                                {
                                    Button cp = ctl as Button;
                                    if (cp != null)
                                    {
                                        // Works, but there is probably a better way
                                        cp.BackgroundColor = Color.FromInt((int)(0xff000000 | (int)value));
                                        cp.TextColor = ColorPicker.SugestedTextColor(cp.BackgroundColor);
                                    }
                                    else
                                    {
                                        result = false;
                                    }
                                }
                            }
                        }
                    }
                });
            }
            else
            {
                result = false;
            }
            return result;
        }
        #endregion

        #region Event Handlers
        private void Status_updated(object sender, StatusUpdatedEventArgs e)
        {
            updating = true;
            Type type = e.status.GetType();
            PropertyInfo[] propertyInfo = type.GetProperties(BindingFlags.Public | BindingFlags.Instance);
            foreach (PropertyInfo info in propertyInfo)
            {
                object o = info.GetValue(e.status, null);
                if (o.GetType().IsArray)
                {
                    int[] val = o as int[];
                    for (int i = 0; i < val.Length; ++i)
                    {
                        UpdateControl(info.Name + "[" + i + "]", val[i]);
                    }
                }
                else
                {
                    UpdateControl(info.Name, o);
                }
            }
            updating = false;
        }

        private void Swt_Toggled(object sender, ToggledEventArgs e)
        {
            if (!updating)
            {
                Switch swt = sender as Switch;
                String name;
                if (mapping.TryGetValue(sender, out name))
                {
                    device.Submit(name, swt.IsToggled.ToString());
                }
            }
        }

        private void Text_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!updating)
            {
                Entry text = sender as Entry;
                String name;
                if (mapping.TryGetValue(sender, out name))
                {
                    device.Submit(name, text.Text);
                }
            }
        }

        private void Select_SelectedIndexChanged(object sender, EventArgs e)
        {
            if (!updating)
            {
                Picker picker = sender as Picker;
                String name;
                if (mapping.TryGetValue(sender, out name))
                {
                    device.Submit(name, picker.SelectedIndex.ToString());
                }
            }
        }

        private void Color_changed(object sender, ColorPicker.ColorChangedEventArgs e)
        {
            if (!updating)
            {
                ColorPicker picker = sender as ColorPicker;
                device.Submit(picker.Key, e.color.ToInt().ToString());
            }
        }

        #endregion
    }
    #region ViewMetadata class
#pragma warning disable IDE1006 // Naming Styles
    /// <summary>
    /// Metadata describing user interface alements
    /// </summary>
    public class ViewMetadata
    {
        /// <summary>
        /// Name of Metadata item
        /// </summary>
        public string name { get; set; }
        /// <summary>
        /// If non-null, this metadata item describes array of elements,
        /// all of the same type
        /// </summary>
        public string[] names { get; set; }
        /// <summary>
        /// UI label suggested for this item
        /// </summary>
        public string label { get; set; }
        /// <summary>
        /// Type of data being described. One of:
        /// <list type="bullet">
        /// <item>bool - A boolean value</item> 
        /// <item>select - A value that hase a specified set of textual values represented by their index</item> 
        /// <item>text - A String value</item> 
        /// <item>composite - A set of ViewMetadata objs that combine to represent a composite entity</item> 
        /// <item>collapsible - A set of ViewMetadata objs that should be in a collapsible section</item> 
        /// <item>value - A constant value to be displayed rather than connected to a control</item> 
        /// <item>opts - Enumeration of a set of 'option' values</item> 
        /// </list>
        /// </summary>
        public string type { get; set; }
        /// <summary>
        /// In the case of type 'select' name of Metadata containing options
        /// </summary>
        public string opts { get; set; }
        /// <summary>
        /// In the case of type 'opts', array of option values
        /// </summary>
        public string[] values { get; set; }
        /// <summary>
        /// In the case of type 'value', the (string) value
        /// </summary>
        public string value { get; set; }
        /// <summary>
        /// In the case of type 'composite', ViewMetadata subfields
        /// </summary>
        public ViewMetadata[] fields { get; set; }

        public override string ToString()
        {
            String s = name + "(" + type + ") ";
            if (fields != null)
            {
                s += "fields: { ";
                foreach (ViewMetadata md in fields)
                {
                    s += md.ToString() + ", ";
                }
                s += "} ";
            }
            if (names != null)
            {
                s += "names: { ";
                foreach (String name in names)
                {
                    s += name + ", ";
                }
                s += "} ";
            }
            if (values != null)
            {
                s += "values: { ";
                foreach (String name in values)
                {
                    s += name + ", ";
                }
                s += "} ";
            }
            return s;
        }
    }
#pragma warning restore IDE1006 // Naming Styles
    #endregion
}
