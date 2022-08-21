#include "emoCoreBLECharacteristicCallbacks.h"
#include <emoCoreBLE.h>
#include <emoTextCommand.h>
#include <emoBinCommand.h>

std::string textMessageBuffer = "";
int textMessageExpected = 0;
volatile bool *notificationSend_;

emoCoreBLECharacteristicCallbacks::emoCoreBLECharacteristicCallbacks(volatile bool *inNotificationSend)
{
    notificationSend_ = inNotificationSend;
}

void emoCoreBLECharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic)
{
    // Serial.println("Characteristic Write 2");

    size_t dataLen = pCharacteristic->getLength();
    u_int8_t *rxData = pCharacteristic->getData();

    // Bin command
    if (isBinCommand(rxData, dataLen))
    {
        emoCoreBLE::sendBin(parseBinCommand(rxData, dataLen));
        return;
    }

    // check text command marker
    if ((dataLen > 3) && (*rxData == 0xbb) && (*(rxData + 1) == 0xaa))
    {
        textMessageExpected = *(rxData + 2) + (*(rxData + 3) * 256);
        textMessageBuffer.clear();
        for (int i = 4; i < dataLen; i++)
            textMessageBuffer += *(rxData + i);
    }
    else if (textMessageBuffer.length() > 0)
    {
        for (int i = 0; i < dataLen; i++)
            textMessageBuffer += *(rxData + i);
    }
    else
    {
        Serial.print("Unknown packet received:[");
        printBuffer(rxData, dataLen);
        Serial.println("]");
    }

    // check is textcommand is finaly assembled
    if (textMessageBuffer.length() > 0)
    {
        if (textMessageBuffer.length() >= textMessageExpected)
        {
            Serial.print("request:");
            Serial.println(textMessageBuffer.c_str());
            emoCoreBLE::sendText(parseTextCommand(textMessageBuffer.c_str()));

            textMessageBuffer.clear();
            textMessageExpected = 0;
        }
    }
}

void emoCoreBLECharacteristicCallbacks::onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
{
    // Serial.println("Characteristic Write 1");
    onWrite(pCharacteristic);
}

void emoCoreBLECharacteristicCallbacks::onNotify(BLECharacteristic *pCharacteristic)
{
    *notificationSend_ = true;
}
