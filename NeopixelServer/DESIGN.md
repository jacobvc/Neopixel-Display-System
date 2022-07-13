# Neopixel Server Design

TODO - Compose the design documentation ...

# Dynamic User Interface Generation

## ViewMetaData
**string name:**
Name of the metadata item (same as name of associated data)

**string[] names:**
If non-null, item value is an array, and 'names' are names for each array member

**string label:**
Suggested label for this item

**string type:**
Type of item being described. One of:
* bool - A boolean value
* select - A value that hase a specified set of textual values represented by their index
* text - A String value
* composite - A set of ViewMetadata objs that combine to represent a compisite entity
* value - A constant value to be displayed rather than connected to a control
* opts - Enumeration of a set of 'option' values

**string opts:**
In the case of type = 'select', the name of ViewMetaData item that enumerates the select options

**string[] values:**
Option value definition in the case of type = 'opts'

**string value:**
In the case of type = 'value', its constant value

**ViewMetadata[] fields:**
In the case of type = 'composite', the ViewMetadata items that comprise the composite item
