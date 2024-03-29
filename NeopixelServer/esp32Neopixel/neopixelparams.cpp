#include "neopixeldisplay.h"
#include <Arduino.h>
#include <Preferences.h>

#include "neopixel.h"
#include "neopixelparams.h"

// IDs used to guide generic parsing fuctions
enum parsingIds
{
    TEXT_ID,
    DEVNAME_ID,
    BITMAP_ID,
    TEXT_ALIGNMENT_ID,
    BMP_ALIGNMENT_ID,
    COLOR_INDEX_ID,
    PANELS_WIDE_ID,    
    PANELS_HIGH_ID
};

Preferences prefs;
NeopixelParams params;

// Preferences
String prefsPassword;
String prefsSsid;
String prefsDeviceName;

int prefsPanelsW = TILES_WIDE;
int prefsPanelsH = TILES_HIGH;

struct arraySpec {
    int value;
    const char *name;
};

const char *ledsNames[] = { LEDS_NAMES };
#define VIRTUAL_LED_COUNT (sizeof(ledsNames) / sizeof(ledsNames[0]))

int prefsLedColorA[] = { LEDS_COLORS_A };
int prefsLedColorB[] = { LEDS_COLORS_B };

const struct arraySpec outputsPorts[] = {OUTPUTS_PORTS};
#define OUTPUT_COUNT (sizeof(outputsPorts) / sizeof(outputsPorts[0]))
byte outputValues[OUTPUT_COUNT];

StaticJsonDocument<3000> jsonEncodingDoc;
String metadataJson;
String statusJson;

/***
 *      ____            __
 *     |  _ \ _ __ ___ / _| ___ _ __ ___ _ __   ___ ___  ___
 *     | |_) | '__/ _ \ |_ / _ \ '__/ _ \ '_ \ / __/ _ \/ __|
 *     |  __/| | |  __/  _|  __/ | |  __/ | | | (_|  __/\__ \
 *     |_|   |_|  \___|_|  \___|_|  \___|_| |_|\___\___||___/
 *
 */
void InitPreferences()
{
    prefs.begin("NEOPixel", false);
    bool tpInit = prefs.isKey("nvsInit"); // Test for the existence of the "already initialized" key.
    if (tpInit == false)
    {
        // If tpInit is 'false', the key "nvsInit" does not yet exist therefore this
        //  must be our first-time run. Set Defaults

        // Device related Preferences
        prefs.putString("device_name", "NEOPixel Display");

        // WiFi related Preferences
        prefs.putString("ssid", "");
        prefs.putString("password", "");
        
        prefs.putInt("prefsPanelsW", TILES_WIDE);
        prefs.putInt("prefsPanelsH", TILES_HIGH);

        prefs.putBytes("prefsLedColorA", prefsLedColorA, sizeof(prefsLedColorA));
        prefs.putBytes("prefsLedColorB", prefsLedColorB, sizeof(prefsLedColorB));

        prefs.putBool("nvsInit", true); // Create the "already initialized" key and store a value.
    }
    prefs.end();
}

void GetPreferences()
{
    prefs.begin("NEOPixel", true);

    prefsDeviceName = prefs.getString("device_name");
    prefsSsid = prefs.getString("ssid");
    prefsPassword = prefs.getString("password");

    prefsPanelsW = prefs.getInt("prefsPanelsW");
    prefsPanelsH = prefs.getInt("prefsPanelsH");

    prefs.getBytes("prefsLedColorA", prefsLedColorA, sizeof(prefsLedColorA));
    prefs.getBytes("prefsLedColorB", prefsLedColorB, sizeof(prefsLedColorB));

    prefs.end(); // close the namespace in RO mode and...
}

void SavePreferences()
{
    prefs.begin("NEOPixel", false);

    prefs.putString("device_name", prefsDeviceName);
    prefs.putString("ssid", prefsSsid);
    prefs.putString("password", prefsPassword);

    prefs.putInt("prefsPanelsW", prefsPanelsW);
    prefs.putInt("prefsPanelsH", prefsPanelsH);

    prefs.putBytes("prefsLedColorA", prefsLedColorA, sizeof(prefsLedColorA));
    prefs.putBytes("prefsLedColorB", prefsLedColorB, sizeof(prefsLedColorB));

    prefs.end(); // close the namespace in RO mode and...
    JsonEncodeNeopixelMetaData();
    Serial.print(metadataJson);
}

/***
 *      ____                                _
 *     |  _ \ __ _ _ __ __ _ _ __ ___   ___| |_ ___ _ __ ___
 *     | |_) / _` | '__/ _` | '_ ` _ \ / _ \ __/ _ \ '__/ __|
 *     |  __/ (_| | | | (_| | | | | | |  __/ ||  __/ |  \__ \
 *     |_|   \__,_|_|  \__,_|_| |_| |_|\___|\__\___|_|  |___/
 *
 */


