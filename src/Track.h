#ifndef __TRACK_H__
#define __TRACK_H__

#include "Arduino.h"

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
  INLET,
  LEFT_INLET,
  RIGHT_INLET,
  OUTLET,
  LEFT_OUTLET,
  RIGHT_OUTLET
};

class PathSet;
class HeadedTrackSet;

#ifdef DEBUG
void displayConnectorName(
  const uint8_t inConnector,
  const bool inNewLine = false
);
void displayDirection(uint8_t inDir);
#endif

#ifdef DEBUG
#define N(name) F("name")
#define N_ const __FlashStringHelper * inName
#define N__ const __FlashStringHelper * inName,
#define _N inName
#else
#define N(name)
#define N_
#define N__
#define _N
#endif

/*
 * Abstract class for tracks
 */
class Track
{
private:
#ifdef DEBUG
  __FlashStringHelper * mName;
#endif
  uint16_t mIdentifier : 14; /* Track identifier */
  uint8_t  mDirection  : 2;  /* Travel direction */

  static uint16_t sCount;           /* Declared track count */
  static uint16_t sTrackTableSize;  /* hold the size during the construction */
  static Track ** sTracks;          /* Table of track pointers */
  static bool sTrackNetOk;   /* True after verifying no NULL pointer remains */

protected:
  void setDirection(const uint8_t inDir); /* Set the travelling direction */
  virtual void connectFrom(
    const Track * inTrack,
    const uint8_t inConnector
  ) = 0;
  virtual bool allPathsTo(
    const uint16_t inId,
    const uint8_t inDir,
    PathSet &ioPaths,
    const Track * inFrom,
    HeadedTrackSet &inMarking
  ) = 0;

public:
  Track(N_);
  uint8_t direction() { return mDirection; }
  uint16_t identifier() { return mIdentifier; }
  bool pathTo(Track & inTrack, uint8_t inDir, PathSet & ioPaths);
  bool pathTo(uint16_t inId, uint8_t inDir, PathSet & ioPaths);

  static const uint16_t count() { return sCount; }
  static const uint8_t sizeForSet()
  {
    return ((sNombre >> 3) + ((sNombre & 7) != 0));
  }

  static void finalize();
  static Track & trackForId(uint16_t inId);
  static bool checkTrackNet();
  static bool trackNetIsOk() { return sTrackNetOk; }
};

/*
 * Dead-end track
 */
class DeadEndTrack : public Track
{
private:
  Track * mOutTrack;  /* OUTLET track */
  uint16_t mLength;   /* length of the dead-end track */

protected:
  virtual void connectFrom(
    const Track * inTrack,
    const uint8_t inConnector
  );
  virtual bool allPathsTo(
    const uint16_t inId,
    const uint8_t inDir,
    PathSet &ioPaths,
    const Track * inFrom,
    HeadedTrackSet &inMarking
  );

public:
  DeadEndTrack(N__ uint16_t inLength = 0u);
  void connectTo(const Track & inTrack, const uint8_t inConnector);
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
  virtual void connectFrom(
    const Track * inTrack,
    const uint8_t inConnector
  );
  virtual bool allPathsTo(
    const uint16_t inId,
    const uint8_t inDir,
    PathSet &ioPaths,
    const Track * inFrom,
    HeadedTrackSet &inMarking
  );

public:
  NormalTrack(N__ uint16_t inLength = 0u);
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
  virtual void connectFrom(
    const Track * inTrack,
    const uint8_t inConnector
  );
  virtual bool allPathsTo(
    const uint16_t inId,
    const uint8_t inDir,
    PathSet &ioPaths,
    const Track * inFrom,
    HeadedTrackSet &inMarking
  );

public:
  TurnoutTrack(N__ uint16_t inLeftLength = 0u, uint16_t inRightLength = 0u);
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

  virtual void connectFrom(
    const Track * inTrack,
    const uint8_t inConnector
  );
  virtual bool allPathsTo(
    const uint16_t inId,
    const uint8_t inDir,
    PathSet &ioPaths,
    const Track * inFrom,
    HeadedTrackSet &inMarking
  );

public:
  CrossingTrack(
    N__
    uint16_t inLeftToRightLength = 0u,
    uint16_t inRightToLeftLength = 0u
  );
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
  virtual bool allPathsTo(
    const uint16_t inId,
    const uint8_t inDir,
    PathSet &ioPaths,
    const Track * inFrom,
    HeadedTrackSet &inMarking
  );

public:
  DoubleSlipTrack(
    N__
    uint16_t inLeftToRightLength = 0u,
    uint16_t inRightToLeftLength = 0u
  );
};

#endif /* __TRACK_H__ */
