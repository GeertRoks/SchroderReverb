#include <iostream>
#include <fstream>
#include <queue>
#include <chrono>
#include <random>
#include <stdlib.h>

#include "../schrodingersReverb.h"

int main(int argc, char* argv[]){
  unsigned short buffersize = 128;
  if (argc > 1) {
      buffersize = atoi(argv[1]);
      std::cout << "buffersize set to: " << buffersize << std::endl;
  } else {
      std::cout << "no buffer size given. Usage: " << argv[0] << " <int buffersize>" << std::endl;
      return 1;
  }
  std::ofstream o, s;
  o.open("./random_tests/rev_multi_thread_test.dat");

  SchrodingersReverb reverb(buffersize,0);
  reverb.setDryWetMix(1.0f);
  std::queue<float> input, output_m, output_s;

  input.push(1.0f);
  for (int i = 1; i<buffersize; i++) {
    input.push(0.0f);
  }

  std::chrono::time_point<std::chrono::steady_clock> start, stop;

  start = std::chrono::steady_clock::now();
  std::thread reverb_task(&SchrodingersReverb::process_multi_task, &reverb, &input, &output_m);
  reverb_task.join();
  //reverb.process_multi_task(&input, &output_m);
  stop = std::chrono::steady_clock::now();

  auto duration_m = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << "Multi Tasks - ";
  std::cout << "in: " << 1.0f;
  std::cout << ", out: " << output_m.front();
  std::cout << ", duration: " << duration_m.count() << " ns";
  std::cout << ", per sample: " << duration_m.count()/buffersize << " ns" << std::endl;

  while (!output_m.empty()) {
    //std::cout << "sample: " << output_m.front() << std::endl;
    o << output_m.front() << std::endl;
    output_m.pop();
  }

  o.close();

  s.open("./random_tests/rev_single_thread_test.dat");
  reverb.reset();

  input.push(1.0f);
  for (int i = 1; i<buffersize; i++) {
    input.push(0.0f);
  }

  start = std::chrono::steady_clock::now();
  reverb.process_single_task(&input, &output_s);
  stop = std::chrono::steady_clock::now();
  auto duration_s = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << "Single Task - ";
  std::cout << "in: " << 1.0f;
  std::cout << ", out: " << output_s.front();
  std::cout << ", duration: " << duration_s.count() << " ns";
  std::cout << ", per sample: " << duration_s.count()/buffersize << " ns" << std::endl;

  std::cout << "duration difference: multi - single = " << duration_m.count() - duration_s.count() << " ns, per sample: " << ( duration_m.count() - duration_s.count() ) /buffersize << " ns" << std::endl;

  while (!output_s.empty()) {
    //std::cout << "sample: " << adc_output.front() << std::endl;
    s << output_s.front() << std::endl;
    output_s.pop();
  }
  s.close();

  return 0;
}
