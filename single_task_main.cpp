#include <iostream>
#include <chrono>
#include <thread>
#include <queue>
#include <atomic>
#include <stdlib.h>

#include "schrodingersReverb.h"
#include "adc/task_audio_io.h"
#include "adc/task_audio_io.cpp"

std::atomic<bool> ui_done(false);

void user_input(SchrodingersReverb *reverb) {
  std::string mix;

  std::cout << "Dry wet mix (value between 0.0f - 1.0f): ";

  if (!std::getline(std::cin, mix)) { /* I/O error! */ return; }

  if (!mix.empty()) {
      if (std::stof(mix) > 0.0f && std::stof(mix) < 1.0f) {
          reverb->setDryWetMix(std::stof(mix));
          std::cout << "\ndry wet mix updated" << std::endl;
      } else {
          std::cout << "Enter a value between 0.0 and 1.0" << std::endl;
      }
  } else {
      std::cout << "No input given" <<std::endl;
  }
  ui_done = true;
}

int main() {
    const std::size_t buffersize = 128;
    TaskAudioIO<buffersize> audio_io;
    SchrodingersReverb reverb(buffersize, 0);

    // FIFOs: naming convention: fifo_<source>_<sink>
    std::queue<float> fifo_adc_reverb;
    std::queue<float> fifo_reverb_dac;

    std::thread audio_task(&TaskAudioIO<buffersize>::run, audio_io, &fifo_adc_reverb, &fifo_reverb_dac);
    //std::thread audio_task(&TaskAudioIO<buffersize>::run, audio_io, &fifo_adc_reverb, &fifo_adc_reverb);

    std::thread user_input_task;
    user_input_task = std::thread(&user_input, &reverb);

    std::cout << "Schroeder Reverb is running..." << std::endl;
    while (1) {
        if (fifo_adc_reverb.size() > buffersize) {
            std::thread reverb_task(&SchrodingersReverb::process_single_task, &reverb, &fifo_adc_reverb, &fifo_reverb_dac);
            reverb_task.join();
            //std::cout << "fifo_adc_rev: " << fifo_adc_reverb.size() << ", fifo_rev_dac: " << fifo_reverb_dac.size() << std::endl;
        }
        if (ui_done) {
            user_input_task.join();
            user_input_task = std::thread(&user_input, &reverb);
            ui_done = false;
        }
    };
    return 0;
}