//
// Outputs
//
int OutputCount()
{
    return OUTPUT_COUNT;
}

void JsonMetaOutputs(JsonObject obj)
{
  obj["name"] = "outputs";
  obj["type"] = "bool";
  obj["label"] = "Outputs";
  JsonArray arr = obj.createNestedArray("names");

    for (int i = 0; i < OUTPUT_COUNT; ++i)
    {
        arr[i] = outputsPorts[i].name;
    }
}

int OutputValue(int index)
{
    if (index < OUTPUT_COUNT)
    {
        return digitalRead(outputsPorts[index].value);
    }
    return -1;
}

void OutputValueChanged(int index, int newvalue)
{
    digitalWrite(outputsPorts[index].value, newvalue);
}

void JsonOutputDataArray(JsonArray obj)
{
    for (int i = 0; i < OUTPUT_COUNT; ++i)
    {
        obj[i] = OutputValue(i);
    }
}

byte parseOutput(const String &value, int arg)
{
    int val;
    if (value.equalsIgnoreCase("true"))
    {
        val = 1;
    }
    else
    {
        val = value.toInt();
    }
    OutputValueChanged(arg, val);
    return 1;
}

//
// Led
//
int LedsCount()
{
    return VIRTUAL_LED_COUNT;
}

void JsonMetaLeds(JsonObject obj)
{
  obj["name"] = "leds";
  obj["type"] = "select";
  obj["label"] = "LEDs";
  obj["opts"] = "led-opts";
  JsonArray arr = obj.createNestedArray("names");

    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        arr[i] = ledsNames[i];
    }
}

void LedChanged(int index, LedMode value)
{
    display.SetLed(index, value);
}

void JsonLedDataArray(JsonArray obj)
{
    LedDef *leds = display.GetLeds();
    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        obj[i] = leds[i].mode;
    }
}

byte parseLed(const String &value, int arg)
{
    LedMode val = LedMode::off;
    if (value.equalsIgnoreCase("true"))
    {
        val = LedMode::on;
    }
    else
    {
        val = (LedMode)value.toInt();
    }
    LedChanged(arg, val);
    return 1;
}

//
// LED Color A
//
void JsonMetaLedColorA(JsonObject obj)
{
  obj["name"] = "ledcolora";
  obj["type"] = "color";
  obj["label"] = "LED Color A";
  JsonArray arr = obj.createNestedArray("names");

    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        arr[i] = ledsNames[i];
    }
}

void JsonLedColorAArray(JsonArray obj)
{
    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        obj[i] = display.LedGetColorA(i);
    }
}

byte parseLedColorA(const String &value, int arg)
{
    int val = value.toInt();
    
    prefsLedColorA[arg] = val;
    SavePreferences();
     
    display.LedSetColorA(arg, val);
    return 1;
}

//
// LED Color B
//
void JsonMetaLedColorB(JsonObject obj)
{
  obj["name"] = "ledcolorb";
  obj["type"] = "color";
  obj["label"] = "LED Color B";
  JsonArray arr = obj.createNestedArray("names");

    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        arr[i] = ledsNames[i];
    }
}

void JsonLedColorBArray(JsonArray obj)
{
    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        obj[i] = display.LedGetColorB(i);
    }
}

byte parseLedColorB(const String &value, int arg)
{
    int val = value.toInt();
    prefsLedColorB[arg] = val;
    SavePreferences();
    display.LedSetColorB(arg, val);
    return 1;
}

//
// Text
//
String &TextValue()
{
    return display.GetText();
}

void TextChanged(String msg)
{
    display.ShowText(msg);
}

byte parseString(const String &value, int arg)
{
    switch (arg)
    {
    case TEXT_ID:
        TextChanged(value);
        break;
    case DEVNAME_ID:
        prefsDeviceName = value;
        SavePreferences();
        break;
    }
    return 1;
}

//
// Integer
//
int IntValue(int arg)
{
    switch (arg)
    {
    case BITMAP_ID:
        return display.GetBitmap();
        break;
    case TEXT_ALIGNMENT_ID:
        return display.GetTextAlignment();
        break;
    case BMP_ALIGNMENT_ID:
        return display.GetBmpAlignment();
        break;
    case COLOR_INDEX_ID:
        return display.GetTextColor();
        break;
    case PANELS_WIDE_ID:
        return prefsPanelsW;        
        break;      
    case PANELS_HIGH_ID:
        return prefsPanelsH;        
        break;      
    }
    return -1;
}

