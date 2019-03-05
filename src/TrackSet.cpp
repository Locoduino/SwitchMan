/*
 *  Ensemble de voie. L'ensemble est un vecteur de bits indexé par l'identifiant
 *  des voies. Chaque bit indique l'appartenance d'une voie à l'ensemble
 *  - 1 la Voie appartient à l'ensemble
 *  - 0 elle n'appartient pas à l'ensemble.
 */
#include "TrackSet.h"

void TrackSet::allocate()
{
  mSet = new uint8_t[Track::sizeForSet()];
}

TrackSet::TrackSet()
{
  allocate();
  clear();
}

TrackSet::TrackSet(const TrackSet & set)
{
//  Serial.println("  Constructeur par recopie");
//  delay(1000);
  allocate();
  operator=(set);
}

TrackSet::~TrackSet()
{
//  Serial.println("  destruction d'un ensemble de voies");
//  delay(1000);
  delete mSet;
//  Serial.println("  destruction terminee");
//  delay(1000);
}

void TrackSet::clear()
{
  for (uint8_t i = 0; i < Track::sizeForSet(); i++) {
    mSet[i] = 0;
  }
}

bool TrackSet::isEmpty()
{
  for (uint8_t i = 0; i < Track::sizeForSet(); i++) {
    if (mSet[i] != 0) return false;
  }
  return true;
}

void TrackSet::addTrack(const uint8_t inId)
{
  mSet[inId >> 3] |= 1 << (inId & 7);
}

void TrackSet::removeTrack(const uint8_t inId)
{
  mSet[inId >> 3] &= ~(1 << (inId & 7));
}

bool TrackSet::containsTrack(const uint8_t inId)
{
  return (mSet[inId >> 3] & 1 << (inId & 7)) != 0;
}

TrackSet & TrackSet::operator=(const TrackSet & inSet)
{
  for (uint8_t i = 0; i < Track::sizeForSet(); i++) {
    mSet[i] = inSet.mSet[i];
  }
  return *this;
}

bool TrackSet::operator==(TrackSet & inSet)
{
  bool result = true;
  for (uint8_t i = 0; i < Track::sizeForSet(); i++) {
    result = result && (mSet[i] == inSet.mSet[i]);
  }
  return result;
}


#ifdef DEBUG
void TrackSet::print()
{
  bool emptySet = true;
  for (uint8_t i = 0; i < Track::sizeForSet(); i++) {
    uint8_t subVec = mSet[i];
    for (uint8_t j = 0; j < 8; j++) {
      uint8_t id = (i << 3) + j;
      if (subVec & (1 << j)) {
        emptySet = false;
        Track::trackForId(id).print();
        Serial.print(' ');
      }
    }
  }
  if (emptySet) {
    Serial.print("*empty*");
  }
}

void TrackSet::println()
{
  print();
  Serial.println();
}
#endif
