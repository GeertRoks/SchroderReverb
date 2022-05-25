#ifndef SCHRODINGERSREVERB_H_
#define SCHRODINGERSREVERB_H_

#include "filters/allpassDFII.h"
#include "filters/comb.h"

class SchrodingersReverb {

public:
  SchrodingersReverb(int reverbTime);
  ~SchrodingersReverb();

  float process(float x);

private:
  int reverbTime = 0; //ms ???

  AllpassDFII allpass1 = AllpassDFII(125, 0.7);
  AllpassDFII allpass2 = AllpassDFII(42, 0.7);
  AllpassDFII allpass3 = AllpassDFII(12, 0.7);

  Comb comb1 = Comb(901, 0.805);
  Comb comb2 = Comb(778, 0.827);
  Comb comb3 = Comb(1011, 0.783);
  Comb comb4 = Comb(1123, 0.764);
};//class

#endif//SCHRODINGERSREVERB_H_

/**
  TODO:
    - array of filter pointers.
    - parameter control.
    - Combine ticks.
    - Combine fillbuffer.

*/
