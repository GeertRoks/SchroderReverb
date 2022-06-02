
#include "schrodingersReverb.h"

SchrodingersReverb::SchrodingersReverb(int reverbTime) {
  this->reverbTime = reverbTime;

}//SchrodingersReverb()

SchrodingersReverb::~SchrodingersReverb() {
}//~SchrodingersReverb()


float SchrodingersReverb::process(void* x) {
//Reference: https://www.dsprelated.com/freebooks/pasp/Schroeder_Reverberators.html
  float* x_val = (float *) x;
  float resC1, resC2, resC3, resC4;
  float t = *x_val;
  Comb *combThread1 = new Comb(901, 0.805);
  Comb *combThread2 = new Comb(778, 0.827);
  Comb *combThread3 = new Comb(1011, 0.783);
  Comb *combThread4 = new Comb(1123, 0.764);

  std::thread th(&Comb::process, combThread1, t, &resC1);
  std::thread thr(&Comb::process, combThread2, t, &resC2);
  std::thread thre(&Comb::process, combThread3, t, &resC3);
  std::thread threa(&Comb::process, combThread4, t, &resC4);
  th.join();
  thr.join();
  thre.join();
  threa.join();
 
  float y = (resC1 + resC2 + resC3 + resC4) * 0.25;

  std::cout << "result of y" << y << std::endl;
 // y = allpass1.process(y);
  //y = allpass2.process(y);
  //y = allpass3.process(y);
  return 0.0;
}//process()
