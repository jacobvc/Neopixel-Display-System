#include <WiFi.h>
#include <WiFiClient.h>
#include <ESPAsyncWebSrv.h>
#include <ESPmDNS.h>
#include <esp_wifi.h>

#include "AsyncJson.h"

#include "config.h"
#include "neopixelparams.h"
#include "neopixeldisplay.h"
#include "neopixelble.h"
#include "neopixelwifi.h"

#include "assets/webcontent.min.h"
//#include "assets/webcontent.h"

/***
 *      __  __
 *     |  \/  |  __ _   ___  _ __  ___   ___
 *     | |\/| | / _` | / __|| '__|/ _ \ / __|
 *     | |  | || (_| || (__ | |  | (_) |\__ \
 *     |_|  |_| \__,_| \___||_|   \___/ |___/
 *
 */

// A macro just to reduce file clutter
#define HOOK_ASYNC_REQUEST(path, handler) \
    server.on(path, [](AsyncWebServerRequest *request) { handler(request); })

/***
 *     __     __            _         _      _
 *     \ \   / /__ _  _ __ (_)  __ _ | |__  | |  ___  ___
 *      \ \ / // _` || '__|| | / _` || '_ \ | | / _ \/ __|
 *       \ V /| (_| || |   | || (_| || |_) || ||  __/\__ \
 *        \_/  \__,_||_|   |_| \__,_||_.__/ |_| \___||___/
 *
 */
AsyncWebServer server(80);

void processArgs(AsyncWebServerRequest *request);

/***
 *      _   _  _    _            _                        _  _
 *     | | | || |_ | |_  _ __   | |__    __ _  _ __    __| || |  ___  _ __  ___
 *     | |_| || __|| __|| '_ \  | '_ \  / _` || '_ \  / _` || | / _ \| '__|/ __|
 *     |  _  || |_ | |_ | |_) | | | | || (_| || | | || (_| || ||  __/| |   \__ \
 *     |_| |_| \__| \__|| .__/  |_| |_| \__,_||_| |_| \__,_||_| \___||_|   |___/
 *                      |_|
 */
void processArgs(AsyncWebServerRequest *request)
{
    String error = "";
    for (uint8_t i = 0; i < request->args(); i++)
    {
        if (!params.parse(request->argName(i), request->arg(i)))
        {
            error += "! " + request->argName(i) + " (" + request->arg(i) + ") FAILED ! ";
        }
    }
}

