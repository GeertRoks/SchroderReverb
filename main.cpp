#include <iostream>
#include "schrodingersReverb.h"

int main() {
  SchrodingersReverb reverb(0);
  reverb.process(0.5);
  std::cout << "test complete!" << std::endl;
  return 1;
}
