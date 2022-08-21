#pragma once
#ifndef _EMOCOREBLE_H
#define _EMOCOREBLE_H

#include <Arduino.h>

class emoCoreBLE
{
public:
    static void setup();
    static void loop(bool useAlt);

    static void sendText(const char *textCommand);
    static void sendBin(u_int8_t binCommand[20]);
};
#endif
