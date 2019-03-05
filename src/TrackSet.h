/*
 * TrackSet.
 * The set is a bit vector indexed by the identifier
 * of the tracks. Each bit indicates the belonging of a track to the set
 * - 1 the Way belongs to the set
 * - 0 it does not belong to the set.
 */
#ifndef __TRACKSET_H__
#define __TRACKSET_H__

#include "HardwareSerial.h"
#include "Track.h"

class TrackSet
{
  private:
    void allocate();

  protected:
    uint8_t *mSet;

  public:
    TrackSet(); /* Construit un ensemble vide */
    TrackSet(const TrackSet & set);
    ~TrackSet();
    void clear();
    bool isEmpty();
    void addTrack(const uint8_t inId);
    void addTrack(const Track & inTrack) { addTrack(inTrack.identifier()); }
    void addTrack(const Track * inTrack) { addTrack(inTrack->identifier()); }
    void removeTrack(const uint8_t inId);
    void removeTrack(const Track & inTrack) { removeTrack(inTrack.identifier()); }
    void removeTrack(const Track * inTrack) { removeTrack(inTrack->identifier()); }
    bool containsTrack(const uint8_t inId);
    bool containsTrack(const Track * inTrack) { return containsTrack(inTrack->identifier()); }
    bool containsTrack(const Track & inTrack) { return containsTrack(inTrack.identifier()); }
    TrackSet & operator=(const TrackSet & set);
    bool operator==(TrackSet & set);

#ifdef DEBUG
    void print();
    void println();
#endif
};

#endif
