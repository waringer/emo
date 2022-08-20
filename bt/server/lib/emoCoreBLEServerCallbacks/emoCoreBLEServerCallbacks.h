#include <Arduino.h>
#include <BLEServer.h>

class emoCoreBLEServerCallbacks : public BLEServerCallbacks
{
public:
    emoCoreBLEServerCallbacks(bool *inDeviceConnected);

    void onConnect(BLEServer *pServer);
    void onDisconnect(BLEServer *pServer);
};