#include "HardwareSerial.h"

std::ostream& fixeBase(std::ostream &stream, int base)
{
  switch (base) {
    case DEC : stream << std::dec; break;
    case HEX : stream << std::hex; break;
    default  : stream << std::dec; break;
  }
  return stream;
}

HardwareSerial Serial;