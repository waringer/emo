#include "MyBLETest.h"

unsigned long lastTime = 0;
unsigned long timerDelay = 60000;
bool deviceConnected, oldDeviceConnected = false;
volatile bool notificationSend = true;

uint8_t byte_array[20] = {0xdd, 0xcc, 0x00, 0x02, 0x00, 0x50, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x00};
uint8_t byte_array_desc[2] = {0x00, 0x00};

std::string textMessageBuffer = "";
int textMessageExpected = 0;
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

    // Serial.print("textlen:");
    // Serial.println(textResponseBuffer.length());

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
      // Serial.print("send...len:");
      // Serial.print(bufferlen);
      // Serial.print("...value:");
      // Serial.println((char *)buffer);

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

    // Serial.println("sent");
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

// Setup callbacks onConnect and onDisconnect
class MyServerCallbacks : public BLEServerCallbacks
{
  void onConnect(BLEServer *pServer)
  {
    deviceConnected = true;
    Serial.println("Connected");
  };
  void onDisconnect(BLEServer *pServer)
  {
    deviceConnected = false;
    Serial.println("Disconnected");
  }
};

class MyCharacteristicCallBack : public BLECharacteristicCallbacks
{
public:
  void onWrite(BLECharacteristic *pCharacteristic) override
  {
    // Serial.println("Characteristic Write 2");

    size_t dataLen = pCharacteristic->getLength();
    if (dataLen > 3)
    {
      u_int8_t *rxData = pCharacteristic->getData();
      if ((dataLen == 20) && (*rxData == 0xdd) && (*(rxData + 1) == 0xcc))
      {
        for (int i = 0; i < 21; i++)
          binResponseBuffer[i] = 0x00;

        if ((*rxData == 0xdd) && (*(rxData + 1) == 0xcc))
        {
          Serial.print("type:");
          Serial.print(*(rxData + 3), HEX);
          Serial.println(*(rxData + 4), HEX);

          if ((*(rxData + 3) == 0x02) && (*(rxData + 4) == 0x00))
          {
            Serial.print("charge sub:");
            Serial.println(*(rxData + 5), HEX);

            binResponseBuffer[0] = 0xdd;
            binResponseBuffer[1] = 0xcc;
            binResponseBuffer[2] = 0x00;
            binResponseBuffer[3] = 0x02;
            binResponseBuffer[4] = 0x00;
            binResponseBuffer[5] = 0x50;
            binResponseBuffer[21] = 0xff;
          }
        }
        return;
      }
      else
      {
        if ((*rxData == 0xbb) && (*(rxData + 1) == 0xaa))
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
      }
    }

    if (textMessageBuffer.length() > 0)
    {
      if (textMessageBuffer.length() >= textMessageExpected)
      {
        Serial.print("Got Text:");
        Serial.println(textMessageBuffer.c_str());
        parseTextMessage(pCharacteristic);

        textMessageBuffer.clear();
        textMessageExpected = 0;
      }
    }
  }

  void onWrite(BLECharacteristic *pCharacteristic, esp_ble_gatts_cb_param_t *param)
  {
    // Serial.println("Characteristic Write 1");
    onWrite(pCharacteristic);
  }

  void onNotify(BLECharacteristic *pCharacteristic)
  {
    notificationSend = true;
  }

  void parseTextMessage(BLECharacteristic *pCharacteristic)
  {
    DynamicJsonDocument doc(1024);
    DeserializationError error = deserializeJson(doc, textMessageBuffer.c_str());

    if (error)
    {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
    }
    else
    {
      std::string reqType((const char *)doc["type"]);

      Serial.print("type:");
      Serial.println(reqType.c_str());

      if (reqType == "sta_req")
      {
        std::string responce = "{\"type\":\"sta_rsp\",\"data\":{";

        bool needComma = false;
        for (int i = 0; i < doc["data"]["request"].size(); i++)
        {
          long reqID = doc["data"]["request"][i];
          if (needComma)
            responce += ",";
          else
            needComma = true;

          switch (reqID)
          {
          case 1:
            responce += "\"version\":{\"number\":22,\"name\":\"1.5.0\"}";
            break;
          case 2:
            responce += "\"network\":{\"connected\":1,\"name\":\"wlan ssid\"}";
            break;
          case 7:
            responce += "\"city\":{\"id\":0,\"name\":\"Bonn\",\"state\":\"\",\"country\":\"\",\"coord\":{\"lon\":0,\"lat\":0}}";
            break;
          case 8:
            responce += "\"timezone\":{\"name\":\"Europe/Berlin\",\"code\":\"\",\"offset\":7200}";
            break;
          case 11:
            responce += "\"personality\":{\"name\":\"EMO Name\",\"age\":2000,\"birthday\":\"12.31.\",\"color\":\"pink\",\"lucky_number\":99,\"sign\":\"leo\",\"number\":\"eeff\"}";
            break;

          default:
            responce += "";
            break;
          }
        }

        responce += "}}";

        Serial.print("responce:");
        Serial.println(responce.c_str());
        textResponseBuffer += responce;
      }

      if (reqType == "setting_req")
        textResponseBuffer += "{\"type\":\"setting_rsp\",\"data\":{\"result\":1}}";
    }
  }

  class MyDescrCallBack : public BLEDescriptorCallbacks
  {
  public:
    void onWrite(BLEDescriptor *desc) override
    {
      Serial.println("desc. data received!");
      size_t len = desc->getLength();
      Serial.print("len:");
      Serial.println(len);
      Serial.println((char *)desc->getValue());
    }
  };

  void MyBLETest::setup()
  {
    BLEDevice::init(deviceName);

    // Create the BLE Server
    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *bmeService = pServer->createService(SERVICE_UUID);

    // Create BLE Characteristics and Create a BLE Descriptor
    bmeService->addCharacteristic(&bmeCharacteristics);

    bmeDescriptor.setValue(byte_array_desc, 2);
    bmeDescriptor.setCallbacks(new MyDescrCallBack());

    bmeCharacteristics.addDescriptor(&bmeDescriptor);
    bmeCharacteristics.setCallbacks(new MyCharacteristicCallBack());

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

  void MyBLETest::loop(bool useAlt)
  {
    if (deviceConnected)
    {
      sendTextResponse();
      sendBinResponse();

      if ((millis() - lastTime) > timerDelay)
      {
        while (!notificationSend)
        {
        }

        notificationSend = false;
        bmeCharacteristics.setValue(byte_array, 20);
        bmeCharacteristics.notify();
        Serial.println("notify");

        lastTime = millis();
      }
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
