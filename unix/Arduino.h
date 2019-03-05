#ifndef ARDUINO_H
#define ARDUINO_H

#include <stdint.h>
#include <stdlib.h>
#include "String.h"

#define F(str) str
#define PROGMEM

typedef uint8_t  byte;
typedef uint16_t word;
typedef bool boolean;

#define true 1
#define false 0

extern uint32_t millis();
extern uint32_t micros();

#define pgm_read_word(str) str
#define strcpy_P sm_strcpy

char * sm_strcpy(char * dst, const char * src);

#endif
