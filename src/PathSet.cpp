/*
 * PathSet : A Trackset list
 */
#include "PathSet.h"

bool Path::fitWith(const Path & inPath)
{
  uint8_t result = 0;
  for (uint8_t i = 0; i < Track::sizeForSet(); i++) {
    result |= mSet[i] & inPath.mSet[i]; /* Probably buggy */
  }
  return (result == 0);
}


void PathSet::clear()
{
  Path * p = mListHead;
  while (p != NULL) {
    Path *current = p;
    p = p->mNext;
    delete current;
  }
  mListHead = NULL;
}

void PathSet::copy(PathSet & inSet)
{
  mListHead = NULL;
  Path * p = inSet.mListHead;
  while (p != NULL) {
    Path * c = new Path(* p);
    c->mNext = mListHead;
    mListHead = c;
    p = p->mNext;
  }
}

/*
 * copy constructor
 */
PathSet::PathSet(PathSet & inSet)
{
  copy(inSet);
}

/*
 * Destructor clears the set
 */
PathSet::~PathSet()
{
  clear();
}

/*
 * Adds a track to all paths contained in the set
 */
void PathSet::addTrack(uint16_t inId)
{
#ifdef TRACE
  this->println();
  Serial.print("Adding ");
  Track::trackForId(inId).println();
#endif
  Path * p = mListHead;
  while (p != NULL) {
    p->addTrack(inId);
    p = p->mNext;
  }
}

bool PathSet::containsPath(Path & inPath)
{
  Path *p = mListHead;
  while (p != NULL) {
    if (*p == inPath) {
      return true;
    }
    p = p->mNext;
  }
  return false;
}

/*
 * Makes the union of two sets of paths
 */
PathSet & PathSet::operator+=(PathSet & inSet)
{
  Path *p = inSet.mListHead;
  while (p != NULL) {
    if (! containsPath(*p)) {
      Path *c = new Path(*p);
      c->mNext = mListHead;
      mListHead = c;
    }
    p = p->mNext;
  }
  return *this;
}

/*
 * Copy of a set of paths
 */
PathSet &PathSet::operator=(PathSet & inSet)
{
  /* If not empty, destroys the paths */
  clear();
  /* Copy */
  copy(inSet);
  return *this;
}

uint16_t PathSet::count()
{
  uint16_t c = 0;
  Path *p = mListHead;
  while (p != NULL) {
    if (! p->isEmpty()) c++;
    p = p->mNext;
  }
  return c;
}

#ifdef DEBUG
void PathSet::print()
{
  Path *p = mListHead;
  Serial.println("================");
  if (p == NULL) {
    Serial.println("*    No Path   *");
  }
  while (p != NULL) {
    p->println();
    p = p->mNext;
  }
  Serial.println("================");
}

void PathSet::println()
{
  print();
  Serial.println();
}
#endif
