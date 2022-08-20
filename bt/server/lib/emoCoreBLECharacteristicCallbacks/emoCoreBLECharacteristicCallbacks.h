#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#include <emoTextCommand.h>
#include <emoBinCommand.h>

class emoCoreBLECharacteristicCallbacks : public BLECharacteristicCallbacks
{
public:
    emoCoreBLECharacteristicCallbacks(volatile bool *inNotificationSend, std::string *inTextResponseBuffer, uint8_t inBinResponseBuffer[21]);

    void onWrite(BLECharacteristic *pCharacteristic) override;
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param);
    void onNotify(BLECharacteristic *pCharacteristic);
};
