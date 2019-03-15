#include "Track.h"
#include "PathSet.h"
#include "HeadedTrackSet.h"

#ifdef DEBUG
/*
 * Error messages
 */
const char connectorErrorMsg[] PROGMEM =  "Bad connector: ";
const char usedConnectorMsg[] PROGMEM =   "Connector already in use: ";

#endif

#if defined(DEBUG) || defined(TRACE)

void displayConnectorName(const Connector inConnector)
{
  switch (inConnector) {
    case INLET:         Serial.print(F("INLET"));                   break;
    case LEFT_INLET:    Serial.print(F("LEFT_INLET"));              break;
    case RIGHT_INLET:   Serial.print(F("RIGHT_INLET"));             break;
    case OUTLET:        Serial.print(F("OUTLET"));                  break;
    case LEFT_OUTLET:   Serial.print(F("LEFT_OUTLET"));             break;
    case RIGHT_OUTLET:  Serial.print(F("RIGHT_OUTLET"));            break;
    default:            Serial.print(F("** Unknown Connector **")); break;
  }
}

void displayConnectorNameln(const Connector inConnector)
{
  displayConnectorName(inConnector);
  Serial.println();
}

void displayDirection(const Direction inDir)
{
  switch (inDir) {
    case FORWARD_DIRECTION:   Serial.print('F');  break;
    case BACKWARD_DIRECTION:  Serial.print('B');  break;
    case NO_DIRECTION:        Serial.print('*');  break;
    default:                  Serial.print('?');  break;
  }
}

void displayTrack(const uint16_t inId)
{
  Track::trackForId(inId).print();
}

void displayTrack(const Track * inTrack)
{
  inTrack->print();
}

void displayTrack(const Track & inTrack)
{
  inTrack.print();
}

void displayTrackln(const uint16_t inId)
{
  displayTrack(inId);
  Serial.println();
}

void displayTrackln(const Track * inTrack)
{
  displayTrack(inTrack);
  Serial.println();
}

void displayTrackln(const Track & inTrack)
{
  displayTrack(inTrack);
  Serial.println();
}

void displayTrackConnector(const Track * inTrack, const Connector inConnector)
{
  Serial.print('[');
  displayTrack(inTrack);
  Serial.print(']');
  Serial.print('(');
  displayConnectorName(inConnector);
  Serial.println(')');
}

static uint16_t gDepth = 0;

void depthDisplayTrack(const Track * inTrack)
{
  uint16_t i = gDepth << 2;
  while (i--) Serial.print(' ');
  displayTrackln(inTrack);
}

#define ddTrack(track) depthDisplayTrack(track)
#define incDepth() gDepth++
#define decDepth() gDepth--

#else

#define ddTrack(track)
#define incDepth()
#define decDepth()

#endif

/* Macros to desplay errors in Debug mode */
#ifdef DEBUG

#define BAD_CONNECTOR_ERROR(message,obj,connector) \
Serial.print(F(message)); \
Serial.print(connectorErrorMsg); \
displayTrackConnector(obj,connector);
#define USED_CONNECTOR_ERROR(message,obj,connector) \
Serial.print(F(message)); \
Serial.print(usedConnectorMsg); \
displayTrackConnector(obj,connector);

#else

#define BAD_CONNECTOR_ERROR(message,connector)
#define USED_CONNECTOR_ERROR(message,obj,connector)

#endif

#define ensureTrackNetOk() if (! trackNetIsOk()) return false

/*=============================================================================
 * Track
 */
uint16_t Track::sCount = 0;
uint16_t Track::sTrackTableSize = 16;
Track **Track::sTracks = NULL;
uint16_t Track::sErrorCount = 0;

/*---------------------------------------------------------------------------*/
Track::Track(NAME_DECL_FIRST(inName) const uint16_t inId) :
#ifdef DEBUG
  mName(inName),
#endif
  mIdentifier(inId),
  mDirection(NO_DIRECTION)
{
  if (sTracks == NULL) {
    sTracks = (Track **)malloc(sTrackTableSize * sizeof(Track **));
  }

  sCount++;

  if (mIdentifier >= sTrackTableSize) {
    sTrackTableSize = sTrackTableSize * 2;
    sTracks = (Track **)realloc(sTracks, sTrackTableSize * sizeof(Track **));
  }

  sTracks[mIdentifier] = this;
}

