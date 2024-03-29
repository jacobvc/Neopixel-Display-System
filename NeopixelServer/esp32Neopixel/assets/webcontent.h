const char * index_html = R"rawtext(
<!DOCTYPE html>
<html>

<head>
  <meta charset="UTF-8">
  <meta name="viewport" content="width=device-width, initial-scale=1.0">
  <link href="dynamicview.css" rel="stylesheet" type="text/css" />
  <script type="text/javascript" src="dynamicview.js"></script>
</head>

<body>

   <div class="dev-control">
    <h2 class="dev-heading">
      <div id="devicename">Connecting to NEOPixel Device</div></h2>
    <div id="devicepanel" class="dev-panel">
    </div>

  </div>
  <div id="status" class="alertw"></div>
  <div id="metadata" class="alertw"></div>
</body>

</html>)rawtext";

const char * dynamicview_js = R"rawtext(
"use strict";

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

function createControl(ctl, key, metaType, label, item) {
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
            `<select id="` + key + `" class='` + item.opts + `' onchange="updateStatus(this.id, this.value)"></select>`;
    }
    else if (metaType == 'color') {
        ctl.innerHTML +=
            `<label for="` + key + `">` + label + `</label>`;
        ctl.innerHTML +=
            `<input id="` + key + `" type="color" onchange="updateColor(this.id, this.value)"></input>`;
    }
    else if (metaType == 'number') {
        ctl.innerHTML +=
            `<label for="` + key + `">` + label + `</label>`;
        ctl.innerHTML +=
            `<input id="` + key + `" type="number" min="` + item.min + `" max = "` + item.max + `" onchange="updateStatus(this.id, this.value)"></input>`;
    }
    else if (metaType == 'readonly') {
        ctl.innerHTML +=
            `<label for="` + key + `">` + label + `</label>`;
        ctl.innerHTML +=
            `<input id="` + key + `" type="text" disabled></input>`;
    }
    else {
        ctl.innerHTML +=
            `<label for="` + key + `">` + label + `</label>`;
        ctl.innerHTML +=
            `<input id="` + key + `" type="text" onchange="updateStatus(this.id, this.value)"></input>`;
    }
}

function createView(ctl, key, item) {
    if (item.type == 'composite') {
        // div
        for (var i = 0; i < item.fields.length; ++i) {
            createView(ctl, item.fields[i].name, item.fields[i]);
        }
        // /div
        ctl.innerHTML += '<br>';
    }
    else if (item.type == 'collapsible') {
        // div
        ctl.innerHTML += '<details><summary>' + item.label + '</summary><div id="collapsible-' + item.name + '"></details>';
        var inner = document.getElementById('collapsible-' + item.name);
        for (var i = 0; i < item.fields.length; ++i) {
            createView(inner, item.fields[i].name, item.fields[i]);
        }
        // /div
        ctl.innerHTML += '<br>';
    }
    else if (Array.isArray(item.names)) {
        ctl.innerHTML += "<label class='column1'>" + item.label + ":</label>";
        for (var i = 0; i < item.names.length; ++i) {
            createControl(ctl, key + '[' + i + ']', item.type, item.names[i], item);
        }
        ctl.innerHTML += '<br>';
    }
    else {
        createControl(ctl, key, item.type, item.label, item);
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
                if (ctl.type == 'color') {
                    var str = value.toString(16);
                    ctl.value = '#'+ str.padStart(6, 0);
                }
                else {
                // Dont update the active element
                ctl.value = value;
                }
            }
        }
    }
}

function reportError() {
    connected = false;
    document.getElementById("status").innerHTML
        = 'Request Failed ... Reconnecting';
}