void IntChanged(int index, int value)
{
    switch (index)
    {
    case BITMAP_ID:
        display.ShowBitmap(value);
        break;
    case TEXT_ALIGNMENT_ID:
        display.SetTextAlignment((Alignment)value);
        break;
    case BMP_ALIGNMENT_ID:
        display.SetBmpAlignment((Alignment)value);
        break;
    case COLOR_INDEX_ID:
        display.SetTextColor(value);
        break;
    case PANELS_WIDE_ID:
        prefsPanelsW = value;      
        SavePreferences();
        NeoMatrixReInit();
        break;      
    case PANELS_HIGH_ID:
        prefsPanelsH = value;      
        SavePreferences();
        NeoMatrixReInit();
        break;      
    }
}

byte parseInt(const String &value, int arg)
{
    IntChanged(arg, value.toInt());
    return 1;
}

/***
 *      _  _              _         _ ___                          ___            _ 
 *     | \| |___ ___ _ __(_)_ _____| | _ \__ _ _ _ __ _ _ __  ___ |_ _|_ __  _ __| |
 *     | .` / -_) _ \ '_ \ \ \ / -_) |  _/ _` | '_/ _` | '  \(_-<  | || '  \| '_ \ |
 *     |_|\_\___\___/ .__/_/_\_\___|_|_| \__,_|_| \__,_|_|_|_/__/ |___|_|_|_| .__/_|
 *                  |_|                                                     |_|     
 */

void NeopixelParams::AddParser(String name, byte (*parse)(const String &value, int arg), int arg)
{
    struct parseFn tmp = {parse, arg};
    parseMap[name] = tmp;
}

byte NeopixelParams::parse(const String &name, const String &value)
{
    std::map<String, struct parseFn>::iterator found = parseMap.find(name);
    if (found != parseMap.end()) {
        Serial.println("Set " + name + "=" + value);
        byte result = found->second.parse(value, found->second.arg);
        JsonEncodeNeopixelData();
        return result;
    }
    Serial.println("Failed " + name + "=" + value);
    return 0;
}

/***
 *      ____       _
 *     / ___|  ___| |_ _   _ _ __
 *     \___ \ / _ \ __| | | | '_ \
 *      ___) |  __/ |_| |_| | |_) |
 *     |____/ \___|\__|\__,_| .__/
 *                          |_|
 */
void NeoParamSetup()
{
    InitPreferences();
    // Get preferences ASAP. Other setup may need them
    GetPreferences();

    params.AddParser("textalign", parseInt, TEXT_ALIGNMENT_ID);
    params.AddParser("bmpalign", parseInt, BMP_ALIGNMENT_ID);
    params.AddParser("txtclr", parseInt, COLOR_INDEX_ID);
    params.AddParser("text", parseString, TEXT_ID);
    params.AddParser("bmp", parseInt, BITMAP_ID);
    params.AddParser("panelsw", parseInt, PANELS_WIDE_ID);
    params.AddParser("panelsh", parseInt, PANELS_HIGH_ID);

    params.AddParser("devicename", parseString, DEVNAME_ID);

    for (int i = 0; i < VIRTUAL_LED_COUNT; ++i)
    {
        params.AddParser(String("leds[") + i + "]", parseLed, i);
        params.AddParser(String("ledcolora[") + i + "]", parseLedColorA, i);
        params.AddParser(String("ledcolorb[") + i + "]", parseLedColorB, i);
    }

    for (int i = 0; i < OUTPUT_COUNT; ++i)
    {
        pinMode(outputsPorts[i].value, OUTPUT);
        params.AddParser(String("outputs[") + i + "]", parseOutput, i);
    }
        
    JsonEncodeNeopixelMetaData();
}

/***
 *      _____ _      _
 *     |_   _(_) ___| | __
 *       | | | |/ __| |/ /
 *       | | | | (__|   <
 *       |_| |_|\___|_|\_\
 *
 */
void NeoParamTick()
{
    // Nothing to do here yet
}

/***
 *          _ ____   ___  _   _   _____                     _ _
 *         | / ___| / _ \| \ | | | ____|_ __   ___ ___   __| (_)_ __   __ _
 *      _  | \___ \| | | |  \| | |  _| | '_ \ / __/ _ \ / _` | | '_ \ / _` |
 *     | |_| |___) | |_| | |\  | | |___| | | | (_| (_) | (_| | | | | | (_| |
 *      \___/|____/ \___/|_| \_| |_____|_| |_|\___\___/ \__,_|_|_| |_|\__, |
 *                                                                    |___/
 */

