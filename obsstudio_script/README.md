# OBS Studio Script

The OBS Studio script is a python script that allows control of any / all of the provided
features as a result of activation and / or deactivation of any scene. The user interface
also supports enabling / disabling the participation of OBS Studio scenes.

The script presents a user interface with a group for each OBS scene and a checkbox that enables / disables participation of that scene. When a scene is disabled, it collapses to a single line.

When a scene is enabled, it expands to a set of selections:
* Banner  (upon scene activation)
    * Text to display on banner 
    * Banner color - Choice of selected color, rainbow, off, or No Action
    * Banner Alignment
* Banner off upon scene deactivation
* Virtual LED selection
    * Specified LED
    * Action upon activatio, deactivation or both
* Physical output selection
    * Specified Output
    * Action upon activatio, deactivation or both

<img src="/assets/obsstudio.png">

