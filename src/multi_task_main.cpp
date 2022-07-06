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

int main(int argc, char* argv[]) {
    std::size_t buffersize = 1;
    if (argc > 1) {
        buffersize = atoi(argv[1]);
        std::cout << "buffersize set to: " << buffersize << std::endl;
    } else {
        std::cout << "no buffer size given. Usage: " << argv[0] << " <int buffersize>" << std::endl;
        return 1;
    }
    TaskAudioIO audio_io(buffersize);
    SchrodingersReverb reverb(buffersize, 0);

    // FIFOs: naming convention: fifo_<source>_<sink>
    //float* fifo_adc_reverb = new float[buffersize];
    //float* fifo_reverb_dac = new float[buffersize];

    RTS_Buffer<float> fifo_adc_rev(buffersize);
    RTS_Buffer<float> fifo_rev_comb1(buffersize);
    RTS_Buffer<float> fifo_rev_comb2(buffersize);
    RTS_Buffer<float> fifo_rev_comb3(buffersize);
    RTS_Buffer<float> fifo_rev_comb4(buffersize);
    RTS_Buffer<float> fifo_rev_dry(buffersize);
    RTS_Buffer<float> fifo_comb1_sum(buffersize);
    RTS_Buffer<float> fifo_comb2_sum(buffersize);
    RTS_Buffer<float> fifo_comb3_sum(buffersize);
    RTS_Buffer<float> fifo_comb4_sum(buffersize);
    RTS_Buffer<float> fifo_sum_ap1(buffersize);
    RTS_Buffer<float> fifo_ap1_ap2(buffersize);
    RTS_Buffer<float> fifo_ap2_ap3(buffersize);
    RTS_Buffer<float> fifo_ap3_mix(buffersize);
    RTS_Buffer<float> fifo_mix_dac(buffersize);

    std::cout << "Schroeder Reverb is running..." << std::endl;

    //std::thread audio_task(&TaskAudioIO::run, audio_io, &fifo_adc_reverb, &fifo_reverb_dac);
    std::thread adc_task(&TaskAudioIO::adc_task, audio_io, &fifo_adc_rev);
    std::thread dac_task(&TaskAudioIO::dac_task, audio_io, &fifo_mix_dac);

    std::thread hyper_edge_task(&SchrodingersReverb::hyper_edge_task, &reverb, &fifo_adc_rev, &fifo_rev_comb1, &fifo_rev_comb2, &fifo_rev_comb3, &fifo_rev_comb4, &fifo_rev_dry);
    std::thread comb1_task(&SchrodingersReverb::comb1_task, &reverb, &fifo_rev_comb1, &fifo_comb1_sum);
    std::thread comb2_task(&SchrodingersReverb::comb2_task, &reverb, &fifo_rev_comb2, &fifo_comb2_sum);
    std::thread comb3_task(&SchrodingersReverb::comb3_task, &reverb, &fifo_rev_comb3, &fifo_comb3_sum);
    std::thread comb4_task(&SchrodingersReverb::comb4_task, &reverb, &fifo_rev_comb4, &fifo_comb4_sum);
    std::thread sum_task(&SchrodingersReverb::sum_task, &reverb, &fifo_comb1_sum, &fifo_comb2_sum, &fifo_comb3_sum, &fifo_comb4_sum, &fifo_sum_ap1);
    std::thread allpass1_task(&SchrodingersReverb::allpass1_task, &reverb, &fifo_sum_ap1, &fifo_ap1_ap2);
    std::thread allpass2_task(&SchrodingersReverb::allpass2_task, &reverb, &fifo_ap1_ap2, &fifo_ap2_ap3);
    std::thread allpass3_task(&SchrodingersReverb::allpass3_task, &reverb, &fifo_ap2_ap3, &fifo_ap3_mix);
    std::thread mix_task(&SchrodingersReverb::dry_wet_mix_task, &reverb, &fifo_rev_dry, &fifo_ap3_mix, &fifo_mix_dac);

    std::thread user_input_task(user_input, &reverb);

    adc_task.join();
    dac_task.join();

    hyper_edge_task.join();
    comb1_task.join();
    comb2_task.join();
    comb3_task.join();
    comb4_task.join();
    sum_task.join();
    allpass1_task.join();
    allpass2_task.join();
    allpass3_task.join();
    mix_task.join();

    user_input_task.join();

    return 0;
}
