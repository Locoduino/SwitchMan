#ifndef __SWITCHMAN_H__
#define __SWITCHMAN_H__

#include "Arduino.h"

#define TRACK (type, name) \
static const uint16_t name##_id = __COUNTER__; \
type name(name##_id);


#endif /* __SWITCHMAN_H__ */