void JsonEncodeNeopixelData()
{
    jsonEncodingDoc.clear();
    StaticJsonDocument<3000> &doc = jsonEncodingDoc;
    doc["textalign"] = IntValue(TEXT_ALIGNMENT_ID);
    doc["bmpalign"] = IntValue(BMP_ALIGNMENT_ID);
    doc["txtclr"] = IntValue(COLOR_INDEX_ID);
    doc["text"] = TextValue();
    doc["bmp"] = IntValue(BITMAP_ID);
    doc["panelsh"] = prefsPanelsH;
    doc["panelsw"] = prefsPanelsW;
    char buf[32];
    sprintf(buf, "%d x %d", prefsPanelsW * 32, prefsPanelsH * 8);    
    doc["wxh"] = buf;
    JsonOutputDataArray(doc.createNestedArray("outputs"));
    JsonLedDataArray(doc.createNestedArray("leds"));
    JsonLedColorAArray(doc.createNestedArray("ledcolora"));
    JsonLedColorBArray(doc.createNestedArray("ledcolorb"));

    statusJson = "";
    serializeJson(doc, statusJson);
    Serial.println(statusJson);
}

void JsonMetaCompositeText(JsonObject obj)
{
  obj["name"] = "text";
  obj["type"] = "composite";
  JsonArray arr = obj.createNestedArray("fields");

  JsonObject arrobj = arr.createNestedObject();
  arrobj["name"] = "text";
  arrobj["label"] = "Banner";
  arrobj["type"] = "text";

  arrobj = arr.createNestedObject();
  arrobj["name"] = "txtclr";
  arrobj["label"] = "Color";
  arrobj["type"] = "select";
  arrobj["opts"] = "clr-opts";

  arrobj = arr.createNestedObject();
  arrobj["name"] = "textalign";
  arrobj["label"] = "Align";
  arrobj["type"] = "select";
  arrobj["opts"] = "align-opts";
}

void JsonMetaCompositeBitmap(JsonObject obj)
{
  obj["name"] = "bmp";
  obj["type"] = "composite";
  JsonArray arr = obj.createNestedArray("fields");
  
  JsonObject arrobj = arr.createNestedObject();
  arrobj["name"] = "bmp";
  arrobj["label"] = "Bitmap";
  arrobj["type"] = "select";
  arrobj["opts"] = "bmp-opts";

  arrobj = arr.createNestedObject();
  arrobj["name"] = "bmpalign";
  arrobj["label"] = "Align";
  arrobj["type"] = "select";
  arrobj["opts"] = "align-opts";
}

void JsonMetaPanels(JsonObject obj)
{
  obj["name"] = "panels";
  obj["type"] = "composite";
  obj["label"] = "Panels";
  JsonArray arr = obj.createNestedArray("fields");
 
  JsonObject arrobj = arr.createNestedObject();
  arrobj["name"] = "panelsw";
  arrobj["label"] = "Panels W";
  arrobj["type"] = "number";
  arrobj["min"] = 1;
  arrobj["max"] = 4;

  arrobj = arr.createNestedObject();
  arrobj["name"] = "panelsh";
  arrobj["label"] = "Panels H";
  arrobj["type"] = "number";
  arrobj["min"] = 1;
  arrobj["max"] = 4;

  arrobj = arr.createNestedObject();
  arrobj["name"] = "wxh";
  arrobj["label"] = "W x H";
  arrobj["type"] = "readonly";
}

void JsonMetaSettings(JsonObject obj)
{
  obj["name"] = "settings";
  obj["type"] = "collapsible";
  obj["label"] = "Settings";
  JsonArray arr = obj.createNestedArray("fields");
  JsonMetaLedColorA(arr.createNestedObject());
  JsonMetaLedColorB(arr.createNestedObject());
  JsonMetaPanels(arr.createNestedObject());
}

void JsonEncodeNeopixelMetaData()
{
    jsonEncodingDoc.clear();
    // root["password"] = prefsPassword;
    // root["ssid"] = prefsSsid;
    StaticJsonDocument<3000> &doc = jsonEncodingDoc;
    JsonObject devicename = doc.createNestedObject();
    devicename["name"] = "devicename";
    devicename["type"] = "value";
    devicename["value"] = prefsDeviceName;

    JsonMetaCompositeText(doc.createNestedObject());
    JsonMetaCompositeBitmap(doc.createNestedObject());
    JsonMetaOutputs(doc.createNestedObject());
    JsonMetaLeds(doc.createNestedObject());

    JsonMetaSettings(doc.createNestedObject());

    JsonMetaOptsColor(doc.createNestedObject());
    JsonMetaOptsBitmap(doc.createNestedObject());
    JsonMetaOptsLed(doc.createNestedObject());
    JsonMetaOptsAlignment(doc.createNestedObject());

    metadataJson = "";
    serializeJson(doc, metadataJson);
}
