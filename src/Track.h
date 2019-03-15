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

/*
 * Positions of turnout, single and double slip
 */
typedef enum {
  NO_POSITION,
  LEFT_POSITION,
  RIGHT_POSITION
} Position;

typedef enum {
  NO_ERROR,
  BAD_CONNECTOR,
  USED_CONNECTOR
} ErrorCode;

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

/*-----------------------------------------------------------------------------
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

  static uint16_t sCount;          /* Declared track count                  */
  static uint16_t sTrackTableSize; /* hold the size during the construction */
  static Track **sTracks;          /* Table of track pointers               */
  static uint16_t sErrorCount;     /* Number of errors during connections
                                      and during finalize                   */

protected:
  void setDirection(const Direction inDir); /* Set the travelling direction */
  static void incErrorCount() { sErrorCount++; }

public:
  /* return true if the track is a block */
  virtual bool isBlock() { return false; }
  /* Connect to a connector of this track from a track */
  virtual ErrorCode connectFrom(Track * inTrack, const Connector inConnector) = 0;
  /* Build paths from this track to track having inId */
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  ) = 0;
  /* Constructor */
  Track(NAME_DECL_FIRST(inName) const uint16_t inId);
  /* Get the connection direction of the track */
  Direction direction() const { return mDirection; }
  /* Get the identifier of the track */
  uint16_t identifier() const { return mIdentifier; }
  /* Connect a track connector to a connector of this track */
  virtual ErrorCode connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  ) = 0;
  virtual bool connectionsOk() = 0;
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
  static bool trackNetIsOk() { return sErrorCount == 0; }
};

/*
 * Dead-end track
 */
class DeadendTrack : public Track
{
private:
  Track * mOutTrack;  /* OUTLET connector */

public:
  virtual ErrorCode connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  DeadendTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual ErrorCode connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
  virtual bool connectionsOk();
};

/*
 * Normal track
 */
class BlockTrack : public Track
{
private:
  Track * mInTrack;   /* INLET connector  */
  Track * mOutTrack;  /* OUTLET connector */

public:
  virtual bool isBlock() { return true; }
  virtual ErrorCode connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  BlockTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual ErrorCode connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
  virtual bool connectionsOk();
};

/*
 * Turnout track
 */
class TurnoutTrack : public Track
{
private:
  Track * mInTrack;       /* INLET connector        */
  Track * mOutLeftTrack;  /* LEFT_OUTLET connector  */
  Track * mOutRightTrack; /* RIGHT_OUTLET connector */
  PathSet * mPartialPath;
  Position mPosition;    /* The position of the Turnout */

public:
  virtual ErrorCode connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  TurnoutTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual ErrorCode connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
  virtual bool connectionsOk();

  void setPosition(const Position inPosition);
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
  virtual ErrorCode connectFrom(Track * inTrack, const Connector inConnector);
  virtual bool allPathsTo(
    const uint16_t inId,
    const Direction inDir,
    PathSet & ioPaths,
    const Track * inFrom,
    HeadedTrackSet & ioMarking
  );

  CrossingTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
  virtual ErrorCode connect(
    const Connector inFromConnector,
    Track & inToTrack,
    const Connector inToConnector
  );
  virtual bool connectionsOk();
};

/*
 * Double slip track
 */
// class DoubleslipTrack : public CrossingTrack
// {
// private:
//   PathSet * mPartialPath[2];
//   Position mInPosition:3;     /* The position of on the In side  */
//   Position mOutPosition:3;    /* The position of on the Out side */
//
// public:
//   virtual bool allPathsTo(
//     const uint16_t inId,
//     const Direction inDir,
//     PathSet & ioPaths,
//     const Track * inFrom,
//     HeadedTrackSet & ioMarking
//   );
//
//   DoubleslipTrack(NAME_DECL_FIRST(inName) const uint16_t inId);
//
//   void setInPosition(const Position inPosition);
//   void setOutPosition(const Position outPosition);
// };

#endif /* __TRACK_H__ */
