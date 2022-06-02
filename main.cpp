#include <iostream>
#include "schrodingersReverb.h"
#include <chrono>

int main() {
  SchrodingersReverb reverb(0);
  float x = 0.5;

  auto start = std::chrono::high_resolution_clock::now();

  reverb.process((void*) &x);
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);
  std::cout << "test complete! it ran for " << duration.count() << "ms" <<  std::endl;
  return 1;
}
