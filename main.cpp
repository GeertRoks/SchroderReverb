#include <iostream>
#include "schrodingersReverb.h"

int main() {
  SchrodingersReverb reverb(0);
  int x = 1;

  reverb.process((void*) &x);
  std::cout << "test complete!" << std::endl;
  return 1;
}
