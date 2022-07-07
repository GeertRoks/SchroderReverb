#include <iostream>
#include <iomanip>
#include <chrono>
#include <math.h>
#include <stdlib.h>

#include "../../src/schrodingersReverb.h"
#include "../../src/rts_buffer.h"

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
    reverb.setSingleRunMode();
    reverb.setDryWetMix(1.0f);

    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
    std::chrono::nanoseconds duration;

    const int loop_amount = 100000;
    int count = loop_amount;

    unsigned long long int push_avg = 0;
    unsigned int push_max = 0;
    unsigned int push_min = 0 - 1;

    RTS_Buffer<float> edge1(buffersize);
    RTS_Buffer<float> edge2(buffersize);
    RTS_Buffer<float> edge3(buffersize);
    RTS_Buffer<float> edge4(buffersize);
    RTS_Buffer<float> dry(buffersize);
    RTS_Buffer<float> sum(buffersize);

    float* impulse = new float[buffersize]();
    float* response = new float[buffersize]();
    for (int i = 0; i<buffersize; i++) {
        impulse[i] = i;
    }

    sum.write(impulse);

    std::cout << "Sum perf Test: Using loop_amount: " << loop_amount << std::endl;

    while (count > 0) {
        start = std::chrono::steady_clock::now();
        reverb.hyper_edge_task(&sum, &edge1, &edge2, &edge3, &edge4, &dry);
        stop = std::chrono::steady_clock::now();

        reverb.sum_task(&edge1, &edge2, &edge3, &edge4, &sum);

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);

        dry.read(response);

        push_avg += duration.count();
        if (duration.count() > push_max) {
            push_max = duration.count();
        }
        if (duration.count() < push_min) {
            push_min = duration.count();
        }

        count --;
    }

    sum.read(response);

    std::cout << "output: ";
    for (int i = 0; i<buffersize; i++) {
      std::cout << response[i] << ", ";
    }
    std::cout << std::endl;

    //std::cout << "avg runtime: " << push_avg/loop_amount << " ns, per sample: " << push_avg/(float)(loop_amount*buffersize) << " ns" << std::endl;
    //std::cout << "min runtime: " << push_min << " ns, per sample: " << push_min/(float)buffersize << "ns" << std::endl;
    //std::cout << "max runtime: " << push_max << " ns, per sample: " << push_max/(float)buffersize << " ns" << std::endl;

    std::cout << "=======================================================================" << std::endl;

    std::cout << std::setw(14) << std::left << "& buffer size" << std::setw(20) << "& avg" << std::setw(20) << "& min" << std::setw(20) << "& max" << "\\\\" << std::endl;
    std::cout << "& " << std::setw(12) << std::left << buffersize << "& " << std::setw(18) << push_avg/loop_amount << "& " << std::setw(18) << push_min << "& " << std::setw(18) << push_max << "\\\\" << std::endl;
    std::cout << "& " << std::setw(12) << buffersize << "& " << std::setw(18) << push_avg/(loop_amount*buffersize) << "& " << std::setw(18) << push_min/buffersize << "& " << std::setw(18) << push_max/buffersize << "\\\\" << std::endl;

    return 0;
}

