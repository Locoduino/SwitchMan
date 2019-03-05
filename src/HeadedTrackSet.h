/*
* Set of oriented tracks.
* The set is a bit vector indexed by the identifier
* of the channels * 2 + the direction: 0 forward direction, 1 reverse direction.
* Each bit indicates the belonging of a track to the set
* - 1 the Way belongs to the whole
* - 0 it does not belong to the whole.
 */
#ifndef __HEADEDTRACKSET_H__
#define __HEADEDTRACKSET_H__

#include "HardwareSerial.h"
#include "Track.h"

class HeadedTrackSet
{
  private:
    void allocate();

  protected:
    uint8_t *mSet;

  public:
    HeadedTrackSet(); /* Build an empty set */
    HeadedTrackSet(const HeadedTrackSet & set);
    ~HeadedTrackSet();
    void clear();
    void addTrack(uint8_t inId, uint8_t inDir);
    void addTrack(Track & inTrack, uint8_t inDir) { addTrack(inTrack.identifier(), inDir);  }
    void addTrack(Track * inTrack, uint8_t inDir) { addTrack(inTrack->identifier(), inDir); }
    void removeTrack(uint8_t inId, uint8_t inDir);
    void removeTrack(Track & inTrack, uint8_t inDir) { removeTrack(inTrack.identifier(), inDir);  }
    void removeTrack(Track * inTrack, uint8_t inDir) { removeTrack(inTrack->identifier(), inDir); }
    bool containsTrack(uint8_t inId, uint8_t inDir);
    bool isEmpty();
    bool containsTrack(Track * inTrack, uint8_t inDir) { return containsTrack(inTrack->identifier(), inDir); }
    bool containsTrack(Track & inTrack, uint8_t inDir) { return containsTrack(inTrack.identifier(), inDir);  }
    HeadedTrackSet &operator=(const HeadedTrackSet & inSet);
    bool operator==(HeadedTrackSet & inSet);
#ifdef DEBUG
    void print();
    void println();
    void printraw();
    void printlnraw();
#endif
};

#endif
