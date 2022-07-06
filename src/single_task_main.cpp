#include <iostream>
#include <chrono>
#include <thread>
#include <atomic>
#include <stdlib.h>

#include "schrodingersReverb.h"
#include "adc/task_audio_io.h"

void user_input(SchrodingersReverb *reverb) {
  std::string text_input;

  while (1) {
      std::cout << "Dry wet mix (value between 0.0f - 1.0f): ";

      if (!std::getline(std::cin, text_input)) { /* I/O error! */ return; }

      if (!text_input.empty()) {
          if (std::stof(text_input) >= 0.0f && std::stof(text_input) <= 1.0f) {
              reverb->setDryWetMix(std::stof(text_input));
              // std::cout << "\ndry wet mix updated" << std::endl;
          } else {
              std::cout << "Enter a value between 0.0 and 1.0" << std::endl;
          }
      } else {
          std::cout << "No input given" <<std::endl;
      }
  }
}

int main() {
    const std::size_t buffersize = 128;
    TaskAudioIO audio_io(buffersize);
    SchrodingersReverb reverb(buffersize, 0);

    // FIFOs: naming convention: fifo_<source>_<sink>
    RTS_Buffer<float> fifo_adc_reverb(buffersize);
    RTS_Buffer<float> fifo_reverb_dac(buffersize);

    std::thread audio_task(&TaskAudioIO::run, &audio_io, &fifo_adc_reverb, &fifo_reverb_dac);
    std::thread reverb_task(&SchrodingersReverb::single_task, &reverb, &fifo_adc_reverb, &fifo_reverb_dac);
    std::thread user_input_task(&user_input, &reverb);

    std::cout << "Schroeder Reverb is running..." << std::endl;


    reverb_task.join();
    user_input_task.join();
    return 0;
}
