#include <Arduino.h>
#include <BLEDevice.h>

#include <emoBinCommand.h>

class emoCoreBLEDescriptorCallbacks : public BLEDescriptorCallbacks
{
public:
    void onWrite(BLEDescriptor *desc) override;
};