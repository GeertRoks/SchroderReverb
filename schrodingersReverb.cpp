
#include "schrodingersReverb.h"

SchrodingersReverb::SchrodingersReverb(int reverbTime) {
  this->reverbTime = reverbTime;

}//SchrodingersReverb()

SchrodingersReverb::~SchrodingersReverb() {
}//~SchrodingersReverb()


float SchrodingersReverb::process(float x) {
//Reference: https://www.dsprelated.com/freebooks/pasp/Schroeder_Reverberators.html
  
  th = std::thread(&Comb::process, combThread1, x, &resC1);
  thr = std::thread(&Comb::process, combThread2, x, &resC2);
  thre = std::thread(&Comb::process, combThread3, x, &resC3);
  threa = std::thread(&Comb::process, combThread4, x, &resC4);

  //std::thread th(&Comb::process, combs[0], t, &resC1);
  //std::thread thr(&Comb::process, combs[1], t, &resC2);
  //std::thread thre(&Comb::process, combs[2], t, &resC3);
  //std::thread threa(&Comb::process, combs[3], t, &resC4);
  th.join();
  thr.join();
  thre.join();
  threa.join();
 
  float y = (resC1 + resC2 + resC3 + resC4) * 0.25;

//  std::cout << "result of y" << y << std::endl;
 // y = allpass1.process(y);
  //y = allpass2.process(y);
  //y = allpass3.process(y);
  return 0.0;
}//process()
