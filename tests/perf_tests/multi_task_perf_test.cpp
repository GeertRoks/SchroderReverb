#include <iostream>
#include <chrono>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <math.h>
#include <stdlib.h>

#include "../../src/schrodingersReverb.h"
#include "../../src/rts_buffer.h"

std::mutex mu;
std::condition_variable condition;
short run_state = 2;

bool fire = false;
bool fired = false;

void producer(RTS_Buffer<float>* output, std::size_t* buffersize, std::chrono::time_point<std::chrono::steady_clock>* start) {
  float* input = new float[*buffersize];
  input[0] = 1.0f;
  for (unsigned int i = 0; i < *buffersize; i++) {
    input[i] = 0.0f;
  }
  while (1) {
    if(fire && !fired) {
      while (output->write(input)) {}
      //std::cout << "producer(): new block to reverb, run_state: " << run_state << std::endl;
      //std::unique_lock<std::mutex> locker(mu);
      //run_state = 1;
      //locker.unlock();
      *start = std::chrono::steady_clock::now();
      fired = true;
      //std::this_thread::sleep_for(std::chrono::microseconds(5120));
    } else {
      //std::cout << "producer(): waiting to output" << std::endl;
    }
  }
}
void consumer(RTS_Buffer<float>* input, std::size_t* buffersize, std::chrono::time_point<std::chrono::steady_clock>* stop) {
  float* output = new float[*buffersize];
  while (1) {
    input->read(output);
    *stop = std::chrono::steady_clock::now();
    fire = false;
    fired = false;
    //std::unique_lock<std::mutex> locker(mu);
    //run_state = 0;
    //locker.unlock();
    //condition.notify_one();
  }
}

int main(int argc, char* argv[])
{
  std::size_t buffersize = 1;
    if (argc > 1) {
        buffersize = atoi(argv[1]);
        std::cout << "buffersize set to: " << buffersize << std::endl;
    } else {
        std::cout << "no buffer size given. Usage: " << argv[0] << " <int buffersize>" << std::endl;
        return 1;
    }

    SchrodingersReverb reverb(buffersize, 0);
    //reverb.setSingleRunMode();
    reverb.setDryWetMix(1.0f);

    std::chrono::time_point<std::chrono::steady_clock> start;
    std::chrono::time_point<std::chrono::steady_clock> stop;
    std::chrono::nanoseconds duration;

    const int loop_amount = 100000;
    int count = loop_amount;

    unsigned long long int push_avg = 0;
    unsigned int push_max = 0;
    unsigned int push_min = 0 - 1;
    unsigned int count_low = 0;
    unsigned int count_high = 0;
    unsigned int count_missed_deadline = 0;

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

    std::thread adc_task(producer, &fifo_adc_rev, &buffersize, &start);
    std::thread dac_task(consumer, &fifo_mix_dac, &buffersize, &stop);

    std::cout << "Multi task perf Test: Using loop_amount: " << loop_amount << std::endl;

    std::cout << "[" << std::flush;
    while (count > 0) {
        if (!(count % 1000)) {
          std::cout << "#" << std::flush;
        }

        fire = true;
        while (fire) {}

        //std::unique_lock<std::mutex> locker(mu);
        //condition.wait(locker, [](){ return run_state == 0; });
        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop-start);
        //std::cout << "run: " << loop_amount - count << ", duration: " << duration.count() << std::endl;
        //run_state = 2;
        //locker.unlock();

        if (duration.count() < 20000) {
          count_low++;
        }
        if (duration.count() > 80000) {
          count_high++;
        }
        if (duration.count() > 5120000) {
          count_missed_deadline++;
        }

        push_avg += duration.count();
        if (duration.count() > push_max) {
            push_max = duration.count();
        }
        if (duration.count() < push_min) {
            push_min = duration.count();
        }

        count --;
    }
    std::cout << "]" << std::endl;

    std::cout << "avg runtime: " << push_avg/loop_amount << " ns, per sample: " << push_avg/(loop_amount*buffersize) << " ns" << std::endl;
    std::cout << "min runtime: " << push_min << " ns, per sample: " << push_min/buffersize << " ns, " << count_low << " runs were shorter than 20000 ns" << std::endl;
    std::cout << "max runtime: " << push_max << " ns, per sample: " << push_max/buffersize << " ns, " << count_high << " runs were longer than 80000 ns" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << count_missed_deadline << " out of " << loop_amount << " missed their deadline. That is " << (100.0f * count_missed_deadline/(float)loop_amount) << "%" << std::endl;

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

    return 0;
}

