#include <iostream>
#include "schrodingersReverb.h"
#include <chrono>
#include <thread>

int main() {
  SchrodingersReverb reverb(0);
  float x = 0.5;

  auto start = std::chrono::high_resolution_clock::now();

  for (int i = 0; i < 10000000; i++) {
  	reverb.process(x);
  }
  auto stop = std::chrono::high_resolution_clock::now();
  auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
  std::cout << "test complete! it ran for " << duration.count() << "ms" <<  std::endl;
  return 1;
}
