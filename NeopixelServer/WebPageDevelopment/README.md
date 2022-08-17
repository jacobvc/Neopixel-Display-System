# Web Page Development

The WebPageDevelopment directory is used to develop and debug the embedded web page. The web page consists of the files index.html, dynamicview.js, and dynamicview.css

This web page fetches additional files from the same path as the root 'index.html' and interacts using the DOM 'location'. dynamicview.js declares the jacascript var __LOCATION__ as the hard coded ip address of the current device for non-embedded testing (it needs to be set for the current device). The web page files also use exclude regions (__BEGIN_EXCLUDE__ through __END_EXCLUDE__). The directory also contains a python script (deploywebpage.py) and a batch file (deploywebpage.bat) that convert the nonembedded webpage to embedded (more on that below).

## Root HTML file (index.html)
The index file is a trivial HTML file referencing the javascript file and CSS file, and a few div's to contain the device view as well as some debugging capabilities. The device view itself is implemented in dynamicview.js.

## Javascript Dynamic View
The javascript dynamic view is implemented in dynamicview.js

This file hooks the 'DOMContentLoaded' event to **createPage()** and sets a timer to invoke **updateStatus()**.

### Entry Points
 
**createPage()** requests the metadata, and upon receipt of the metadata
each member of the metadata array is processed. Metadata objects of type 'opts' are used to build an options dictionary, and each metadata object of other types is used to **createView()**.
 
After all of the metadata objects have been processed, the options dictionary is used to populate all of the options associated with the created controls.
 
**updateStatus(key, value)** makes a http request to "/status" with the 'key'='value' as a get parameter. Upon receipt of status data, updateValue is called for each field of the status object.
 
 ### Support functions
**createView(ctl, key, item)** does a **createView()** for each 'field' in metadata 'item' of type 'composite' or 'collapsible', a **createControl()** for each name in a metadata 'item' containing an array of 'names', a **createControl()** for all other metadata 'item's
 
**createControl(ctl, key, metaType, label, selectClass)** creates a "switch" for metaType 'bool, a combobox (html select) for metaType 'select', for metaType number an numeric input, for metatype readonly a disabled text input, and a text input for all other metaType's. Each control is assigned a onchange event that calls **updateStatus()**.
 
 **updateValue(key, value)** gets the html element with an ID matching 'key', and sets its value / content to 'value'
 
 ## Deploy Web Page
 the python script **deploywebpage.py** processes web page files, converts them from nonembedded to embedded, and creates a header file to be compiled into the application. 
 
 It utilzes css_html_js_minify to reduce file size
 
     pip install css_html_js_minify

The script reads the files listed on its command line, converting "__LOCATION__" to "location" and excluding regions marked for exclusion, and writes their contents to the specified output file, wrapped as "Rawtext". It then processes them through the relevant minimizer and writes them to a file named the same as the output file with ".min" inserted ahead of the extension.

The exclude regions are used both for reducing code size by removing bulk comment sections and for removing development / diagnostic functionality from the embedded version.

**deploywebpage.bat** is a convenience file that specifies and output as ../esp32Neopixel/assets/webcontent.h and the source files: index.html dynamicview.js dynamicview.css 
