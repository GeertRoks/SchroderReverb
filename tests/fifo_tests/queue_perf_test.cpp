#include <iostream>
#include <queue>
#include <chrono>

int main()
{
    std::queue<int> fifo;

    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
	std::chrono::nanoseconds duration;

    unsigned int push_avg = 0;
    unsigned int push_max = 0;
    unsigned int push_min = 2147483647;
    unsigned int read_avg = 0;
    unsigned int read_max = 0;
    unsigned int read_min = 2147483647;
    unsigned int pop_avg = 0;
    unsigned int pop_max = 0;
    unsigned int pop_min = 2147483647;

    const int loop_amount = 100000;

    int count = loop_amount;
    const int bufsize = 128;

    std::cout << "Queue perf Test: Using loop_amount: " << loop_amount << ", bufsize: " << bufsize << std::endl;

    while (count > 0) {

        start = std::chrono::steady_clock::now();
        for (int i =0; i < bufsize; i++) {
            fifo.push(i);
        }
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
    std::cout << "Average push duration: " << push_avg/loop_amount  << " ns, per sample: " << push_avg/(loop_amount*bufsize) << " ns, ";
    std::cout << "min push time: " << push_min << " ns, max push time: " << push_max << " ns" << std::endl;

    count = loop_amount;

    while(count > 0) {
        start = std::chrono::steady_clock::now();
        for (int i =0; i < bufsize; i++) {
            fifo.front();
        }
        stop = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);

        read_avg += duration.count();
        if (duration.count() > read_max) {
            read_max = duration.count();
        }
        if (duration.count() < read_min) {
            read_min = duration.count();
        }

        count --;
    }

    std::cout << "Average read duration: " << read_avg/loop_amount  << " ns, per sample: " << read_avg/(loop_amount*bufsize) << " ns, ";
    std::cout << "min read time: " << read_min << " ns, max read time: " << read_max << " ns" << std::endl;

    count = loop_amount;
    while(count > 0) {
        start = std::chrono::steady_clock::now();
        for (int i =0; i < bufsize; i++) {
            fifo.front();
            fifo.pop();
        }
        stop = std::chrono::steady_clock::now();

        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);

        pop_avg += duration.count();
        if (duration.count() > pop_max) {
            pop_max = duration.count();
        }
        if (duration.count() < pop_min) {
            pop_min = duration.count();
        }

        count --;
    }
    std::cout << "Average pop  duration: " << pop_avg/loop_amount  << " ns, per sample: " << pop_avg/(loop_amount*bufsize) << " ns, ";
    std::cout << "min pop  time: " << pop_min << " ns, max pop  time: " << pop_max << " ns" << std::endl;

    return 0;
}
