#include <emoCoreBLEDescriptorCallbacks.h>

void emoCoreBLEDescriptorCallbacks::onWrite(BLEDescriptor *desc)
{
    Serial.print("descriptor data received: [");
    printBuffer(desc->getValue(), desc->getLength());
    Serial.println("]");
}
