#include <bits/chrono.h>
#include <iostream>
#include <queue>
#include <chrono>
#include <random>

#include "../schrodingersReverb.h"

int main() {
  std::random_device rd;
  std::mt19937 gen(rd());
  std::uniform_real_distribution<> distr(-1.0f, 1.0f);

  SchrodingersReverb reverb(0);
  std::queue<float> input, output;

  for (int i = 0; i<128; i++) {
    input.push(distr(gen));
  }

  std::chrono::time_point<std::chrono::steady_clock> start, stop;

  float in = input.front();
  start = std::chrono::steady_clock::now();
  reverb.process(&input, &output);
  stop = std::chrono::steady_clock::now();

  auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << "in: " << in;
  std::cout << ", out: " << output.front();
  std::cout << ", duration: " << duration.count() << " ns" << std::endl;
  return 0;
}
