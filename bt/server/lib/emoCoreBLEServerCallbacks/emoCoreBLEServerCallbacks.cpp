#include <emoCoreBLEServerCallbacks.h>

bool *deviceConnected_;

emoCoreBLEServerCallbacks::emoCoreBLEServerCallbacks(bool *inDeviceConnected)
{
    deviceConnected_ = inDeviceConnected;
}

void emoCoreBLEServerCallbacks::onConnect(BLEServer *pServer)
{
    *deviceConnected_ = true;
    Serial.println("Connected");
};

void emoCoreBLEServerCallbacks::onDisconnect(BLEServer *pServer)
{
    *deviceConnected_ = false;
    Serial.println("Disconnected");
}
