#include "SwitchMan.h"
#include "Specifs.h"

#ifdef DEBUG

void afficheLigne(byte nb)
{
  while(nb--) {
    Serial.print('-');
  }
  Serial.println();
}
#endif


unsigned long nombreItineraires = 0;
unsigned long nombreDeVides = 0;

void testItineraire(byte source, byte destination)
{
  PathSet iti;
  Serial.println("################");
  Serial.print("De ");
  displayTrack(source);
  Serial.print(" a ");
  displayTrackln(destination);

  unsigned long combien = iti.count();
  Track::trackForId(source).pathsTo(destination, FORWARD_DIRECTION, iti);
  nombreItineraires += combien;
  if (combien == 0) nombreDeVides++;
  iti.print();
}

void testItineraireManu(byte source, byte destination)
{
  unsigned long d;
  PathSet iti;
  d = micros();
  Track::trackForId(source).pathsTo(destination, FORWARD_DIRECTION, iti);
  Serial.print(F(" temps de calcul uS : "));Serial.println(micros() - d);
  iti.print();
}

void setup() {
  Track::finalize();

  Serial.begin(115200);
  Serial.println(F("\nGestionnaire reseau"));

  export_setup();

  aiguille6.println();

#ifdef DEBUG
  Serial.print(F("Nombre de voies : "));
  Serial.println((unsigned long)Track::count());
#endif
}    // fin setup


void loop() {
}
