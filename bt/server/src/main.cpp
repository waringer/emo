#include <Arduino.h>
#include <MyBLETest.h>

#define SERIALSPEED 115200

int buttonPin = GPIO_NUM_0;

void setup()
{
  Serial.begin(SERIALSPEED);
  pinMode(buttonPin, INPUT);

  MyBLETest::setup();
}

void loop()
{
  int buttonState = digitalRead(buttonPin);
  MyBLETest::loop(buttonState == HIGH);
}