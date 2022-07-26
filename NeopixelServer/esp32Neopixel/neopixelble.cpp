#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <BLECharacteristic.h>

#include "neopixelparams.h"
#include "neopixelble.h"

/***
 *     __     __         _       _     _
 *     \ \   / /_ _ _ __(_) __ _| |__ | | ___  ___
 *      \ \ / / _` | '__| |/ _` | '_ \| |/ _ \/ __|
 *       \ V / (_| | |  | | (_| | |_) | |  __/\__ \
 *        \_/ \__,_|_|  |_|\__,_|_.__/|_|\___||___/
 *
 */
BLEServer *pServer = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

BLECharacteristic *pMetadata1Characteristic;
BLECharacteristic *pMetadata2Characteristic;
BLECharacteristic *pStatusCharacteristic;

/***
 *       ____      _ _ _                _
 *      / ___|__ _| | | |__   __ _  ___| | _____
 *     | |   / _` | | | '_ \ / _` |/ __| |/ / __|
 *     | |__| (_| | | | |_) | (_| | (__|   <\__ \
 *      \____\__,_|_|_|_.__/ \__,_|\___|_|\_\___/
 *
 */
class MyServerCallbacks : public BLEServerCallbacks
{
    void onConnect(BLEServer *pServer)
    {
        deviceConnected = true;
        BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer *pServer)
    {
        deviceConnected = false;
    }
};

class StatusCallbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic)
    {
        pCharacteristic->setValue(statusJson.c_str());
    }
    void onWrite(BLECharacteristic *pCharacteristic)
    {
        uint8_t *data = pCharacteristic->getData();
        StaticJsonDocument<200> doc;
        DeserializationError error = deserializeJson(doc, data);

        // Test if parsing succeeds.
        if (error)
        {
            Serial.print(F("deserializeJson() failed: "));
            Serial.println(error.f_str());
            return;
        }
        JsonObject root = doc.as<JsonObject>();
        for (JsonPair kv : root)
        {
            String key = kv.key().c_str();
            String value = kv.value().as<const char *>();
            if (!params.parse(key, value))
            {
                // error += "! " + request->argName(i) + " (" + request->arg(i) + ") FAILED ! ";
            }
            Serial.println(kv.key().c_str());
            Serial.println(kv.value().as<const char *>());
        }
    }
};

class Metadata1Callbacks : public BLECharacteristicCallbacks
{
    // This is how to send a characteristic bigger than 600 (ESP_GATT_MAX_ATTR_LEN) bytes
    void onRead(BLECharacteristic *pCharacteristic)
    {
        int length = metadataJson.length();
        if (length >= ESP_GATT_MAX_ATTR_LEN)
        {
            pCharacteristic->setValue(metadataJson.substring(0, ESP_GATT_MAX_ATTR_LEN).c_str());
        }
        else
        {
            pCharacteristic->setValue(metadataJson.c_str());
        }
    }
};

class Metadata2Callbacks : public BLECharacteristicCallbacks
{
    void onRead(BLECharacteristic *pCharacteristic)
    {
        int length = metadataJson.length();
        if (length > ESP_GATT_MAX_ATTR_LEN)
        {
            pCharacteristic->setValue(metadataJson.substring(ESP_GATT_MAX_ATTR_LEN).c_str());
        }
        else
        {
            pCharacteristic->setValue("");
        }
    }
};

void NeoBleSendStatus()
{
    pStatusCharacteristic->setValue(statusJson.c_str());
    pStatusCharacteristic->notify(true);
}

/***
 *               _
 *      ___  ___| |_ _   _ _ __
 *     / __|/ _ \ __| | | | '_ \
 *     \__ \  __/ |_| |_| | |_) |
 *     |___/\___|\__|\__,_| .__/
 *                        |_|
 */
void NeoBleSetup()
{
    // BLE Init
    BLEDevice::init(prefsDeviceName.c_str());
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);
    pMetadata1Characteristic = pService->createCharacteristic(
        METADATA_1_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ);
    pMetadata1Characteristic->addDescriptor(new BLE2902());
    pMetadata1Characteristic->setCallbacks(new Metadata1Callbacks());

    pMetadata2Characteristic = pService->createCharacteristic(
        METADATA_2_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ);
    pMetadata2Characteristic->addDescriptor(new BLE2902());
    pMetadata2Characteristic->setCallbacks(new Metadata2Callbacks());

    pStatusCharacteristic = pService->createCharacteristic(
        STATUS_CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE |
            BLECharacteristic::PROPERTY_NOTIFY);
    pStatusCharacteristic->setCallbacks(new StatusCallbacks());
    pStatusCharacteristic->addDescriptor(new BLE2902());

    pService->start();
    // BLEAdvertising *pAdvertising = pServer->getAdvertising();  // this still is working for backward compatibility
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06); // functions that help with iPhone connections issue
    pAdvertising->setMinPreferred(0x12);
    BLEDevice::startAdvertising();
}

/***
 *      _____ _      _
 *     |_   _(_) ___| | __
 *       | | | |/ __| |/ /
 *       | | | | (__|   <
 *       |_| |_|\___|_|\_\
 *
 */
void NeoBleTick()
{
    //
    // Maintain BLE Connection
    //
    if (deviceConnected)
    {
        // Do connected things
        // delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting transition
        oldDeviceConnected = deviceConnected;
    }
}
