/*
 * PathSet : A Trackset list
 */
#ifndef __PATHSET_H__
#define __PATHSET_H__

#include "TrackSet.h"

class PathSet;

class Path : public TrackSet
{
  private:
    Path *mNext; /* To chain */
    friend class PathSet;

  public:
    Path() : TrackSet() { mNext = NULL; }
    Path(const Path & inPath) : TrackSet(inPath) { mNext = NULL; }
    bool fitWith(const Path & inPath);
};

/*
 * Set of paths from a departure track to a track
 * of arrival following a direction
 */
class PathSet
{
  private:
    Path *mListHead;
    uint8_t mDeparture;
    uint8_t mArrival;
    Direction mDirection;

    void clear();
    void copy(PathSet & inSet);

  public:
    PathSet() { mListHead = new Path(); }
    PathSet(PathSet & inSet);
    ~PathSet();
    void addTrack(uint16_t inId);
    void addTrack(Track * inTrack) { addTrack(inTrack->identifier()); }
    bool containsPath(Path & inPath);
    PathSet & operator+=(PathSet & inSet);
    PathSet & operator=(PathSet & inSet);
    uint16_t count();
#ifdef DEBUG
    void print();
    void println();
#endif
};

#endif
