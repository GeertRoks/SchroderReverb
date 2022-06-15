#include <iostream>
#include <chrono>
#include <thread>
#include <queue>

#include "schrodingersReverb.h"
#include "adc/task_audio_io.h"

int main() {
    TaskAudioIO audio_io;
    SchrodingersReverb reverb(0);

    // FIFOs: naming convention: fifo_<source>_<sink>
    std::queue<int> fifo_adc_reverb;
    std::queue<int> fifo_reverb_dac;

    std::thread audio_task(&TaskAudioIO::run, audio_io, &fifo_adc_reverb, &fifo_reverb_dac);


    std::cout << "Schroeder Reverb is running..." << std::endl;
    while (1) {
        if (!fifo_adc_reverb.empty()) {
            fifo_reverb_dac.push(fifo_adc_reverb.front());
            fifo_adc_reverb.pop();
        }
    };
    return 0;
}
