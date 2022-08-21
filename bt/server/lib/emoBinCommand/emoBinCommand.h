#include <Arduino.h>

bool isBinCommand(u_int8_t *inMessage, size_t dataLen);
u_int8_t *parseBinCommand(u_int8_t *inMessage, size_t dataLen);

void printByte(u_int8_t in);
void printBuffer(u_int8_t *inBuffer, size_t dataLen);