/*---------------------------------------------------------------------------*/
void Track::finalize()
{
  if (sTrackTableSize > sCount) {
    sTrackTableSize = sCount;
    sTracks = (Track **)realloc(sTracks, sTrackTableSize * sizeof(Track **));
  }
  /* Check the track net */
  for (uint16_t t = 0; t < sTrackTableSize; t++) {
    if (! sTracks[t]->connectionsOk()) incErrorCount();
  }
}

/*---------------------------------------------------------------------------*/
void Track::setDirection(const Direction inDir)
{
  if (mDirection == NO_DIRECTION || mDirection == inDir) mDirection = inDir;
#ifdef DEBUG
  else {
    Serial.print(F("Direction already set: "));
    displayTrackln(this);
  }
#endif
}

/*---------------------------------------------------------------------------*/
bool Track::pathsTo(Track & inTrack, const Direction inDir, PathSet & ioPaths)
{
#ifdef TRACE
  gDepth = 0;
#endif
  HeadedTrackSet marking;
  return allPathsTo(inTrack.identifier(), inDir, ioPaths, NULL, marking);
}

/*---------------------------------------------------------------------------*/
bool Track::pathsTo(uint16_t inId, const Direction inDir, PathSet & ioPaths)
{
#ifdef TRACE
  gDepth = 0;
#endif
  HeadedTrackSet marking;
  return allPathsTo(inId, inDir, ioPaths, NULL, marking);
}

/*---------------------------------------------------------------------------*/
Track & Track::trackForId(uint16_t inId)
{
  return *(sTracks[inId]);
}

#ifdef DEBUG
void Track::print() const
{
  char buf[32];
  strcpy_P(buf, (char *)pgm_read_word(mName));
  Serial.print(buf);
}

void Track::println() const
{
  print();
  Serial.println();
}
#endif

/*=============================================================================
 * Dead-end track
 */
DeadendTrack::DeadendTrack(NAME_DECL_FIRST(inName) const uint16_t inId) :
  Track(NAME_ARG_FIRST(inName) inId), mOutTrack(NULL)
{}

