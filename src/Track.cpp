#include "Track.h"

#ifdef DEBUG
/*
 * Error messages
 */
const char connectorErrorMsg[] PROGMEM =  "Bad connector: ";
const char usedConnectorMsg[] PROGMEM =   "Connector already in use: ";

#endif

#if defined(DEBUG) || defined(TRACE)

void displayConnectorName(
  const uint8_t inConnector,
  const bool inNewLine = false)
{
  switch (inConnector) {
    case INLET:         Serial.print(F("INLET"));                 break;
    case LEFT_INLET:    Serial.print(F("LEFT_INLET"));            break;
    case RIGHT_INLET:   Serial.print(F("RIGHT_INLET"));           break;
    case OUTLET:        Serial.print(F("OUTLET"));                break;
    case LEFT_OUTLET:   Serial.print(F("LEFT_OUTLET"));           break;
    case RIGHT_OUTLET:  Serial.print(F("RIGHT_OUTLET"));          break;
    default:              Serial.print(F("** Unknown Connector **")); break;
  }
  if (inNewLine) Serial.println();
}

void displayDirection(uint8_t inDir)
{
  switch (inDir) {
    case FORWARD_DIRECTION:   Serial.print('F');  break;
    case BACKWARD_DIRECTION:  Serial.print('B');  break;
    case NO_DIRECTION:        Serial.print('*');  break;
    default:                  Serial.print('?');  break;
  }
}

void displayTrack(
  const Track * inTrack,
  const bool inNewLine = false) __attribute__((weak))
{
  Serial.print(inTrack->identifier());
  if (inNewLine) Serial.println();
}

void displayTrackConnector(const Track * inTrack, const uint8_t inConnector)
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
  displayTrack(inTrack, true);
}

#define ddTrack(track) depthDisplayTrack(track)
#define incDepth() gDepth++
#define decDepth() gDepth--

#else

#define ddTrack(track)
#define incDepth()
#define decDepth()

#endif

#define ensureTrackNetOk() if (! trackNetIsOk()) return false

/*=============================================================================
 * Track
 */
uint16_t Track::sCount = 0;
uint16_t Track::sTrackTableSize = 16;
Track ** Track::sTracks = NULL;
bool Track::sTrackNetOk = false;

/*---------------------------------------------------------------------------*/
Track::Track(NAME_DECL_ALONE)
#ifdef DEBUG
  : mName(inName)
#endif
{
  if (sTracks == NULL) {
    sTracks = (Track **)malloc(sTrackTableSize * sizeof(Track **));
  }

  mIdentifier = sCount++;
  mDirection = NO_DIRECTION;

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
}

/*---------------------------------------------------------------------------*/
void Track::setDirection(uint8_t inDir)
{
  if (mDirection == NO_DIRECTION || mDirection == inDir) mDirection = inDir;
#ifdef DEBUG
  else {
    Serial.print(F("Direction already set: "));
    displayTrack(this, true);
  }
#endif
}

/*---------------------------------------------------------------------------*/
bool Track::pathTo(Track & inTrack, uint8_t inDir, PathSet & ioPaths)
{
#if TRACE
  gDepth = 0;
#endif
  HeadedTrackSet marking;
  return allPathsTo(inTrack.identifier(), inDir, ioPaths, NULL, marking);
}

/*---------------------------------------------------------------------------*/
bool Track::pathTo(uint16_t inId, uint8_t inDir, PathSet & ioPaths)
{
#if TRACE
  gDepth = 0;
#endif
  HeadedTrackSet marking;
  return allPathsTo(inId, inDir, ioPaths, NULL, marking);
}

/*=============================================================================
 * Dead-end track
 */
DeadEndTrack::DeadEndTrack(NAME_DECL_FIRST uint16_t inLength) :
  Track(NAME_ARG), mOutTrack(NULL), mLength(0)
{}

