"use strict";
//__BEGIN_EXCLUDE__
/*
    <label for ID> <div ID> OR <input ID>
    Where ID is a JSON input property name (key)
    OR key[index] in the case of an array

    select elements are created empty with class='enum-name'
    where the the options are populated when the enum, for example 'ledmode' 
    is received in a configuration message

    Each input element has 
      onchange="updateStatus(this.id, this.value)
    or similar, resulting in a GET request with a JSON property-name=value parameter
    being sent immediately

    JSON array properties are used to construct a section. It is constructed
    inside a DIV with id="<key>-section"
       function createView(elemType, label, key, source, srcProp, selectClass) 
    An element of elemType is constructed for each value of the srcProp array
    with id="key[index]", following a label with text 'label'



WARNING WARNING WARNING 
js_minify messes up with lines broken before a '+'
*/

var __LOCATION__ = "http://192.168.12.208";
//var __LOCATION__ = "http://192.168.12.227";
//__END_EXCLUDE__

var connected = false;

document.addEventListener('DOMContentLoaded', function () {
    createPage('', '');
    const timeout = setInterval(function () {
        if (connected) {
            updateStatus('', '');
        }
        else {
            createPage('', '');
        }
    }, 5 * 1000);
});

//__BEGIN_EXCLUDE__
/// <summary>
/// Create an element (typically a label and a control) for data of 'metaType' using 'label'
/// for the label and 'optionsName' for options in the case of metatype='select'. Attach it to 'ctl'
/// </summary>
/// <param name="ctl"></param>
/// <param name="key"></param>
/// <param name="metaType"></param>
/// <param name="label"></param>
/// <param name="optionsName"></param>
//__END_EXCLUDE__
function createControl(ctl, key, metaType, label, selectClass) {
    if (metaType == 'bool') {
        ctl.innerHTML +=
            `<label class="switch"><input type="checkbox" id="` + key + `" onchange="updateStatus(this.id, this.checked)"><span class="slider round"></span></label>`;
        ctl.innerHTML +=
            `<label for="` + key + `" class="checkboxlabel">` + label + `</label>`;
    }
    else if (metaType == 'select') {
        ctl.innerHTML +=
            `<label for="` + key + `">` + label + `</label>`;
        ctl.innerHTML +=
            `<select id="` + key + `" class='` + selectClass + `' onchange="updateStatus(this.id, this.value)"></select>`;
    }
    else {
        ctl.innerHTML +=
            `<label for="` + key + `">` + label + `</label>`;
        ctl.innerHTML +=
            `<input id="` + key + `" onchange="updateStatus(this.id, this.value)"></select>`;
    }
}

//__BEGIN_EXCLUDE__
/// <summary>
/// Create view of 'item', mapped using 'key' and attach it to 'ctl'
/// </summary>
/// <param name="ctl"></param>
/// <param name="key"></param>
/// <param name="item"></param>
//__END_EXCLUDE__
function createView(ctl, key, item) {
    if (item.type == 'composite') {
        // div
        for (var i = 0; i < item.fields.length; ++i) {
            createView(ctl, item.fields[i].name, item.fields[i]);
        }
        // /div
        ctl.innerHTML += '<br>';
    }
    else if (Array.isArray(item.names)) {
        ctl.innerHTML += "<label class='column1'>" + item.label + ":</label>";
        for (var i = 0; i < item.names.length; ++i) {
            createControl(ctl, key + '[' + i + ']', item.type, item.names[i], item.opts);
        }
        ctl.innerHTML += '<br>';
    }
    else {
        createControl(ctl, key, item.type, item.label, item.opts);
    }
}

function populateOptions(selectClass) {
    if (this.hasOwnProperty(selectClass)) {
        var array = this[selectClass].values;
        var ctls = document.getElementsByClassName(this[selectClass].name);

        for (var index = 0; index < ctls.length; ++index) {
            ctls[index].innerHTML = "";
            for (var i = 0; i < array.length; ++i) {
                ctls[index].innerHTML +=
                    `<option value="` + i + `">` + array[i] + `</option>`;
            }
        }
    }
}

