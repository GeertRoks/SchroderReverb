#include <iostream>
#include "schrodingersReverb.h"
#include <chrono>
#include <thread>

int main() {
  SchrodingersReverb reverb(0);
  float x = 0.5;
  Comb* combs[4];

  Comb *combThread1 = new Comb(901, 0.805);
  Comb *combThread2 = new Comb(778, 0.827);
  Comb *combThread3 = new Comb(1011, 0.783);
  Comb *combThread4 = new Comb(1123, 0.764);

  combs[0] = combThread1;
  combs[1] = combThread2;
  combs[2] = combThread3;
  combs[3] = combThread4;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 64; i++) {
  	reverb.process((void*) &x, combs);
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "test complete! it ran for " << duration.count() << "ms" <<  std::endl;
  return 1;
}
