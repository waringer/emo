#include <Arduino.h>
#include <BLEDevice.h>

class emoCoreBLECharacteristicCallbacks : public BLECharacteristicCallbacks
{
public:
    emoCoreBLECharacteristicCallbacks(volatile bool *inNotificationSend);

    void onWrite(BLECharacteristic *pCharacteristic) override;
    void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param);
    void onNotify(BLECharacteristic *pCharacteristic);
};