function createPage(key, value) {
    const xhr = new XMLHttpRequest();
    xhr.onerror = reportError;

    xhr.onload = function () {
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

        connected = true;
        updateStatus('', '');
    }
    var url = new URL("/metadata", location);
    if (key && key.length > 0) {
        url.searchParams.set(key, value);
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

function updateColor(key, value) {
    updateStatus(key, parseInt(value.substring(1), 16));
}

function updateStatus(key, value) {
    const xhr = new XMLHttpRequest();
    xhr.onerror = reportError;

    xhr.onload = function () {
        for (const objid in this.response) {
            if (this.response.hasOwnProperty(objid)) {
                updateValue(objid, this.response[objid]);
            }
        }
    }
    var url = new URL("/status", location);
    if (key && key.length > 0) {
        url.searchParams.set(key, value);
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

)rawtext";

const char * dynamicview_css = R"rawtext(
html {
    touch-action: manipulation;
}

.info {
    color: rgb(200, 200, 200);
    font-size: 0.7rem;
    width: 100%;
    display: inline-block;
}


html,
body {
    background-color: rgb(45, 45, 45);
    font-family: Helvetica, Arial, Sans-Serif;
    font-size: 1.2rem;
}

h1 {
    color: rgb(135, 135, 135);
    font-weight: normal;
    margin: 5px 0;
    font-size: 1.7rem;
}

h2 {
    color: rgb(145, 145, 145);
    font-weight: normal;
    margin: 3px 0;
    font-size: 1.5rem;
}

input, select {
    font-size: 1.2rem;
    margin-top:2px;
    margin-bottom:2px;
}

label {
    padding-left: 5px;
    padding-right: 5px;
}
.column1 {
    display: inline-block;
    min-width: 90px;
    padding-left: 5px;
}
.checkboxlabel {
    display: inline-block;
    padding-right: 25px;
}

.info, .alerte, .alertw {
    width: auto;
    background-color: whitesmoke;
    color: black;
    display: inline-block;
    border: 3px solid gray;
    border-radius: 10px;
    padding: 10px;
    margin: 5px;
    font-size: 1rem;
}

.alerte {
    color: rgb(200, 0, 0);
}

.alertw {
    color: orange;
}


/** A device control is constructed within a
 * a dev-control class, and is composed of a dev-heading and a dev-panel
 */
 .dev-control {
    height: auto;
    width: auto;
    min-width: 20px;
    background-color: cornsilk;
    color: black;
    display: inline-grid;
    border: 3px solid goldenrod;
    border-radius: 10px;
    vertical-align: top;
    margin: 5px;
}

.dev-control>div {
    position: relative;
    height: auto;
}

.dev-heading {
    position: relative;
    top: -2px;
    height: 32px;
    width: 100%;
    background-color: cornsilk;
    color: black;
    border-radius: 10px;
    display: inline-block;
    text-indent: 12px;
    margin: 3px 0;
}

.dev-panel {
    border-radius: 0px;
    width: auto;
    height: auto;
    display: inline-block;
    vertical-align: top;
    padding: 4px;
    height: 134px;
}

.dev-control input[type=range] {
    left: auto !important;
    background-color: transparent;
    color: white
}


.disabledpanel {
    pointer-events: none;
    opacity: 0.4;
}

/* The switch - the box around the slider */
.switch {
    position: relative;
    display: inline-block;
    width: 44px;
    height: 28px;
  }
  
  /* Hide default HTML checkbox */
  .switch input {
    opacity: 0;
    width: 0;
    height: 0;
  }
  
  /* The slider */
  .slider {
    position: absolute;
    cursor: pointer;
    top: 0;
    left: 0;
    right: 0;
    bottom: 0;
    background-color: #ccc;
    -webkit-transition: .4s;
    transition: .4s;
  }
  
  .slider:before {
    position: absolute;
    content: "";
    height: 20px;
    width: 20px;
    left: 4px;
    bottom: 4px;
    background-color: white;
    -webkit-transition: .4s;
    transition: .4s;
  }
  
  input:checked + .slider {
    background-color: red;
  }
  
  input:focus + .slider {
    box-shadow: 0 0 1px red;
  }
  
  input:checked + .slider:before {
    -webkit-transform: translateX(20px);
    -ms-transform: translateX(20px);
    transform: translateX(20px);
  }
  
  /* Rounded sliders */
  .slider.round {
    border-radius: 34px;
  }
  
  .slider.round:before {
    border-radius: 50%;
  }
)rawtext";

