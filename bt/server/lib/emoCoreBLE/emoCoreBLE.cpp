#include "emoCoreBLE.h"

unsigned long lastTime = 0;
unsigned long timerDelay = 60000;
bool deviceConnected, oldDeviceConnected = false;
volatile bool notificationSend = true;

uint8_t byte_array[20] = {0xdd, 0xcc, 0x00, 0x02, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
uint8_t byte_array_desc[2] = {0x00, 0x00};

std::string textResponseBuffer = "";
uint8_t binResponseBuffer[21] = {};

// Characteristic and Descriptor
BLECharacteristic bmeCharacteristics(CHARACTERISTIC_UUID, BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE_NR);
BLEDescriptor bmeDescriptor(BLEUUID((uint16_t)0x2902));
BLEAdvertising *pAdvertising;

void sendTextResponse()
{
  if (textResponseBuffer.length() != 0)
  {
    uint8_t buffer[21] = {};
    int bufferlen = 20;
    int pos = 0;

    buffer[0] = 0xBB;
    buffer[1] = 0xAA;
    buffer[2] = (textResponseBuffer.length() >> 0) & 0xFF;
    buffer[3] = (textResponseBuffer.length() >> 8) & 0xFF;

    if (textResponseBuffer.length() >= 16)
    {
      textResponseBuffer.copy(((char *)buffer) + 4, bufferlen - 4, pos);
      pos += bufferlen - 4;
    }
    else
    {
      bufferlen = textResponseBuffer.length() + 4;
      textResponseBuffer.copy(((char *)buffer) + 4, bufferlen, pos);
      pos += bufferlen;
    }

    do
    {
      while (!notificationSend)
      {
      }

      notificationSend = false;
      bmeCharacteristics.setValue(buffer, bufferlen);
      bmeCharacteristics.notify();

      if (textResponseBuffer.length() - pos >= 20)
      {
        textResponseBuffer.copy((char *)buffer, bufferlen, pos);
        pos += bufferlen;
      }
      else
      {
        if (textResponseBuffer.length() - pos != 0)
        {
          bufferlen = textResponseBuffer.length() - pos;
          textResponseBuffer.copy((char *)buffer, bufferlen, pos);
          pos += bufferlen;
        }
        else
          bufferlen = 0;
      }
    } while (bufferlen > 0);

    textResponseBuffer.clear();
  }
}

void sendBinResponse()
{
  if (binResponseBuffer[21] != 0x00)
  {
    notificationSend = false;
    bmeCharacteristics.setValue(binResponseBuffer, 20);
    bmeCharacteristics.notify();

    while (!notificationSend)
    {
    }

    binResponseBuffer[21] = 0x00;
  }
}

void emoCoreBLE::setup()
{
  BLEDevice::init(deviceName);

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new emoCoreBLEServerCallbacks(&deviceConnected));

  // Create the BLE Service
  BLEService *bmeService = pServer->createService(SERVICE_UUID);

  // Create BLE Characteristics and Create a BLE Descriptor
  bmeService->addCharacteristic(&bmeCharacteristics);

  bmeDescriptor.setValue(byte_array_desc, 2);
  bmeDescriptor.setCallbacks(new emoCoreBLEDescriptorCallbacks());

  bmeCharacteristics.addDescriptor(&bmeDescriptor);
  bmeCharacteristics.setCallbacks(new emoCoreBLECharacteristicCallbacks(&notificationSend, &textResponseBuffer, binResponseBuffer));

  // Start the service
  bmeService->start();

  // Start advertising
  pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);

  BLEAdvertisementData scanResData;
  scanResData.setName(serverName);
  scanResData.setShortName(serverName);
  pAdvertising->setScanResponseData(scanResData);

  BLEAdvertisementData advData;
  advData.setShortName(serverName);
  pAdvertising->setAdvertisementData(advData);

  pAdvertising->start();

  Serial.println("Waiting for a client connection to notify...");
}

void emoCoreBLE::loop(bool useAlt)
{
  if (deviceConnected)
  {
    sendTextResponse();
    sendBinResponse();

    // if ((millis() - lastTime) > timerDelay)
    // {
    //   while (!notificationSend)
    //   {
    //   }

    //   notificationSend = false;
    //   bmeCharacteristics.setValue(byte_array, 20);
    //   bmeCharacteristics.notify();
    //   Serial.println("notify");

    //   lastTime = millis();
    // }
  }

  // disconnecting
  if (!deviceConnected && oldDeviceConnected)
  {
    delay(500);            // give the bluetooth stack the chance to get things ready
    pAdvertising->start(); // restart advertising
    oldDeviceConnected = deviceConnected;
  }
  // connecting
  if (deviceConnected && !oldDeviceConnected)
  {
    // do stuff here on connecting
    oldDeviceConnected = deviceConnected;
    lastTime = millis();
  }
}
