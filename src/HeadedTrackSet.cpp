/*
 *  Ensemble de voies orientées.
 *  L'ensemble est un vecteur de bits indexé par l'identifiant
 *  des voies * 2 + l'orientation : 0 sens avant, 1 sens arriere.
 *  Chaque bit indique l'appartenance d'une voie à l'ensemble
 *  - 1 la Voie appartient à l'ensemble
 *  - 0 elle n'appartient pas à l'ensemble.
 */
#include "HeadedTrackSet.h"

void HeadedTrackSet::allocate()
{
  mSet = new byte[2 * Track::sizeForSet()];
#ifdef TRACE2
  Serial.print(F("*** HeadedTrackSet : allocate "));
  Serial.print(2 * Track::sizeForSet());
  Serial.println(F(" bytes"));
#endif
}

HeadedTrackSet::HeadedTrackSet()
{
  allocate();
  clear();
}

HeadedTrackSet::HeadedTrackSet(
  const HeadedTrackSet & inSet)
{
//  Serial.println("  Constructeur par recopie");
//  delay(1000);
  allocate();
  operator=(inSet);
}

HeadedTrackSet::~HeadedTrackSet()
{
//  Serial.println("  destruction d'un ensemble de voies");
//  delay(1000);
  delete mSet;
//  Serial.println("  destruction terminee");
//  delay(1000);
}

void HeadedTrackSet::clear()
{
  for (uint8_t i = 0; i < (2 * Track::sizeForSet()); i++) {
    mSet[i] = 0;
  }
}

bool HeadedTrackSet::isEmpty()
{
  for (uint8_t i = 0; i < 2 * Track::sizeForSet(); i++) {
    if (mSet[i] != 0) return false;
  }
  return true;
}

void HeadedTrackSet::addTrack(uint8_t inId, uint8_t inDir)
{
  mSet[(inId >> 2)] |=
    1 << (((inId & 3) << 1) + inDir );
#ifdef TRACE2
  Serial.print(F("*** HeadedTrackSet : add, byte "));
  Serial.print(inId >> 2);
  Serial.print(F(" bit "));
  Serial.println(((inId & 3) << 1) + inDir);
#endif
}

void HeadedTrackSet::removeTrack(uint8_t inId, uint8_t inDir)
{
  mSet[(inId >> 2)] &=
    ~(1 << (((inId & 3) << 1) + inDir ));
}

bool HeadedTrackSet::containsTrack(uint8_t inId, uint8_t inDir)
{
  return (mSet[(inId >> 2)] &
    1 << (((inId & 3) << 1) + inDir )) != 0;
}

HeadedTrackSet & HeadedTrackSet::operator=(
  const HeadedTrackSet & inSet)
{
  for (uint8_t i = 0; i < 2 * Track::sizeForSet(); i++) {
    mSet[i] = inSet.mSet[i];
  }
  return *this;
}

bool HeadedTrackSet::operator==(HeadedTrackSet & inSet)
{
  bool result = true;
  for (byte i = 0; i < 2 * Track::sizeForSet(); i++) {
    result = result && (mSet[i] == inSet.mSet[i]);
  }
  return result;
}


#ifdef DEBUG
void HeadedTrackSet::print()
{
  bool emptySet = true;
  for (uint8_t i = 0; i < 2 * Track::sizeForSet(); i++) {
    uint8_t subVec = mSet[i];
    for (uint8_t j = 0; j < 4; j++) {
      uint8_t id = (i << 2) + j;
      if (subVec & (1 << (2 * j))) {
        emptySet = false;
        Track::trackForId(id).print();
        Serial.print('/');
        displayDirection(FORWARD_DIRECTION);
        Serial.print(' ');
      }
      if (subVec & (1 << ((2 * j) +1))) {
        emptySet = false;
        Track::trackForId(id).print();
        Serial.print('/');
        displayDirection(BACKWARD_DIRECTION);
        Serial.print(' ');
      }
    }
  }
  if (emptySet) {
    Serial.print("*empty*");
  }
}

void HeadedTrackSet::println()
{
  print();
  Serial.println();
}

void HeadedTrackSet::printraw()
{
  for (uint8_t i = 0; i < 2 * Track::sizeForSet(); i++) {
    Serial.print((unsigned long)mSet[i], HEX);
    Serial.print(' ');
  }
}

void HeadedTrackSet::printlnraw()
{
  printraw();
  Serial.println();
}
#endif
