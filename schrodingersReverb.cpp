
#include "schrodingersReverb.h"

SchrodingersReverb::SchrodingersReverb(int reverbTime) {
  this->reverbTime = reverbTime;

}//SchrodingersReverb()

SchrodingersReverb::~SchrodingersReverb() {

}//~SchrodingersReverb()


float SchrodingersReverb::process(float x) {
//Reference: https://www.dsprelated.com/freebooks/pasp/Schroeder_Reverberators.html
  float y = (comb1.process(x) + comb2.process(x) + comb3.process(x) + comb3.process(x)) * 0.25;
  y = allpass1.process(y);
  y = allpass2.process(y);
  y = allpass3.process(y);
  return y;
}//process()
