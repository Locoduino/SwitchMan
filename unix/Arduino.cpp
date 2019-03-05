#include "Arduino.h"
#include <sys/time.h>

extern void setup();
extern void loop();

char * sm_strcpy(char * dst, const char * src)
{
  char *d = dst;
  while (*src != '\0') *d++ = *src++;
  *d = '\0';
  return dst;
}

static struct timeval initialT;

void initTimeAtStartup()
{
  gettimeofday(&initialT, NULL);
}

uint32_t micros()
{
  struct timeval currentT;
  gettimeofday(&currentT, NULL);
  time_t elapsedSecs = currentT.tv_sec - initialT.tv_sec;
  suseconds_t elapsedMicros;
  if (currentT.tv_usec > initialT.tv_usec) {
    elapsedMicros = currentT.tv_usec - initialT.tv_usec;
  }
  else {
    elapsedSecs--;
    elapsedMicros = currentT.tv_usec + 1000000 - initialT.tv_usec;
  }
  return elapsedSecs * 1000000 + elapsedMicros;
}

uint32_t millis()
{
  struct timeval currentT;
  gettimeofday(&currentT, NULL);
  time_t elapsedSecs = currentT.tv_sec - initialT.tv_sec;
  suseconds_t elapsedMicros;
  if (currentT.tv_usec > initialT.tv_usec) {
    elapsedMicros = currentT.tv_usec - initialT.tv_usec;
  }
  else {
    elapsedSecs--;
    elapsedMicros = currentT.tv_usec + 1000000 - initialT.tv_usec;
  }
  return elapsedSecs * 1000 + elapsedMicros / 1000;
}

int main()
{
  initTimeAtStartup();
  setup();
  while (1) loop();

  return 0;
}
