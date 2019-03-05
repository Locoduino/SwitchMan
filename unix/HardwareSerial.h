#ifndef HARDWARESERIAL_H
#define HARDWARESERIAL_H

#include <iostream>
#include <iomanip>
#include <stdlib.h>

#define DEC 0
#define HEX 1

std::ostream& fixeBase(std::ostream &stream, int base);

class HardwareSerial
{
  public:
    void print(const int val, int base = DEC)             { fixeBase(std::cout, base) << val; }
    void println(const int val, int base = DEC)           { fixeBase(std::cout, base) << val << std::endl; }
    void print(const unsigned long val, int base = DEC)   { fixeBase(std::cout, base) << val; }
    void println(const unsigned long val, int base = DEC) { fixeBase(std::cout, base) << val << std::endl; }
    void print(const uint8_t val, int base = DEC)         { fixeBase(std::cout, base) << val; }
    void println(const uint8_t val, int base = DEC)       { fixeBase(std::cout, base) << val << std::endl; }
    void print(const char car)            { std::cout << car; }
    void println(const char car)          { std::cout << car << std::endl; }
    void print(const char *str)           { std::cout << str; }
    void println(const char *str)         { std::cout << str << std::endl; }
    void println()                        { std::cout << std::endl; }
    void begin(const int speed)           {}
};

extern HardwareSerial Serial;

#endif
