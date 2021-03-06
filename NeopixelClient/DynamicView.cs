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
        public static Color PageTextColor = Colors.Black;
        public static Thickness ContentsLabelPadding = new Thickness(7, 4);
        public static Color BorderColor = Color.FromArgb("#C49B33");

        IDynamicViewDevice device;

        Dictionary<string, ViewMetadata> options = new Dictionary<string, ViewMetadata>();
        Dictionary<object, String> mapping = new Dictionary<object, String>();
        Dictionary<String, object> controlMapping = new Dictionary<String, object>();

        /// <summary>
        /// State variable. Used to suppress events due to (during) data updates.
        /// </summary>
        bool updating = false;

        /// <summary>
        /// Constructor
        /// </summary>
        /// <param name="device"></param>
        public DynamicView(IDynamicViewDevice device)
        {
            this.device = device;
        }

        /// <summary>
        /// Create a dynamic view od the device from metadata array
        /// </summary>
        /// <param name="metadata"></param>
        /// <returns></returns>
        public View Create(ViewMetadata[] metadata)
        {
            var contents = new VerticalStackLayout()
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
                Content = contents
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
                    contents.Add(new Label
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
                    CreateView(contents, item.name, item);
                }
            }

            device.Updated += Status_updated;
            device.GetStatusAsync();
            return border;
        }

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
                };
                ctl.Add(row);
                Label newLabel = NewDefaultLabel(label);
                newLabel.HorizontalOptions = LayoutOptions.Start;
                row.Add(newLabel);
                var swt = new Switch
                {
                    WidthRequest = 50,
                    //BackgroundColor = Colors.LightGrey,
                };
                controlMapping[key] = swt;
                mapping[swt] = key;
                swt.Toggled += Swt_Toggled;
                row.Add(swt);
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
        /// Update control in dynamic view mapped to 'key' using 'value'
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
                                    result = false;
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
        #endregion
    }

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
        /// <item>composite - A set of ViewMetadata objs that combine to represent a compisite entity</item> 
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
}
