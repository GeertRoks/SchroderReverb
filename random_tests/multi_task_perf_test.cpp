#include <iostream>
#include <chrono>
#include <math.h>
#include <stdlib.h>

#include "../schrodingersReverb.h"

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

    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
	std::chrono::nanoseconds duration;

    const int loop_amount = 100000;
    int count = loop_amount;

    unsigned long long int push_avg = 0;
    unsigned int push_max = 0;
    unsigned int push_min = 0 - 1;

    float input[buffersize] = {};
    float output[buffersize] = {};

    input[0] = 1.0f;
    for (int i = 1; i<buffersize; i++) {
        input[i] = 0.0f;
    }

    std::cout << "Sum perf Test: Using loop_amount: " << loop_amount << std::endl;

    while (count > 0) {
        start = std::chrono::steady_clock::now();
        reverb.process_multi_task(input, output);
        stop = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);

        push_avg += duration.count();
        if (duration.count() > push_max) {
            push_max = duration.count();
        }
        if (duration.count() < push_min) {
            push_min = duration.count();
        }


        count --;
    }
    std::cout << "avg runtime: " << push_avg/loop_amount << " ns, per sample: " << push_avg/(loop_amount*buffersize) << " ns" << std::endl;
    std::cout << "min runtime: " << push_min << " ns, per sample: " << push_min/buffersize << "ns" << std::endl;
    std::cout << "max runtime: " << push_max << " ns, per sample: " << push_max/buffersize << " ns" << std::endl;
    return 0;
}