/*---------------------------------------------------------------------------*/
ErrorCode DeadendTrack::connect(
  const Connector inFromConnector,
  Track & inToTrack,
  const Connector inToConnector)
{
  ErrorCode result = NO_ERROR;

  if (mOutTrack == NULL) {
    if (inFromConnector == OUTLET) {
      mOutTrack = &inToTrack;
      mOutTrack->connectFrom(this, inToConnector);
      setDirection(FORWARD_DIRECTION);
    }
    else {
      incErrorCount();
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("DeadendTrack::connect/", this, inFromConnector);
    }
  }
  else {
    incErrorCount();
    result = USED_CONNECTOR;
    USED_CONNECTOR_ERROR("DeadendTrack::connect/", this, OUTLET);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
ErrorCode DeadendTrack::connectFrom(Track * inTrack, const Connector inConnector)
{
  ErrorCode result = NO_ERROR;

  if (inConnector == OUTLET) {
    if (mOutTrack == NULL) {
      mOutTrack = inTrack;
      setDirection(BACKWARD_DIRECTION);
    }
    else {
      incErrorCount();
      result = USED_CONNECTOR;
      USED_CONNECTOR_ERROR("DeadEndTrack::connectFrom/", this, inConnector);
    }
  }
  else {
    incErrorCount();
    result = BAD_CONNECTOR;
    BAD_CONNECTOR_ERROR("DeadEndTrack::connectFrom/", this, inConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
bool DeadendTrack::connectionsOk()
{
  return mOutTrack != NULL;
}

/*---------------------------------------------------------------------------*/
bool DeadendTrack::allPathsTo(
  const uint16_t inId,                          /* id of the target track     */
  const Direction inDir,                        /* travel direction           */
  PathSet & ioPaths,                            /* found paths                */
  __attribute__((unused)) const Track * inFrom, /* track used to get there    */
  HeadedTrackSet & ioMarking)                   /* to mark the visited tracks */
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (! ioMarking.containsTrack(this,inDir))
  {
    ioMarking.addTrack(this, inDir);
    if (inId == identifier()) { /* found */
      ioPaths.addTrack(this);
      result = true;
    }
    else {
      if (direction() == FORWARD_DIRECTION && direction() == inDir) {
        if (mOutTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
    }
  }
  decDepth();
  return result;
}

/*=============================================================================
 * Block
 */
BlockTrack::BlockTrack(NAME_DECL_FIRST(inName) const uint16_t inId) :
  Track(NAME_ARG_FIRST(inName) inId), mInTrack(NULL), mOutTrack(NULL)
{}

/*---------------------------------------------------------------------------*/
ErrorCode BlockTrack::connect(
  const Connector inFromConnector,
  Track &         inToTrack,
  const Connector inToConnector)
{
  ErrorCode result = NO_ERROR;

  switch (inFromConnector) {

    case INLET:
      if (mInTrack == NULL) {
        mInTrack = &inToTrack;
        mInTrack->connectFrom(this, inToConnector);
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case OUTLET:
      if (mOutTrack == NULL) {
        mOutTrack = &inToTrack;
        mOutTrack->connectFrom(this, inToConnector);
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_INLET:
    case LEFT_OUTLET:
    case RIGHT_INLET:
    case RIGHT_OUTLET:
    default:
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("BlockTrack::connect/", this, inFromConnector);
      break;
  }
  if (result == USED_CONNECTOR) {
    incErrorCount();
    USED_CONNECTOR_ERROR("BlockTrack::connect", this, inFromConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
ErrorCode BlockTrack::connectFrom(
  Track * inTrack,
  const Connector inConnector)
{
  ErrorCode result = NO_ERROR;

  switch (inConnector) {

    case OUTLET:
      if (mOutTrack == NULL) {
        mOutTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case INLET:
      if (mInTrack == NULL) {
        mInTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_INLET:
    case LEFT_OUTLET:
    case RIGHT_INLET:
    case RIGHT_OUTLET:
    default:
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("BlockTrack::connectFrom", this, inConnector);
      break;
  }
  if (result == USED_CONNECTOR) {
    incErrorCount();
    USED_CONNECTOR_ERROR("BlockTrack::connectFrom", this, inConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
bool BlockTrack::connectionsOk()
{
  return mInTrack != NULL && mOutTrack != NULL;
}

/*---------------------------------------------------------------------------*/
bool BlockTrack::allPathsTo(
  const uint16_t inId,                          /* id of the target track     */
  const Direction inDir,                        /* travel direction           */
  PathSet & ioPaths,                            /* found paths                */
  __attribute__((unused)) const Track * inFrom, /* track used to get there    */
  HeadedTrackSet & ioMarking)                   /* to mark the visited tracks */
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (! ioMarking.containsTrack(this,inDir)) {
    ioMarking.addTrack(this, inDir);
    if (inId == identifier()) { /* found */
      ioPaths.addTrack(this);
      result = true;
    }
    else {
      if (direction() == inDir) {
        if (mOutTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
      else {
        if (mInTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
    }
  }
  decDepth();
  return result;
}

/*=============================================================================
 * Turnout track
 */
TurnoutTrack::TurnoutTrack(NAME_DECL_FIRST(inName) const uint16_t inId) :
  Track(NAME_ARG_FIRST(inName) inId),
  mInTrack(NULL),
  mOutLeftTrack(NULL),
  mOutRightTrack(NULL),
  mPartialPath(NULL),
  mPosition(NO_POSITION)
{}

/*---------------------------------------------------------------------------*/
ErrorCode TurnoutTrack::connect(
  const Connector inFromConnector,
  Track &         inToTrack,
  const Connector inToConnector)
{
  ErrorCode result = NO_ERROR;

  switch (inFromConnector) {

    case INLET:
      if (mInTrack == NULL) {
        mInTrack = &inToTrack;
        mInTrack->connectFrom(this, inToConnector);
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_OUTLET:
      if (mOutLeftTrack == NULL) {
        mOutLeftTrack = &inToTrack;
        mOutLeftTrack->connectFrom(this, inToConnector);
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case RIGHT_OUTLET:
      if (mOutRightTrack == NULL) {
        mOutRightTrack = &inToTrack;
        mOutRightTrack->connectFrom(this, inToConnector);
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_INLET:
    case RIGHT_INLET:
    case OUTLET:
    default:
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("TurnoutTrack::connect/", this, inFromConnector);
      break;
  }
  if (result == USED_CONNECTOR) {
    incErrorCount();
    USED_CONNECTOR_ERROR("TurnoutTrack::connect", this, inFromConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
ErrorCode TurnoutTrack::connectFrom(Track * inTrack, const Connector inConnector)
{
  ErrorCode result = NO_ERROR;

  switch (inConnector) {

    case INLET:
      if (mInTrack == NULL) {
        mInTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_OUTLET:
      if (mOutLeftTrack == NULL) {
        mOutLeftTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case RIGHT_OUTLET:
      if (mOutRightTrack == NULL) {
        mOutRightTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_INLET:
    case RIGHT_INLET:
    case OUTLET:
    default:
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("TurnoutTrack::connectFrom/", this, inConnector);
      break;
  }
  if (result == USED_CONNECTOR) {
    incErrorCount();
    USED_CONNECTOR_ERROR("TurnoutTrack::connectFrom", this, inConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
bool TurnoutTrack::connectionsOk()
{
  return mInTrack != NULL && mOutLeftTrack != NULL && mOutRightTrack != NULL;
}

/*---------------------------------------------------------------------------*/
bool TurnoutTrack::allPathsTo(
  const uint16_t inId,
  const Direction inDir,
  PathSet & ioPaths,
  __attribute__((unused)) const Track * inFrom,
  HeadedTrackSet & ioMarking)
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (ioMarking.containsTrack(this,inDir)) {
    /*
     * track already looked up.
     * 1) a partial path has been left: a path to the target exists and hads
     *    been explored
     * 2) no partial path: no path to the target
     */
    if (mPartialPath != NULL) {
#ifdef TRACE
      Serial.println(F("$$$$ Partial path"));
      mPartialPath->println();
#endif
      ioPaths = *mPartialPath;
      result = true;
    }
    else result = false;
  }
  else {
    ioMarking.addTrack(this, inDir);
    /*
     * remove the partiel path left by a previous
     * exploration if direction is out to in.
     */
    if (direction() != inDir && mPartialPath != NULL) {
      delete mPartialPath;
      mPartialPath = NULL;
    }

    if (identifier() == inId) { /* found */
      ioPaths.addTrack(this);
      result = true;
    }
    else {
      if (direction() == inDir) { /* travelling from in to out */
        PathSet rightPaths(ioPaths);
        if (mOutLeftTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          if (mOutRightTrack->allPathsTo(inId, inDir, rightPaths, this, ioMarking)) {
            /* Both paths lead to the target, merge the paths */
            ioPaths += rightPaths;
          }
          ioPaths.addTrack(this);
          result = true;
        }
        else {
          /* no path on the left, try on right */
          if (mOutRightTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
            ioPaths.addTrack(this);
            result = true;
          }
        }
      }
      else { /* travelling from out to in */
        if (mInTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          mPartialPath = new PathSet(ioPaths);
          result = true;
        }
      }
    }
  }
  decDepth();
  return result;
}

/*---------------------------------------------------------------------------*/
void TurnoutTrack::setPosition(const Position inPosition)
{
  mPosition = inPosition;
}

/*=============================================================================
 * Crossing track
 */
CrossingTrack::CrossingTrack(NAME_DECL_FIRST(inName) const uint16_t inId) :
  Track(NAME_ARG_FIRST(inName) inId),
  mInLeftTrack(NULL),
  mInRightTrack(NULL),
  mOutLeftTrack(NULL),
  mOutRightTrack(NULL)
{}

/*---------------------------------------------------------------------------*/
ErrorCode CrossingTrack::connect(
  const Connector inFromConnector,
  Track &         inToTrack,
  const Connector inToConnector)
{
  ErrorCode result = NO_ERROR;

  switch (inFromConnector) {
    case LEFT_INLET:
      if (mInLeftTrack == NULL) {
        mInLeftTrack = &inToTrack;
        mInLeftTrack->connectFrom(this, inToConnector);
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case RIGHT_INLET:
      if (mInRightTrack == NULL) {
        mInRightTrack = &inToTrack;
        mInRightTrack->connectFrom(this, inToConnector);
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_OUTLET:
      if (mOutLeftTrack == NULL) {
        mOutLeftTrack = &inToTrack;
        mOutLeftTrack->connectFrom(this, inToConnector);
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case RIGHT_OUTLET:
      if (mOutRightTrack == NULL) {
        mOutRightTrack = &inToTrack;
        mOutRightTrack->connectFrom(this, inToConnector);
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case INLET:
    case OUTLET:
    default:
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("CrossingTrack::connect", this, inFromConnector);
      break;
  }
  if (result == USED_CONNECTOR) {
    incErrorCount();
    USED_CONNECTOR_ERROR("CrossingTrack::connect", this, inFromConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
ErrorCode CrossingTrack::connectFrom(
  Track * inTrack,
  const Connector inConnector)
{
  ErrorCode result = NO_ERROR;

  switch (inConnector) {

    case LEFT_INLET:
      if (mInLeftTrack == NULL) {
        mInLeftTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case RIGHT_INLET:
      if (mInRightTrack == NULL) {
        mInRightTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case LEFT_OUTLET:
      if (mOutLeftTrack == NULL) {
        mOutLeftTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case RIGHT_OUTLET:
      if (mOutRightTrack == NULL) {
        mOutRightTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
      else result = USED_CONNECTOR;
      break;

    case INLET:
    case OUTLET:
    default:
      result = BAD_CONNECTOR;
      BAD_CONNECTOR_ERROR("CrossingTrack::connectFrom", this, inConnector);
      break;
  }
  if (result == USED_CONNECTOR) {
    incErrorCount();
    USED_CONNECTOR_ERROR("CrossingTrack::connectFrom", this, inConnector);
  }
  return result;
}

/*---------------------------------------------------------------------------*/
bool CrossingTrack::connectionsOk()
{
  return mInLeftTrack != NULL &&
         mInRightTrack != NULL &&
         mOutLeftTrack != NULL &&
         mOutRightTrack != NULL;
}

/*---------------------------------------------------------------------------*/
bool CrossingTrack::allPathsTo(
  const uint16_t inId,
  const Direction inDir,
  PathSet & ioPaths,
  const Track * inFrom,
  HeadedTrackSet & ioMarking)
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (identifier() == inId) {
    ioPaths.addTrack(this);
    result = true;
  }
  else {
    if (direction() == inDir) {
      if (inFrom == mInLeftTrack) {
        if (mOutRightTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
      else if (inFrom == mInRightTrack) {
        if (mOutLeftTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
    }
    else { /* direction != inDir */
      if (inFrom == mOutLeftTrack) {
        if (mInRightTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
      else if (inFrom == mOutRightTrack) {
        if (mInLeftTrack->allPathsTo(inId, inDir, ioPaths, this, ioMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
    }
  }
  decDepth();
  return result;
}

/*=============================================================================
 * DoubleslipTrack
 */
// DoubleslipTrack::DoubleslipTrack(NAME_DECL_FIRST(inName) const uint16_t inId) :
//   CrossingTrack(NAME_ARG_FIRST(inName) inId),
//   mInPosition(NO_POSITION),
//   mOutPosition(NO_POSITION)
// {
// }
//
// /*---------------------------------------------------------------------------*/
// void DoubleslipTrack::setInPosition(const Position inPosition)
// {
//   mInPosition = inPosition;
// }
//
// /*---------------------------------------------------------------------------*/
// void DoubleslipTrack::setOutPosition(const Position inPosition)
// {
//   mOutPosition = inPosition;
// }
