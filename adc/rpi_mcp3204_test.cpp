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
    TaskAudioIO audio_io;


    // naming convention: fifo_<source>_<sink>
    std::queue<int> fifo_adc_dac;

    std::chrono::nanoseconds duration;
    //auto start_time = std::chrono::steady_clock::now();

    std::thread adc_task(&TaskAudioIO::run, audio_io, &fifo_adc_dac, &fifo_adc_dac, &duration);
    adc_task.join();

    //auto stop_time = std::chrono::steady_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

    std::cout << "execution time: " << duration.count() << " nanoseconds" << std::endl;
    std::cout << "sample frequency: " << 100000/(duration.count() * 1E-9) << " Hz" << std::endl;

    while (!fifo_adc_dac.empty()) {
        o << fifo_adc_dac.front() << std::endl;
        //std::cout << "sample: " << adc_output.front() << std::endl;
        fifo_adc_dac.pop();
    }

    o.close();

    return 0;
}


