#pragma once
#include <map>

#include "ArduinoJson.h"

#include "neopixeldisplay.h"

/***
 *     __   __        _      _    _        
 *     \ \ / /_ _ _ _(_)__ _| |__| |___ ___
 *      \ V / _` | '_| / _` | '_ \ / -_|_-<
 *       \_/\__,_|_| |_\__,_|_.__/_\___/__/
 *                                         
 */

//
// Preference variables
// Globally available. Synchronized with
// values stored in preferences
//
extern String prefsPassword;
extern String prefsSsid;
extern String prefsDeviceName;
extern int prefsLedColorA[];
extern int prefsLedColorB[];

//
// JSON status and metadata are maintained as strings;
// updated when changed. the REST and BLE interfaces 
// simply use the current values of the global string
//
extern String metadataJson;
extern String statusJson;


/***
 *      _  _              _         _ ___                           ___ _            
 *     | \| |___ ___ _ __(_)_ _____| | _ \__ _ _ _ __ _ _ __  ___  / __| |__ _ ______
 *     | .` / -_) _ \ '_ \ \ \ / -_) |  _/ _` | '_/ _` | '  \(_-< | (__| / _` (_-<_-<
 *     |_|\_\___\___/ .__/_/_\_\___|_|_| \__,_|_| \__,_|_|_|_/__/  \___|_\__,_/__/__/
 *                  |_|                                                              
 */

class NeopixelParams {
    struct parseFn {
        byte (*parse)(const String &value, int arg);
        int arg;
    };
    std::map<String, struct parseFn> parseMap;
public:
    void AddParser(String name, byte (*parse)(const String &value, int arg), int arg);
    byte parse(const String &name, const String &value);
};

//
// THE parameter definitions andnparameter parser
//
extern NeopixelParams params;

/***
 *      ___             _   _             
 *     | __|  _ _ _  __| |_(_)___ _ _  ___
 *     | _| || | ' \/ _|  _| / _ \ ' \(_-<
 *     |_| \_,_|_||_\__|\__|_\___/_||_/__/
 *                                        
 */

void NeoParamSetup();
void NeoParamTick();

void GetPreferences();
void SavePreferences();

extern int OutputCount();
int LedsCount();

void JsonEncodeNeopixelData();
void JsonEncodeNeopixelMetaData();

void JsonOutputsMeta(JsonObject obj);
void JsonLedsMeta(JsonObject obj);


