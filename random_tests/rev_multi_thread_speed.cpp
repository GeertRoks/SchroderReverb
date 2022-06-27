#include <bits/chrono.h>
#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
#include <random>

#include "../schrodingersReverb.h"

int main() {
  std::ofstream o;
  o.open("./random_tests/rev_multi_thread_test.dat");

  SchrodingersReverb reverb(0);
  std::queue<float> input, output;
  unsigned short buffersize = 128;

  input.push(1.0f);
  for (int i = 1; i<128; i++) {
    input.push(0.0f);
  }

  std::chrono::time_point<std::chrono::steady_clock> start, stop;

  float in = input.front();
  start = std::chrono::steady_clock::now();
  reverb.process(&input, &output, buffersize);
  stop = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << "in: " << in;
  std::cout << ", out: " << output.front();
  std::cout << ", duration: " << duration.count() << " ns";
  std::cout << ", per sample: " << duration.count()/buffersize << " ns" << std::endl;

  while (!output.empty()) {
    //std::cout << "sample: " << adc_output.front() << std::endl;
    o << output.front() << std::endl;
    output.pop();
  }

  o.close();

  return 0;
}
