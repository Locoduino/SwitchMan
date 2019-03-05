#ifndef __SWITCHMAN_H__
#define __SWITCHMAN_H__

#include "Arduino.h"
#include "Track.h"
#include "TrackSet.h"
#include "PathSet.h"
#include "HeadedTrackSet.h"

#ifdef DEBUG

#define TRACK(tracktype,trackname) \
static const uint16_t trackname##_id = __COUNTER__; \
const char PROGMEM trackname##_name[] = #trackname; \
tracktype trackname(NAME_ARG_FIRST(trackname##_name) trackname##_id);

#else

#define TRACK(tracktype,trackname) \
static const uint16_t trackname##_id = __COUNTER__; \
tracktype trackname(trackname##_id);

#endif

#endif /* __SWITCHMAN_H__ */
