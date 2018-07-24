#ifndef __SWITCHMAN_H__
#define __SWITCHMAN_H__

#include "Arduino.h"

#define TRACK (type, name) \
static const uint16_t name##_id = __COUNTER__; \
type name(name##_id);

/*
 * Direction of travel
 */
enum {
  FORWARD_DIRECTION   = 0,
  BACKWARD_DIRECTION  = 1,
  NO_DIRECTION        = 2
};

/*
 * Connectors of a track element
 */
enum {
  INBOUND,
  LEFT_INBOUND,
  RIGHT_INBOUND,
  OUTBOUND,
  LEFT_OUTBOUND,
  RIGHT_OUTBOUND
};


#endif /* __SWITCHMAN_H__ */