/*---------------------------------------------------------------------------*/
void DeadEndTrack::connectTo(const Track & inTrack, const uint8_t inConnector)
{
  if (mOutTrack == NULL) {
    mOutTrack = &inTrack;
    mOutTrack->connectFrom(this, inConnector);
    setDirection(FORWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("DeadEndTrack::connectTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, OUTLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void DeadEndTrack::connectFrom(const Track * inTrack, const uint8_t inConnector)
{
  if (inConnector == OUTLET) {
    if (mOutTrack == NULL) {
      mOutTrack = inTrack;
      setDirection(BACKWARD_DIRECTION);
    }
#ifdef DEBUG
    else {
      Serial.print(F("DeadEndTrack::connectFrom/"));
      Serial.print(usedConnectorMsg);
      displayTrackConnector(this, inConnector);
    }
#endif
  }
#ifdef DEBUG
  else {
    Serial.print(F("DeadEndTrack::connectFrom/"));
    Serial.print(connectorErrorMsg);
    displayTrackConnector(this, inConnector);
  }
#endif
}

/*---------------------------------------------------------------------------*/
bool DeadEndTrack::allPathsTo(
  const uint16_t inId,        /* id of the target track     */
  const uint8_t inDir,        /* travel direction           */
  PathSet &ioPaths,           /* found paths                */
  const Track * inFrom,       /* track used to get there    */
  HeadedTrackSet &inMarking)  /* to mark the visited tracks */
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (! marking.containsTrack(this,inDir))
  {
    marking.addTrack(this, inDir);
    if (inId == identifier()) { /* found */
      ioPaths.addTrack(this);
      result = true;
    }
    else {
      if (direction() == FORWARD_DIRECTION && direction() == inDir) {
        if (mOutTrack->allPathsTo(inId, inDir, ioPaths, this, marking)) {
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
 * Normal track
 */
NormalTrack::NormalTrack(NAME_DECL_FIRST uint16_t inLength) :
  Track(NAME_ARG), mInTrack(NULL), mOutTrack(NULL), mLength(inLength)
{}

/*---------------------------------------------------------------------------*/
void NormalTrack::connectInTo(Track & inTrack, uint8_t inConnector)
{
  if (mInTrack == NULL) {
    mInTrack = &inTrack;
    mInTrack->connectFrom(this, inConnector);
    setDirection(BACKWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("NormalTrack::connectInTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, INLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void NormalTrack::connectOutTo(Track & inTrack, uint8_t inConnector)
{
  if (mOutTrack == NULL) {
    mOutTrack = &inTrack;
    mOutTrack->connectFrom(this, inConnector);
    setDirection(FORWARD_DIRECTION);
  }
  #ifdef DEBUG
    else {
      Serial.print(F("NormalTrack::connectOutTo/"));
      Serial.print(usedConnectorMsg);
      displayTrackConnector(this, OUTLET);
    }
  #endif
}

/*---------------------------------------------------------------------------*/
void NormalTrack::connectFrom(const Track * inTrack, const uint8_t inConnector)
{
#ifdef DEBUG
  bool error = false;
#endif

  switch (inConnector) {

    case OUTLET:
      if (mOutTrack == NULL) {
        mOutTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break:

    case INLET:
      if (mInTrack == NULL) {
        mInTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

#ifdef DEBUG
    default:
      Serial.print(F("NormalTrack::connectFrom/"));
      Serial.print(connectorErrorMsg);
      displayTrackConnector(this, inConnector);
      break;
#endif
  }
#ifdef DEBUG
  if (error) {
    Serial.print(F("NormalTrack::connectFrom/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, inConnector);
  }
#endif
}

/*---------------------------------------------------------------------------*/
bool NormalTrack::allPathsTo(
  const uint16_t inId,        /* id of the target track     */
  const uint8_t inDir,        /* travel direction           */
  PathSet &ioPaths,           /* found paths                */
  const Track * inFrom,       /* track used to get there    */
  HeadedTrackSet &inMarking)  /* to mark the visited tracks */
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (! marking.containsTrack(this,inDir)) {
    marking.addTrack(this, inDir);
    if (inId == identifier()) { /* found */
      ioPaths.addTrack(this);
      result = true;
    }
    else {
      if (direction() == inDir) {
        if (mOutTrack->allPathsTo(inId, inDir, ioPaths, this, inMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
      else {
        if (mInTrack->allPathsTo(inId, inDir, ioPaths, this, inMarking)) {
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
TurnoutTrack::TurnoutTrack(NAME_DECL_FIRST uint16_t inLeftLength, uint16_t inRightLength) :
  Track(NAME_ARG), mInTrack(NULL), mOutLeftTrack(NULL), mOutRightTrack(NULL),
  mLeftLength(inLeftLength), mRightLength(inRightLength), mPartialPath(NULL)
{}

/*---------------------------------------------------------------------------*/
void TurnoutTrack::connectInTo(Track & inTrack, uint8_t inConnector)
{
  if (mInTrack == NULL) {
    mInTrack = &inTrack;
    mInTrack->connectFrom(this, inConnector);
    setDirection(BACKWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("TurnoutTrack::connectInTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, INLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void TurnoutTrack::connectOutLeftTo(Track & inTrack, uint8_t inConnector)
{
  if (mOutLeftTrack == NULL) {
    mOutLeftTrack = &inTrack;
    mOutLeftTrack->connectFrom(this, inConnector);
    setDirection(FORWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("TurnoutTrack::connectOutLeftTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, LEFT_OUTLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void TurnoutTrack::connectOutRightTo(Track & inTrack, uint8_t inConnector)
{
  if (mOutRightTrack == NULL) {
    mOutRightTrack = &inTrack;
    mOutRightTrack->connectFrom(this, inConnector);
    setDirection(FORWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("TurnoutTrack::connectOutRightTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, RIGHT_OUTLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void TurnoutTrack::connectFrom(const Track * inTrack, const uint8_t inConnector)
{
  #ifdef DEBUG
    bool error = false;
  #endif

  switch (inConnector) {

    case INLET:
      if (mInTrack == NULL) {
        mInTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

    case LEFT_OUTLET:
      if (mOutLeftTrack == NULL) {
        mOutLeftTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

    case RIGHT_OUTLET:
      if (mOutRightTrack == NULL) {
        mOutRightTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

#ifdef DEBUG
    default:
      Serial.print(F("TurnoutTrack::connectFrom/"));
      Serial.print(connectorErrorMsg);
      displayTrackConnector(this, inConnector);
      break;
#endif
  }
#ifdef DEBUG
  if (error) {
    Serial.print(F("TurnoutTrack::connectFrom/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, inConnector);
  }
#endif
}

/*---------------------------------------------------------------------------*/
bool TurnoutTrack::allPathsTo(
  const uint16_t inId,
  const uint8_t inDir,
  PathSet &ioPaths,
  const Track * inFrom,
  HeadedTrackSet &inMarking)
{
  ensureTrackNetOk();
  ddTrack(this);
  incDepth();
  bool result = false;
  if (marking.containsTrack(this,inDir)) {
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
    marking.addTrack(this, inDir);
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
        if (mOutLeftTrack->allPathsTo(inId, inDir, ioPaths, this, marking)) {
          if (mOutRightTrack->allPathsTo(inId, inDir, rightPaths, this, marking)) {
            /* Both paths lead to the target, merge the paths */
            ioPaths += rightPaths;
          }
          ioPaths.addTrack(this);
          result = true;
        }
        else {
          /* no path on the left, try on right */
          if (mOutRightTrack->allPathsTo(inId, inDir, ioPaths, this, marking)) {
            ioPaths.addTrack(this);
            result = true;
          }
        }
      }
      else { /* travelling from out to in */
        if (mInTrack->allPathsTo(inId, inDir, ioPaths, this, marking)) {
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

/*=============================================================================
 * Crossing track
 */
CrossingTrack::CrossingTrack(
  NAME_DECL_FIRST
  uint16_t inLeftToRightLength,
  uint16_t inRightToLeftLength) :
  Track(NAME_ARG), mInLeftTrack(NULL), mInRightTrack(NULL), mOutLeftTrack(NULL),
  mOutRightTrack(NULL), mLeftToRightLength(inLeftToRightLength),
  mRightToLeftLength(inRightToLeftLength)
{}

/*---------------------------------------------------------------------------*/
void CrossingTrack::connectInLeftTo(Track & inTrack, uint8_t inConnector)
{
  if (mInLeftTrack == NULL) {
    mInLeftTrack = &inTrack;
    mInLeftTrack->connectFrom(this, inConnector);
    setDirection(BACKWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("CrossingTrack::connectInLeftTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, LEFT_INLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void CrossingTrack::connectInRightTo(Track & inTrack, uint8_t inConnector)
{
  if (mInRightTrack == NULL) {
    mInRightTrack = &inTrack;
    mInRightTrack->connectFrom(this, inConnector);
    setDirection(BACKWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("CrossingTrack::connectInRightTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, RIGHT_INLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void CrossingTrack::connectOutLeftTo(Track & inTrack, uint8_t inConnector)
{
  if (mOutLeftTrack == NULL) {
    mOutLeftTrack = &inTrack;
    mOutLeftTrack->connectFrom(this, inConnector);
    setDirection(FORWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("CrossingTrack::connectOutLeftTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, LEFT_OUTLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void CrossingTrack::connectOutRightTo(Track & inTrack, uint8_t inConnector)
{
  if (mOutRightTrack == NULL) {
    mOutRightTrack = &inTrack;
    mOutRightTrack->connectFrom(this, inConnector);
    setDirection(FORWARD_DIRECTION);
  }
#ifdef DEBUG
  else {
    Serial.print(F("CrossingTrack::connectOutRightTo/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, RIGHT_OUTLET);
  }
#endif
}

/*---------------------------------------------------------------------------*/
void CrossingTrack::connectFrom(
  const Track * inTrack,
  const uint8_t inConnector)
{
  #ifdef DEBUG
    bool error = false;
  #endif

  switch (inConnector) {

    case LEFT_INLET:
      if (mInLeftTrack == NULL) {
        mInLeftTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

    case RIGHT_INLET:
      if (mInRightTrack == NULL) {
        mInRightTrack = inTrack;
        setDirection(FORWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

    case LEFT_OUTLET:
      if (mOutLeftTrack == NULL) {
        mOutLeftTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

    case RIGHT_OUTLET:
      if (mOutRightTrack == NULL) {
        mOutRightTrack = inTrack;
        setDirection(BACKWARD_DIRECTION);
      }
#ifdef DEBUG
      else error = true;
#endif
      break;

#ifdef DEBUG
    default:
      Serial.print(F("CrossingTrack::connectFrom/"));
      Serial.print(connectorErrorMsg);
      displayTrackConnector(this, inConnector);
      break;
#endif
  }
#ifdef DEBUG
  if (error) {
    Serial.print(F("CrossingTrack::connectFrom/"));
    Serial.print(usedConnectorMsg);
    displayTrackConnector(this, inConnector);
  }
#endif
}

/*---------------------------------------------------------------------------*/
bool CrossingTrack::allPathsTo(
  const uint16_t inId,
  const uint8_t inDir,
  PathSet &ioPaths,
  const Track * inFrom,
  HeadedTrackSet &inMarking)
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
        if (mOutRightTrack->allPathsTo(inId, inDir, ioPaths, this, inMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
      else if (inFrom == mInRightTrack) {
        if (mOutLeftTrack->allPathsTo(inId, inDir, ioPaths, this, inMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
    }
    else { /* direction != inDir */
      if (inFrom == mOutLeftTrack) {
        if (mInRightTrack->allPathsTo(inId, inDir, ioPaths, this, inMarking)) {
          ioPaths.addTrack(this)
          result = true;
        }
      }
      else if (inFrom == mOutRightTrack) {
        if (mInLeftTrack->)allPathsTo(inId, inDir, ioPaths, this, inMarking)) {
          ioPaths.addTrack(this);
          result = true;
        }
      }
    }
  }
  decDepth();
  return result;
}
