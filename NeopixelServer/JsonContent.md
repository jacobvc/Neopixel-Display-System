# JSON Status
    {
        "textalign": 1,
        "bmpalign": 4,
        "txtclr": 0,
        "text": "Hi there",
        "bmp": 0,
        "outputs": [
                0,
                0,
                0,
                0,
                0
        ],
        "leds": [
                0,
                0,
                0,
                0
        ]
    }

# JSON Metadata
    [
        {
                "name": "devicename",
                "type": "value",
                "value": "NEOPixel Display"
        },
        {
                "name": "text",
                "type": "composite",
                "fields": [
                        {
                                "name": "text",
                                "label": "Banner",
                                "type": "text"
                        },
                        {
                                "name": "txtclr",
                                "label": "Color",
                                "type": "select",
                                "opts": "clr-opts"
                        },
                        {
                                "name": "textalign",
                                "label": "Align",
                                "type": "select",
                                "opts": "align-opts"
                        }
                ]
        },
        {
                "name": "bmp",
                "type": "composite",
                "fields": [
                        {
                                "name": "bmp",
                                "label": "Bitmap",
                                "type": "select",
                                "opts": "bmp-opts"
                        },
                        {
                                "name": "bmpalign",
                                "label": "Align",
                                "type": "select",
                                "opts": "align-opts"
                        }
                ]
        },
        {
                "name": "outputs",
                "type": "bool",
                "label": "Outputs",
                "names": [
                        "Audio",
                        "Relay 1",
                        "Relay 2",
                        "Relay 3",
                        "Audio 2"
                ]
        },
        {
                "name": "leds",
                "type": "select",
                "label": "LEDs",
                "opts": "led-opts",
                "names": [
                        "LED 1",
                        "LED 2",
                        "LED 3",
                        "LED 4"
                ]
        },
        {
                "name": "clr-opts",
                "type": "opts",
                "values": [
                        "off",
                        "Rainbow",
                        "Red",
                        "Orange",
                        "Yellow",
                        "Green",
                        "Blue",
                        "Indigo",
                        "Violet"
                ]
        },
        {
                "name": "bmp-opts",
                "type": "opts",
                "values": [
                        "off",
                        "Heart",
                        "Yello Smiley",
                        "Blue Smiley",
                        "Smiley Tongue",
                        "Small Smiley"
                ]
        },
        {
                "name": "led-opts",
                "type": "opts",
                "values": [
                        "off",
                        "on",
                        "blink"
                ]
        },
        {
                "name": "align-opts",
                "type": "opts",
                "values": [
                        "left",
                        "center",
                        "right",
                        "scroll",
                        "bounce"
                ]
        }
    ]
