#include <Arduino.h>
#include <emoCoreBLE.h>
#include <WebConsole.h>

#include "SPIFFS.h"

#define SERIALSPEED 115200

int buttonPin = GPIO_NUM_0;

void setup()
{
  Serial.begin(SERIALSPEED);
  pinMode(buttonPin, INPUT);

  if (!SPIFFS.begin(true))
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WebConsole::setup();
  emoCoreBLE::setup();
}

void loop()
{
  int buttonState = digitalRead(buttonPin);
  emoCoreBLE::loop(buttonState == HIGH);
  WebConsole::loop();
}