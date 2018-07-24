#ifndef __TRACK_H__
#define __TRACK_H__

#include "Arduino.h"

class PathSet;
class HeadedTrackSet;

#ifdef DEBUG
void displayConnectorName(const uint8_t inConnector, const bool inNewLine = false);


/*
 * Abstract class for tracks
 */
class Track
{
private:
  uint16_t mIdentifier : 14; /* Track identifier */
  uint8_t  mDirection  : 2;  /* Travel direction */

  static uint16_t sCount;    /* Declared track count */
  static Track ** sTracks;   /* Table of track pointers */

protected:
  void setDirection(const uint8_t inDir); /* Set the travelling direction */
  virtual void connectFromTrack(const Track * inTrack, const uint8_t inConnector) = 0;
  virtual bool allPathsTo(const uint16_t inId, const uint8_t inDir, PathSet &ioPaths, const Track * inFrom, HeadedTrackSet &inMarking) = 0;

public:
  Track();
  uint8_t direction()                 { return mDirection; }
  bool pathTo(Track & inTrack, uint8_t inDir, PathSet & ioPaths);
  bool pathTo(uint16_t inId, uint8_t inDir, PathSet & ioPaths);

  static const uint16_t count();      { return sCount; }
  static const uint8_t sizeForSet();  { return ((sNombre >> 3) + ((sNombre & 7) != 0)); }

  static void finalize();
  static Track & trackForId(uint16_t inId);
};

/*
 * Dead-end track
 */
class DeadEndTrack : public Track
{
private:
  Track * mOutTrack;  /* OUTBOUND track */
  uint16_t mLength;   /* length of the dead-end track */

protected:
  virtual void connectFromTrack(const Track * inTrack, const uint8_t inConnector) = 0;
  virtual bool allPathsTo(const uint16_t inId, const uint8_t inDir, PathSet &ioPaths, const Track * inFrom, HeadedTrackSet &inMarking) = 0;

public:
  DeadEndTrack(uint16_t inLength = 0u);
  void connectTo(Track & inTrack, uint8_t inConnector);
};

/*
 * Normal track
 */
class NormalTrack : public Track
{
private:
  Track * mInTrack;
  Track * mOutTrack;
  uint16_t mLength;   /* length of the normal track */

protected:
  virtual void connectFromTrack(const Track * inTrack, const uint8_t inConnector) = 0;
  virtual bool allPathsTo(const uint16_t inId, const uint8_t inDir, PathSet &ioPaths, const Track * inFrom, HeadedTrackSet &inMarking) = 0;

public:
  DeadendTrack(uint16_t inLength = 0u);
  void connectInTo(Track & inTrack, uint8_t inConnector);
  void connectOutTo(Track & inTrack, uint8_t inConnector);
};

/*
 * Turnout track
 */
class TurnoutTrack : public Track
{
private:
  Track * mInTrack;
  Track * mOutLeftTrack;
  Track * mOutRightTrack;
  uint16_t mLeftLength;
  uint16_t mRightLength;
  PathSet * mPartialPath;

protected:
  virtual void connectFromTrack(const Track * inTrack, const uint8_t inConnector) = 0;
  virtual bool allPathsTo(const uint16_t inId, const uint8_t inDir, PathSet &ioPaths, const Track * inFrom, HeadedTrackSet &inMarking) = 0;

public:
  TurnoutTrack(uint16_t inLeftLength = 0u, uint16_t inRightLength = 0u);
  void connectInTo(Track & inTrack, uint8_t inConnector);
  void connectOutLeftTo(Track & inTrack, uint8_t inConnector);
  void connectOutRightTo(Track & inTrack, uint8_t inConnector);
};

/*
 * Crossing track
 */
class CrossingTrack : public Track
{
protected:
  Track * mInLeftTrack;
  Track * mInRightTrack;
  Track * mOutLeftTrack;
  Track * mOutRightTrack;
  uint16_t mLeftToRightLength;
  uint16_t mRightToLeftLength;

  virtual void connectFromTrack(const Track * inTrack, const uint8_t inConnector) = 0;
  virtual bool allPathsTo(const uint16_t inId, const uint8_t inDir, PathSet &ioPaths, const Track * inFrom, HeadedTrackSet &inMarking) = 0;

public:
  CrossingTrack(uint16_t inLeftToRightLength = 0u, uint16_t inRightToLeftLength = 0u);
  void connectInLeftTo(Track & inTrack, uint8_t inConnector);
  void connectInRightTo(Track & inTrack, uint8_t inConnector);
  void connectOutLeftTo(Track & inTrack, uint8_t inConnector);
  void connectOutRightTo(Track & inTrack, uint8_t inConnector);
};

/*
 * Double slip track
 */
class DoubleSlipTrack : public CrossingTrack
{
private:
  PathSet * mPartialPath[2];

protected:
  virtual bool allPathsTo(const uint16_t inId, const uint8_t inDir, PathSet &ioPaths, const Track * inFrom, HeadedTrackSet &inMarking) = 0;

public:
  DoubleSlipTrack(uint16_t inLeftToRightLength = 0u, uint16_t inRightToLeftLength = 0u);
};

#endif /* __TRACK_H__ */