function updateValue(key, value) {
    if (Array.isArray(value)) {
        for (var val = 0; val < value.length; ++val) {
            updateValue(key + '[' + val + ']', value[val]);
        }
    }
    else {
        var ctl = document.getElementById(key);
        if (ctl) {
            if (ctl.type == 'checkbox') {
                ctl.checked = value;
            }
            else if (ctl.nodeName == 'DIV') {
                ctl.innerHTML = value;
            }
            else if (ctl != document.activeElement) {
                // Dont update the active element
                ctl.value = value;
            }
        }
    }
}

function reportError() {
    connected = false;
    document.getElementById("status").innerHTML
        = 'Request Failed ... Reconnecting';
}

function createPage(id, value) {
    const xhr = new XMLHttpRequest();
    xhr.onerror = reportError;

    xhr.onload = function () {
        //__BEGIN_EXCLUDE__
        console.log(JSON.stringify(this.response));
        // selectClasses is an array of classes used as selects options
        //__END_EXCLUDE__
        var selectClasses = [];

        var ctl = document.getElementById('devicepanel');
        ctl.innerHTML = "";

        for (const objid in this.response) {
            if (this.response.hasOwnProperty(objid)) {
                var type = this.response[objid].type;
                if (type == 'opts') {
                    selectClasses.push(objid);
                }
                else if (type == 'value') {
                    updateValue(this.response[objid].name, this.response[objid].value);
                }
                else {
                    createView(ctl, this.response[objid].name, this.response[objid]);
                }
                //console.log(objid + ": " +  type);
            }
        }

        selectClasses.forEach(populateOptions, this.response);

        //__BEGIN_EXCLUDE__
        document.getElementById("metadata").innerHTML = htmldump(this.response, '<br>');
        //__END_EXCLUDE__
        connected = true;
        updateStatus('', '');
    }
    var url = new URL("/metadata", __LOCATION__);
    if (id && id.length > 0) {
        url.searchParams.set(id, value);
    }

    xhr.open("GET", url, true);
    xhr.responseType = 'json';
    try {
        xhr.send();
    }
    catch (e) {
        connected = false;
        document.getElementById("metadata").innerHTML
            = 'RequestFailed<br>' + e + '<br>  Reconnecting';
    }
}
function updateStatus(id, value) {
    const xhr = new XMLHttpRequest();
    xhr.onerror = reportError;

    xhr.onload = function () {
        //__BEGIN_EXCLUDE__
        //console.log(mydump(this.response));
        console.log(JSON.stringify(this.response));
        //__END_EXCLUDE__
        for (const objid in this.response) {
            if (this.response.hasOwnProperty(objid)) {
                updateValue(objid, this.response[objid]);
            }
        }
        //__BEGIN_EXCLUDE__
        document.getElementById("status").innerHTML = htmldump(this.response, '<br>');
        //__END_EXCLUDE__
    }
    var url = new URL("/status", __LOCATION__);
    if (id && id.length > 0) {
        url.searchParams.set(id, value);
    }

    xhr.open("GET", url, true);
    xhr.responseType = 'json';
    try {
        xhr.send();
    }
    catch (e) {
        connected = false;
        document.getElementById("status").innerHTML
            = 'RequestFailed<br>' + e + '<br>  Reconnecting';
    }
}

//__BEGIN_EXCLUDE__
function mydump(arr, level) {
    var dumped_text = "";
    if (!level) level = 0;

    var level_padding = "";
    for (var j = 0; j < level + 1; j++) level_padding += "    ";

    if (typeof (arr) == 'object') {
        for (var item in arr) {
            var value = arr[item];

            if (typeof (value) == 'object') {
                dumped_text += level_padding + "[" + item + "]\n";
                dumped_text += mydump(value, level + 1);
            } else {
                dumped_text += level_padding + "[" + item + "] = \"" + value + "\"\n";
            }
        }
    } else {
        dumped_text = "===>" + arr + "<===(" + typeof (arr) + ")";
    }
    return dumped_text;
}
function htmldump(arr, end) {
    var dumped_text = '';

    if (typeof (arr) == 'object') {
        for (var item in arr) {
            var value = arr[item];

            if (typeof (value) == 'object') {
                dumped_text += "[" + item + "] ";
                dumped_text += htmldump(value, '');
            } else {
                dumped_text += "[" + item + "]=\"" + value + "\" ";
            }
            dumped_text += end;
        }
    } else {
        dumped_text = "===>" + arr + "<===(" + typeof (arr) + ")";
    }
    return dumped_text;
}
//__END_EXCLUDE__
