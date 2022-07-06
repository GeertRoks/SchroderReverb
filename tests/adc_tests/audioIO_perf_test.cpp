#include <iostream>
#include <fstream>
#include <chrono>
#include <thread>

#include "../../src/adc/task_audio_io.h"

std::ofstream o;

void write_to_file_task(RTS_Buffer<float>* input, std::size_t* buffersize, int counter) {
    float* output = new float[*buffersize];
    int count = counter/ *buffersize;
    while (count > 0) {
        input->read(output);
        if (count < 100) {
            for (unsigned int i = 0; i < *buffersize; i++) {
                o << output[i] << std::endl;
            }
        }
        count--;
    }
}

int main() {
    std::size_t buffersize = 128;
    TaskAudioIO audio_io(buffersize);

    o.open("./bin/data/adc_test.dat");

    RTS_Buffer<float> buffer_adc(buffersize);
    RTS_Buffer<float> buffer_dac(buffersize);

    std::chrono::time_point<std::chrono::steady_clock> start_time, stop_time;
	std::chrono::nanoseconds duration;

    int counter = 250000; // 10 seconds for 25KHz samplefreq


    start_time = std::chrono::steady_clock::now();

    // run thread for 10 seconds and send the adc output back to the dac input
    //std::thread audio_task(&TaskAudioIO::run_debug, &audio_io, &buffer_adc, &buffer_adc, &duration, counter);
    std::thread audio_task(&TaskAudioIO::adc_task, &audio_io, &buffer_adc, counter);
    std::thread file_task(write_to_file_task, &buffer_adc, &buffersize, counter);
    audio_task.join();
    file_task.join();

    stop_time = std::chrono::steady_clock::now();


    duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

    std::cout << "execution time: " << duration.count() << " nanoseconds" << std::endl;
    std::cout << "sample frequency: " << counter/(duration.count() * 1E-9) << " Hz" << std::endl;

    o.close();

    return 0;
}