void handleRoot(AsyncWebServerRequest *request)
{
    Serial.println("Handle root");
    processArgs(request);

    AsyncWebServerResponse *response = request->beginResponse(200, "text/html", index_html);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void handleDynamicViewJs(AsyncWebServerRequest *request)
{
    Serial.println("Handle neopixel.js");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/javascript", dynamicview_js);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void handleDynamicViewCss(AsyncWebServerRequest *request)
{
    Serial.println("Handle neopixel.css");
    AsyncWebServerResponse *response = request->beginResponse(200, "text/css", dynamicview_css);
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void handleStatus(AsyncWebServerRequest *request)
{
    Serial.println("Handle status");
    processArgs(request);
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", statusJson.c_str());

    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void handleMetaData(AsyncWebServerRequest *request)
{
   Serial.println("Handle metadata");
     processArgs(request);
    AsyncWebServerResponse *response = request->beginResponse(200, "application/json", metadataJson.c_str());
    
    response->addHeader("Access-Control-Allow-Origin", "*");
    request->send(response);
}

void handleNotFound(AsyncWebServerRequest *request)
{
    String message = "File Not Found\n\n";
    message += "URI: ";
    message += request->url();
    message += "\nMethod: ";
    message += (request->method() == HTTP_GET) ? "GET" : "POST";
    message += "\nArguments: ";
    message += request->args();
    message += "\n";

    for (uint8_t i = 0; i < request->args(); i++)
    {
        message += " " + request->argName(i) + ": " + request->arg(i) + "\n";
    }

    request->send(404, "text/plain", message);
}


/***
 *      ____                 _     _             _             
 *     |  _ \ _ __ _____   _(_)___(_) ___  _ __ (_)_ __   __ _ 
 *     | |_) | '__/ _ \ \ / / / __| |/ _ \| '_ \| | '_ \ / _` |
 *     |  __/| | | (_) \ V /| \__ \ | (_) | | | | | | | | (_| |
 *     |_|   |_|  \___/ \_/ |_|___/_|\___/|_| |_|_|_| |_|\__, |
 *                                                       |___/ 
 */
enum ProvisioningState
{
    UNKNOWN,
    SmartConfiging,
    TryingConnect,
    Connected,
    GaveUp
};

static String getSsid()
{
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    return String(reinterpret_cast<const char *>(conf.sta.ssid));
}
static String getPwd()
{
    wifi_config_t conf;
    esp_wifi_get_config(WIFI_IF_STA, &conf);
    return String(reinterpret_cast<const char *>(conf.sta.password));
}
static ProvisioningState provisioningState = UNKNOWN;
static bool wifiConnected = false;
static int triesLeft = 0;

void NeoWifiTryConnect()
{
    bool wasConnected = wifiConnected;
    switch (provisioningState)
    {
    case UNKNOWN:
        WiFi.mode(WIFI_STA);
        WiFi.begin(prefsSsid.c_str(), prefsPassword.c_str());
        provisioningState = TryingConnect;
        triesLeft = 20; // 10 sec
        display.SetLed(3, LedMode::blink);
        break;
    case SmartConfiging:
        if (WiFi.smartConfigDone())
        {
            display.ShowText("SmartConfig", 5, Alignment::scroll);
            display.SetLed(3, LedMode::blink);
            Serial.printf("SmartConfig received.\n Waiting for WiFi");
            provisioningState = TryingConnect;
            triesLeft = 20; // 10 sec
            if (!getSsid().equals(prefsSsid) || !getPwd().equals(prefsPassword)) {
                prefsSsid = getSsid();
                prefsPassword = getPwd();
                SavePreferences();
            }
        }
        if (--triesLeft <= 0)
        {
            provisioningState = GaveUp;
        }
        break;
    case TryingConnect:
        // Serial.println("");

        // Wait for connection
        if (WiFi.status() == WL_CONNECTED)
        {
           display.SetLed(3, LedMode::off);
            wifiConnected = true;
            provisioningState = Connected;

            // Serial.println("");
            Serial.print("Connected to ");
            Serial.println(prefsSsid);
            Serial.print("IP address: ");
            Serial.println(WiFi.localIP());

            char buf[20];
            sprintf(buf, "%d.%d.%d.%d", WiFi.localIP()[0], WiFi.localIP()[1], WiFi.localIP()[2], WiFi.localIP()[3]);

            display.ShowText(buf, 5, Alignment::scroll);
            JsonEncodeNeopixelData();

            if (!wasConnected)
            {
                server.begin();
                Serial.println("HTTP server started");
            }

            if (MDNS.begin(prefsDeviceName.c_str()))
            {
                Serial.println("MDNS responder started");
            }

            MDNS.addService("http", "tcp", 80);
        }
        if (--triesLeft <= 0)
        {
            WiFi.mode(WIFI_AP_STA); // Init WiFi, start SmartConfig
            Serial.printf("Entering SmartConfig\n");

            display.SetLed(3, LedMode::off);
            display.SetLed(2, LedMode::blink);
            WiFi.beginSmartConfig();

            provisioningState = SmartConfiging;
            triesLeft = 600; // about 5 minutes
        }
        break;
    case Connected:
        break;
    case GaveUp:
        display.SetLed(2, LedMode::blink);
        break;
    }
}

/***
 *      ____         _
 *     / ___|   ___ | |_  _   _  _ __
 *     \___ \  / _ \| __|| | | || '_ \
 *      ___) ||  __/| |_ | |_| || |_) |
 *     |____/  \___| \__| \__,_|| .__/
 *                              |_|
 */

void NeoWifiSetup(void)
{
    // digitalWrite(BUILTIN_LED, 0);
    HOOK_ASYNC_REQUEST("/", handleRoot);
    HOOK_ASYNC_REQUEST("/status", handleStatus);
    HOOK_ASYNC_REQUEST("/metadata", handleMetaData);
    HOOK_ASYNC_REQUEST("/dynamicview.js", handleDynamicViewJs);
    HOOK_ASYNC_REQUEST("/dynamicview.css", handleDynamicViewCss);

    server.onNotFound([](AsyncWebServerRequest *request)
                      { handleNotFound(request); });
}

/***
 *      _____ _      _
 *     |_   _(_) ___| | __
 *       | | | |/ __| |/ /
 *       | | | | (__|   <
 *       |_| |_|\___|_|\_\
 *
 */

static unsigned long previous500ms = 0; // Last 500ms interval

void NeoWifiTick(void)
{
    unsigned long currentTime = millis();
    if (currentTime - previous500ms > 500)
    {
        previous500ms = currentTime;
        NeoWifiTryConnect();
    }
}
