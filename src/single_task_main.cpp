#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <stdlib.h>

#include "schrodingersReverb.h"
#include "adc/task_audio_io.h"
#include "adc/task_audio_io.cpp"

void user_input(SchrodingersReverb *reverb, std::atomic<bool>* ui_done) {
  std::string mix;

  std::cout << "Dry wet mix (value between 0.0f - 1.0f): ";

  if (!std::getline(std::cin, mix)) { /* I/O error! */ return; }

  if (!mix.empty()) {
      if (std::stof(mix) > 0.0f && std::stof(mix) < 1.0f) {
          reverb->setDryWetMix(std::stof(mix));
         // std::cout << "\ndry wet mix updated" << std::endl;
      } else {
          std::cout << "Enter a value between 0.0 and 1.0" << std::endl;
      }
  } else {
      std::cout << "No input given" <<std::endl;
  }
  *ui_done = true;
}

int main() {
    const std::size_t buffersize = 128;
    TaskAudioIO<buffersize> audio_io;
    SchrodingersReverb reverb(buffersize, 0);

    // FIFOs: naming convention: fifo_<source>_<sink>
    float* fifo_adc_reverb = new float[buffersize];
    float* fifo_reverb_dac = new float[buffersize];

    std::atomic<bool> ui_done(false);
    std::atomic<bool> new_audio_block(false);

    std::thread audio_task(&TaskAudioIO<buffersize>::run, audio_io, fifo_adc_reverb, fifo_reverb_dac, &new_audio_block);

    std::cout << "Schroeder Reverb is running..." << std::endl;

    std::thread user_input_task;
    user_input_task = std::thread(&user_input, &reverb, &ui_done);

    while (1) {
        if (new_audio_block) {
            std::thread reverb_task(&SchrodingersReverb::process_single_task, &reverb, fifo_adc_reverb, fifo_reverb_dac);
            new_audio_block = false;
            reverb_task.join();
            //std::cout << "fifo_adc_rev: " << fifo_adc_reverb.size() << ", fifo_rev_dac: " << fifo_reverb_dac.size() << std::endl;
        }
        if (ui_done) {
            user_input_task.join();
            user_input_task = std::thread(&user_input, &reverb, &ui_done);
            ui_done = false;
        }
    };
    return 0;
}
