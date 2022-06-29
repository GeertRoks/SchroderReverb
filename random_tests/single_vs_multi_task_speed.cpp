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

  SchrodingersReverb reverb(buffersize,0);
  reverb.setDryWetMix(1.0f);

  std::ofstream m, s;

  std::chrono::time_point<std::chrono::steady_clock> start, stop;
  std::chrono::nanoseconds duration_m, duration_s;

  float* input = new float[buffersize]();
  float* output_s = new float[buffersize]();
  float* output_m = new float[buffersize]();

  input[0] = 1.0f;
  for (int i = 1; i<buffersize; i++) {
      input[i] = 0.0f;
  }

  s.open("./random_tests/single_task_test.dat");
  reverb.reset();

  start = std::chrono::steady_clock::now();
  reverb.process_single_task(input, output_s);
  stop = std::chrono::steady_clock::now();
  duration_s = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << "Single Task - ";
  std::cout << "in: " << 1.0f;
  std::cout << ", out: " << output_s[0];
  std::cout << ", duration: " << duration_s.count() << " ns";
  std::cout << ", per sample: " << duration_s.count()/buffersize << " ns" << std::endl;


  for (int i = 1; i<buffersize; i++) {
    //std::cout << "sample: " << adc_output[i] << std::endl;
    s << output_s[i] << std::endl;
  }
  s.close();


  m.open("./random_tests/multi_task_test.dat");

  start = std::chrono::steady_clock::now();
  std::thread reverb_task(&SchrodingersReverb::process_multi_task, &reverb, input, output_m);
  reverb_task.join();
  //reverb.process_multi_task(input, output_m);
  stop = std::chrono::steady_clock::now();

  duration_m = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

  std::cout << "Multi Tasks - ";
  std::cout << "in: " << 1.0f;
  std::cout << ", out: " << output_m[0];
  std::cout << ", duration: " << duration_m.count() << " ns";
  std::cout << ", per sample: " << duration_m.count()/buffersize << " ns" << std::endl;

  for (int i = 1; i<buffersize; i++) {
    //std::cout << "sample: " << output_m[i] << std::endl;
    m << output_m[i] << std::endl;
  }

  m.close();

  std::cout << "duration difference: multi - single = " << duration_m.count() - duration_s.count() << " ns, per sample: " << ( duration_m.count() - duration_s.count() ) /buffersize << " ns" << std::endl;

  return 0;
}
