#pragma once
#ifndef _EMOCOREBLE_H
#define _EMOCOREBLE_H

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>


#include <emoCoreBLEServerCallbacks.h>
#include <emoCoreBLECharacteristicCallbacks.h>
#include <emoCoreBLEDescriptorCallbacks.h>

#define deviceName "ESP32"
#define serverName "EMO-FFFF"
#define SERVICE_UUID "0000ffe0-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "0000ffe1-0000-1000-8000-00805f9b34fb"

class emoCoreBLE
{
public:
    static void setup();
    static void loop(bool useAlt);
};
#endif
