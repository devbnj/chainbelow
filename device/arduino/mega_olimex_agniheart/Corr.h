#include <stdint.h>

#ifndef Corr_h
#define Corr_h

typedef struct Corr {
  uint8_t mlcor;
  int mscor;
  int fcor;
  int dcor;
  float lcor;
} Corr;

typedef struct Corr2 {
  float l;
  uint8_t ml;
  int ms;
  int f;
  int d;
} Corr2;

#endif
