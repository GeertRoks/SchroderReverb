#include <iostream>
#include <fstream>
#include <chrono>
#include <math.h>
#include <stdlib.h>

#include "../../src/schrodingersReverb.h"

int main(int argc, char* argv[])
{
    unsigned short buffersize = 1;
    if (argc > 1) {
        buffersize = atoi(argv[1]);
        std::cout << "buffersize set to: " << buffersize << std::endl;
    } else {
        std::cout << "no buffer size given. Usage: " << argv[0] << " <int buffersize>" << std::endl;
        return 1;
    }

    SchrodingersReverb reverb(buffersize, 0);
    reverb.setDryWetMix(1.0f);
    std::ofstream m, s;

    float* input = new float[buffersize]();
    float* output_m = new float[buffersize]();
    float* output_s = new float[buffersize]();


    bool flip = false;
    for (int i = 0; i<buffersize; i++) {
        if (!(i % 16)) {
            flip = !flip;
        }
        if (flip) {
        input[i] = 0.5f;
        } else {
        input[i] = -0.5f;
        }
    }

    input[0] = 1.0f;
    for (int i = 1; i<buffersize; i++) {
        input[i] = 0.0f;
    }
  m.open("./random_tests/multi_task_debug.dat");

  reverb.process_multi_task(input, output_m);

  for (int i = 0; i<buffersize; i++) {
    //std::cout << "sample: " << output_m[i] << std::endl;
    m << output_m[i] << std::endl;
  }

  m.close();

  s.open("./random_tests/single_task_debug.dat");

  reverb.process_single_task(input, output_s);

  for (int i = 0; i<buffersize; i++) {
    //std::cout << "sample: " << adc_output[i] << std::endl;
    s << output_s[i] << std::endl;
  }
  s.close();

    return 0;
//
//    while (count > 0) {
//        start = std::chrono::steady_clock::now();
//        reverb.process_multi_task(input, output);
//        stop = std::chrono::steady_clock::now();
//
//        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
//
//        push_avg += duration.count();
//        if (duration.count() > push_max) {
//            push_max = duration.count();
//        }
//        if (duration.count() < push_min) {
//            push_min = duration.count();
//        }
//
//        count --;
//    }
//    std::cout << "avg runtime: " << push_avg/loop_amount << " ns, per sample: " << push_avg/(loop_amount*buffersize) << " ns" << std::endl;
//    std::cout << "min runtime: " << push_min << " ns, per sample: " << push_min/buffersize << "ns" << std::endl;
//    std::cout << "max runtime: " << push_max << " ns, per sample: " << push_max/buffersize << " ns" << std::endl;
//    return 0;
}

