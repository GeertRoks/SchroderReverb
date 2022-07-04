#include <iostream>
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
    RTS_Buffer<float> ap(buffersize);

    float* impulse = new float[buffersize]();
    float* response = new float[buffersize]();
    impulse[0] = 1.0f;
    for (int i = 1; i<buffersize; i++) {
        impulse[i] = 0.0f;
    }


    std::cout << "Sum-Allpass combo task perf Test: Using loop_amount: " << loop_amount << std::endl;

    while (count > 0) {
        reverb.reset();
        while(!ap.write(impulse)) { std::cout << "ap.write(impulse): waiting to output\n";}

        reverb.hyper_edge_task(&ap, &edge1, &edge2, &edge3, &edge4, &dry);

        start = std::chrono::steady_clock::now();
        reverb.sum_ap_task(&edge1, &edge2, &edge3, &edge4, &ap);
        stop = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);

        dry.read(response);
        ap.read(response);

        push_avg += duration.count();
        if (duration.count() > push_max) {
            push_max = duration.count();
        }
        if (duration.count() < push_min) {
            push_min = duration.count();
        }

        count --;
    }

    std::cout << "output: ";
    for (int i = 0; i<buffersize; i++) {
      std::cout << response[i] << ", ";
    }
    std::cout << std::endl;

    std::cout << "avg runtime: " << push_avg/loop_amount << " ns, per sample: " << push_avg/(float)(loop_amount*buffersize) << " ns" << std::endl;
    std::cout << "min runtime: " << push_min << " ns, per sample: " << push_min/(float)buffersize << "ns" << std::endl;
    std::cout << "max runtime: " << push_max << " ns, per sample: " << push_max/(float)buffersize << " ns" << std::endl;
    return 0;
}


