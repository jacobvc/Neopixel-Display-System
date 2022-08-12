# OBS Studio Script

The OBS Studio script is a python script that allows control of any / 
all of the provided features as a result of activation and / or deactivation 
of any scene. The user interface also supports enabling / disabling the 
processing of OBS Studio scenes.

It controls the Neopixel device found at the specified IP address.

This script is a client of an **esp32Neopxel** device. If one is not available, go to [Neopixel Server](/NeopixelServer) and create one of them first.

The script presents a user interface with a group for each OBS scene and a 
checkbox that enables / disables participation of that scene. When a scene 
is disabled, it's interface collapses to a single line.

When a scene is enabled, it's interface expands to a set of selections:
* Banner  (upon scene activation)
    * Text to display on banner 
    * Banner color - Choice of selected color, rainbow, off, or No Action
    * Banner Alignment
* Banner off upon scene deactivation
* Virtual LED selection
    * Specified LED
    * Action upon activation, deactivation or both
* Physical output selection
    * Specified Output
    * Action upon activation, deactivation or both

<img src="/assets/obsstudio.png">

## Design

### Scene Properties
Scene properties form the user interface for each scene. The design utilizes a 
checkable "Property Group", where the checkbox is used to enable / disable
processing of that scene.

The property group contains user interface elements for all of the properties 
listed above, and the group is "collapsed" by changing each of those elements 
to hidden.

#### scene_properties_create(props, name, category)

Create user interface for 'name' identifying as 'category' and add to 'props'.

'props' is the compete set of proprties being created for this script.

'name' is the scene name. 

'category' is used for the property group label, typically "scene" or "Source"

#### scene_properties_enable(enabled, props, name)

Show / hide the property user interface values for scene 'name' based on the value of 'enabled'.

#### scene_properties_exec(scene_name, isPreview, activate)

Execute actions for 'scene_name' upon 'activate' state change.

**Note** isPreview is not currently used, but is provided to support the possibility of desiring
different behavior for preview vs. program state change.

