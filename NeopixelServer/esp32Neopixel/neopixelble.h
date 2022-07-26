#pragma once

/***
 *      ___ _    ___   ___              _
 *     | _ ) |  | __| / __| ___ _ ___ _(_)__ ___
 *     | _ \ |__| _|  \__ \/ -_) '_\ V / / _/ -_)
 *     |___/____|___| |___/\___|_|  \_/|_\__\___|
 *
 */
#define BLE_SERVER_NAME "Neopixel BLE"
#define SERVICE_UUID "991e4749-bdb3-4e79-96d8-9a1b20cd51ab"
#define METADATA_1_CHARACTERISTIC_UUID "9da99ba6-6a55-4bfd-aee8-401fdcd69bf5"
#define METADATA_2_CHARACTERISTIC_UUID "76cd10b8-c3ea-4c16-a589-771506833f1c"
#define STATUS_CHARACTERISTIC_UUID "c0126900-f29f-4baf-8d8d-9e313d881339"

/***
 *      ___             _   _
 *     | __|  _ _ _  __| |_(_)___ _ _  ___
 *     | _| || | ' \/ _|  _| / _ \ ' \(_-<
 *     |_| \_,_|_||_\__|\__|_\___/_||_/__/
 *
 */

void NeoBleSetup();
void NeoBleTick();

void NeoBleSendStatus();
