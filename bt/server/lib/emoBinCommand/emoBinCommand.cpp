#include "emoBinCommand.h"

bool isBinCommand(u_int8_t *inMessage, size_t dataLen)
{
    if (dataLen != 20)
        return false;

    // main command pattern
    bool isCommand = (*inMessage == 0xdd) && (*(inMessage + 1) == 0xcc);

    // debug command pattern
    isCommand |= (*inMessage == 0x55) && (*(inMessage + 1) == 0xaa) && (*(inMessage + 2) == 0x55) && (*(inMessage + 3) == 0xaa);

    return isCommand;
}

bool parseBinCommand(u_int8_t *inMessage, size_t dataLen, u_int8_t *outBuffer)
{
    if ((*inMessage == 0xdd) && (*(inMessage + 1) == 0xcc))
    {
        Serial.print("bin command type: [");
        printBuffer(inMessage + 3, 2);
        Serial.print("] - ");

        if ((*(inMessage + 3) == 0x02) && (*(inMessage + 4) == 0x00))
        {
            Serial.print("charge sub: [");
            printBuffer(inMessage + 5, 1);
            Serial.println("]");

            outBuffer[0] = 0xdd;
            outBuffer[1] = 0xcc;
            outBuffer[2] = 0x00;
            outBuffer[3] = 0x02;
            outBuffer[4] = 0x00;
            outBuffer[5] = 0x50;
            outBuffer[21] = 0xff;
            return true;
        }
        else if ((*(inMessage + 3) == 0x00) && (*(inMessage + 4) == 0x00))
        {
            Serial.print("action sub: [");
            printBuffer(inMessage + 5, 1);
            Serial.println("]");
            return false;
        }
    }

    if ((*inMessage == 0x55) && (*(inMessage + 1) == 0xaa) && (*(inMessage + 2) == 0x55) && (*(inMessage + 3) == 0xaa))
    {
        Serial.print("debug command: [");
        printBuffer(inMessage, dataLen);
        Serial.println("]");
        return false;
    }

    Serial.print("Unknown command received:[");
    printBuffer(inMessage, dataLen);
    Serial.println("]");

    return false;
}

void printByte(u_int8_t in)
{
    Serial.print("0x");
    Serial.print(in < 16 ? "0" : "");
    Serial.print(in, HEX);
}

void printBuffer(u_int8_t *inBuffer, size_t dataLen)
{
    for (int i = 0; i < dataLen - 1; i++)
    {
        printByte(*(inBuffer + i));
        Serial.print(",");
    }
    printByte(*(inBuffer + dataLen - 1));
}
