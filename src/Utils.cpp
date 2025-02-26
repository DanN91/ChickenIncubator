#include "Utils.h"

void SerialDebug(const String& message)
{
#ifdef SERIAL_DEBUG
    Serial.println(message);
#endif // SERIAL_DEBUG
}