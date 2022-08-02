# Web Page Development

# Javascript Dynamic View
The javascript dynamic view is implemented in dynamicview.js
 
When the form is loaded, createPage() is invoked. createPage requests the metadata, and upon receipt of the metadata
each member of the metadata array is processed. members of type 'opts' are used to build an options dictionary, and each member of other types is used to createView().
 
createView(ctl, key, item) creates a view for each 'field' of 'item' of type 'composite', creates a control for each name in an 'item' containing an array of 'names', and creates a control for all other 'item's
 
createControl(ctl, key, metaType, label, selectClass) creates a "switch" for metaType 'bool, a combobox (html select) for metaType 'select', and a text input for all other metaType's. Each control is assigned a onchange event that calls updateStatus.
 
After all of the metadata objects have been processed the options dictionary is used to populate all of the options associated with the created controls.
 
updateStatus(key, value) makes a http request to "/status" with the 'key'='value' as a get parameter. Upon receipt of status data, updateValue is called for each field of the status object.
 
 updateValue(key, value) gets the html element with an ID matching 'key', and sets its value / content to 'value'
