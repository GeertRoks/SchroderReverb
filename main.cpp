#include <iostream>
#include <chrono>
#include <thread>
#include <queue>

#include "schrodingersReverb.h"
#include "adc/task_audio_io.h"
#include "adc/task_audio_io.cpp"

int main() {
    const std::size_t buffersize = 64;
    TaskAudioIO<buffersize> audio_io;
    SchrodingersReverb reverb(0);

    // FIFOs: naming convention: fifo_<source>_<sink>
    std::queue<float> fifo_adc_reverb;
    std::queue<float> fifo_reverb_dac;

    std::thread audio_task(&TaskAudioIO<buffersize>::run, audio_io, &fifo_adc_reverb, &fifo_reverb_dac);
    //std::thread audio_task(&TaskAudioIO<buffersize>::run, audio_io, &fifo_adc_reverb, &fifo_adc_reverb);


    std::cout << "Schroeder Reverb is running..." << std::endl;
    float sample = 0.0f;
    while (1) {
        if (!fifo_adc_reverb.empty()) {
            sample = reverb.process(fifo_adc_reverb.front());
            //fifo_reverb_dac.push(fifo_adc_reverb.front());
            fifo_adc_reverb.pop();
            fifo_reverb_dac.push(sample);
        }
    };
    return 0;
}
