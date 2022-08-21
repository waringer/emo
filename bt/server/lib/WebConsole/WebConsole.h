#pragma once
#ifndef _WEBCONSOLE_H
#define _WEBCONSOLE_H

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <emoCoreBLE.h>

#include "SPIFFS.h"

#define WLAN_SSID "***"
#define WLAN_PASSWORD "***"

class WebConsole
{
public:
    static void setup();
    static void loop();
};

void handle_OnIndex();
void handle_OnStyle();
void handle_Onfavicon();
void handle_NotFound();
String SendFile(const char *Filename);
String SendHTML();
#endif
