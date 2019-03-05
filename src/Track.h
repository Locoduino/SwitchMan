#ifndef __TRACK_H__
#define __TRACK_H__

#include "Arduino.h"
#include "Debug.h"

/*
 * Direction of travel
 */
typedef enum {
  FORWARD_DIRECTION   = 0,
  BACKWARD_DIRECTION  = 1,
  NO_DIRECTION        = 2
} Direction;

/*
 * Connectors of a track element
 */
typedef enum {
  INLET,
  LEFT_INLET,
  RIGHT_INLET,
  OUTLET,
  LEFT_OUTLET,
  RIGHT_OUTLET
} Connector;

class PathSet;
class HeadedTrackSet;
class Track;

#ifdef DEBUG
void displayConnectorName(const Connector inConnector);
void displayConnectorNameln(const Connector inConnector);
void displayDirection(const Direction inDir);
void displayTrack(const uint16_t inId);
void displayTrackln(const uint16_t inId);
void displayTrack(const Track * inTrack);
void displayTrackln(const Track * inTrack);
void displayTrack(const Track & inTrack);
void displayTrackln(const Track & inTrack);
#endif

#ifdef DEBUG
#define NAME(name) F("name")
#define NAME_DECL_ALONE(name) const char * name
#define NAME_DECL_FIRST(name) const char * name,
#define NAME_ARG_FIRST(name) name,
#else
#define NAME(name)
#define NAME_DECL_ALONE(name)
#define NAME_DECL_FIRST(name)
#define NAME_ARG_FIRST(name)
#endif

/*
 * Abstract class for tracks
 */
class Track
{
private:
#ifdef DEBUG
  const char * mName;
#endif
  uint16_t mIdentifier : 14; /* Track identifier */
  Direction mDirection : 2;  /* Travel direction */

  static uint16_t sCount;           /* Declared track count */
  static uint16_t sTrackTableSize;  /* hold the size during the construction */
  static Track **sTracks;          /* Table of track pointers */
  static bool sTrackNetOk;   /* True after verifying no NULL pointer remains */

protected:
  void setDirection(const Direction inDir); /* Set the travelling direction */

public:
  virtual bool connectFrom(Track * inTrack, const Connector inConnector) = 0;
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  ) = 0;

  Track(NAME_DECL_FIRST(inName) const uint16_t inId);
  Direction direction() const { return mDirection; }
  uint16_t identifier() const { return mIdentifier; }
  virtual bool connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  ) = 0;
  bool pathsTo(Track & inTrack, const Direction inDir, PathSet & ioPaths);
  bool pathsTo(uint16_t inId, const Direction inDir, PathSet & ioPaths);

#ifdef DEBUG
  void print() const;
  void println() const;
#endif

  static uint16_t count() { return sCount; }
  static uint8_t sizeForSet()
  {
    return ((sCount >> 3) + ((sCount & 7) != 0));
  }

  static void finalize();
  static Track & trackForId(uint16_t inId);
  static bool checkTrackNet();
  static bool trackNetIsOk() { return sTrackNetOk; }
};

/*
 * Dead-end track
 */
class DeadendTrack : public Track
{
private:
  Track * mOutTrack;  /* OUTLET track */

public:
  virtual bool connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  DeadendTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual bool connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
};

/*
 * Normal track
 */
class BlockTrack : public Track
{
private:
  Track * mInTrack;
  Track * mOutTrack;

public:
  virtual bool connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  BlockTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual bool connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
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
  PathSet * mPartialPath;

public:
  virtual bool connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  TurnoutTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual bool connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
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

public:
  virtual bool connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  CrossingTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual bool connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
};

/*
 * Double slip track
 */
class DoubleslipTrack : public CrossingTrack
{
private:
  PathSet * mPartialPath[2];

public:
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  DoubleslipTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
};

#endif /* __TRACK_H__ */
