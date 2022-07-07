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

    RTS_Buffer<float> input(buffersize);
    RTS_Buffer<float> output(buffersize);

    float* impulse = new float[buffersize]();
    float* response = new float[buffersize]();
    impulse[0] = 1.0f;
    for (int i = 1; i<buffersize; i++) {
        impulse[i] = 0.0f;
    }

    std::cout << "Comb perf Test: Using loop_amount: " << loop_amount << std::endl;

    while (count > 0) {
        reverb.reset();
        input.write(impulse);

        start = std::chrono::steady_clock::now();
        reverb.comb1_task(&input, &output);
        stop = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);

        output.read(response);

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

    //std::cout << "avg runtime: " << push_avg/loop_amount << " ns, per sample: " << push_avg/(loop_amount*buffersize) << " ns" << std::endl;
    //std::cout << "min runtime: " << push_min << " ns, per sample: " << push_min/buffersize << "ns" << std::endl;
    //std::cout << "max runtime: " << push_max << " ns, per sample: " << push_max/buffersize << " ns" << std::endl;

    std::cout << "=======================================================================" << std::endl;

    std::cout << std::setw(14) << std::left << "& buffer size" << std::setw(20) << "& avg" << std::setw(20) << "& min" << std::setw(20) << "& max" << "\\\\" << std::endl;
    std::cout << "& " << std::setw(12) << std::left << buffersize << "& " << std::setw(18) << push_avg/loop_amount << "& " << std::setw(18) << push_min << "& " << std::setw(18) << push_max << "\\\\" << std::endl;
    std::cout << "& " << std::setw(12) << buffersize << "& " << std::setw(18) << push_avg/(loop_amount*buffersize) << "& " << std::setw(18) << push_min/buffersize << "& " << std::setw(18) << push_max/buffersize << "\\\\" << std::endl;

    return 0;
}
