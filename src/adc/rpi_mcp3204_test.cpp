#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>
#include <queue>

#include "task_audio_io.h"
#include "task_audio_io.cpp"

int main() {
    const std::size_t buffersize = 128;
    TaskAudioIO<buffersize> audio_io;

    std::ofstream o;
    o.open("./adc/adc_test.dat");

    float* buffer_adc = new float[buffersize]();
    float* buffer_dac = new float[buffersize]();

    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
	std::chrono::nanoseconds duration;
    int counter = 250000; // 10 seconds for 25KHz samplefreq
    //start_time = std::chrono::steady_clock::now();

    // run thread for 10 seconds and send the adc output back to the dac input
    std::thread audio_task(&TaskAudioIO<buffersize>::run_debug, audio_io, buffer_adc, buffer_dac, &duration, counter);
    audio_task.join();

    //stop_time = std::chrono::steady_clock::now();

    //duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

    std::cout << "execution time: " << duration.count() << " nanoseconds" << std::endl;
    std::cout << "sample frequency: " << counter/(duration.count() * 1E-9) << " Hz" << std::endl;

    for (unsigned short i = 0; i<buffersize; i++) {
        //std::cout << "sample: " << adc_output[i] << std::endl;
        o << buffer_adc[i] << std::endl;
    }

    o.close();

    return 0;
}


