#include <iostream>
#include <chrono>
#include <math.h>

#include "../filters/allpassDFII.h"

int main()
{
    AllpassDFII allpass(347, 0.70);

    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
	std::chrono::nanoseconds duration;

    float x = 1.0;
    const int loop_amount = 100000;
    int count = loop_amount;

    unsigned int push_avg = 0;
    unsigned int push_max = 0;
    unsigned int push_min = 0 - 1;

    std::cout << "AllpassDFII perf Test: Using loop_amount: " << loop_amount << std::endl;

    while (count > 0) {
        start = std::chrono::steady_clock::now();
        allpass.process(x);
        stop = std::chrono::steady_clock::now();
        x = sin(count);
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
    std::cout << "Average runtime duration: " << push_avg/loop_amount  << std::endl;
    std::cout << "min runtime: " << push_min << " ns, max runtime: " << push_max << " ns" << std::endl;
    return 0;
}
