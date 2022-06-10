#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <queue>
#include <array>

#include "task_audio_io.h"

int main() {
    std::ofstream o;
    o.open("/home/reverb/data.txt");

    std::queue<int> adc_output;
    std::queue<int> dac_input;

    std::chrono::nanoseconds duration;

    //auto start_time = std::chrono::steady_clock::now();

    std::thread adc_task(TaskAudioIO(), &adc_output, &dac_input, &duration);
    adc_task.join();

    //auto stop_time = std::chrono::steady_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

    std::cout << "execution time: " << duration.count() << " nanoseconds" << std::endl;
    std::cout << "sample frequency: " << 100000/(duration.count() * 1E-9) << " Hz" << std::endl;

    while (!adc_output.empty()) {
        o << adc_output.front() << std::endl;
        //std::cout << "sample: " << adc_output.front() << std::endl;
        adc_output.pop();
    }

    o.close();

    return 0;
}


