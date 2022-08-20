#include <Arduino.h>
#include <emoCoreBLE.h>

#define SERIALSPEED 115200

int buttonPin = GPIO_NUM_0;

void setup()
{
  Serial.begin(SERIALSPEED);
  pinMode(buttonPin, INPUT);

  emoCoreBLE::setup();
}

void loop()
{
  int buttonState = digitalRead(buttonPin);
  emoCoreBLE::loop(buttonState == HIGH);
}