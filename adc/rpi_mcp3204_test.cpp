#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <queue>

#include "task_audio_io.h"

int main() {
    std::ofstream o;
    o.open("./adc_test.dat");
    TaskAudioIO audio_io;


    // naming convention: fifo_<source>_<sink>
    std::queue<int> fifo_adc_dac;

    std::chrono::nanoseconds duration;
    int counter = 250000; // 10 seconds for 25KHz samplefreq
    //auto start_time = std::chrono::steady_clock::now();

    // run thread for 10 seconds and send the adc output back to the dac input
    std::thread audio_task(&TaskAudioIO::run_debug, audio_io, &fifo_adc_dac, &fifo_adc_dac, &duration, counter);
    audio_task.join();

    //auto stop_time = std::chrono::steady_clock::now();

    //auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

    std::cout << "execution time: " << duration.count() << " nanoseconds" << std::endl;
    std::cout << "sample frequency: " << counter/(duration.count() * 1E-9) << " Hz" << std::endl;

    while (!fifo_adc_dac.empty()) {
        //std::cout << "sample: " << adc_output.front() << std::endl;
        o << fifo_adc_dac.front() << std::endl;
        fifo_adc_dac.pop();
    }

    //o.close();

    return 0;
}


