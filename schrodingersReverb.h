#ifndef SCHRODINGERSREVERB_H_
#define SCHRODINGERSREVERB_H_

#include <iostream>
#include <queue>
#include <thread>

#include "filters/allpassDFII.h"
#include "filters/comb.h"

class SchrodingersReverb {

public:
  SchrodingersReverb(unsigned short buffersize, int reverbTime);
  ~SchrodingersReverb();

  float process(float x);
  void  process_single_task(std::queue<float>* input, std::queue<float>* output);
  void  process_multi(std::queue<float>* input, std::queue<float>* output);
  void reset();

  void sum(std::queue<float>* sum_in1, std::queue<float>* sum_in2, std::queue<float>* sum_in3, std::queue<float>* sum_in4, std::queue<float>* sum_out, unsigned short buffersize = 1);
  void fill_hyper_edge_fifos(std::queue<float>* edge_in, std::queue<float>* edge1, std::queue<float>* edge2, std::queue<float>* edge3, std::queue<float>* edge4, unsigned short buffersize = 1);


  void setDryWetMix(float mix);

private:
  void fill_hyper_edge_fifos_intern(std::queue<float>* input);
  void sum_intern();

  int reverbTime = 0; //ms ???
  uint16_t buffersize = 1;

  AllpassDFII allpass1 = AllpassDFII(125, 0.7);
  AllpassDFII allpass2 = AllpassDFII(42, 0.7);
  AllpassDFII allpass3 = AllpassDFII(12, 0.7);

  Comb comb1 = Comb(901, 0.805);
  Comb comb2 = Comb(778, 0.827);
  Comb comb3 = Comb(1011, 0.783);
  Comb comb4 = Comb(1123, 0.764);

  std::queue<float> fifo_rev_comb1, fifo_rev_comb2, fifo_rev_comb3, fifo_rev_comb4;
  std::queue<float> fifo_comb1_sum, fifo_comb2_sum, fifo_comb3_sum, fifo_comb4_sum;
  std::queue<float> fifo_sum_ap1, fifo_ap1_ap2, fifo_ap2_ap3, fifo_ap3_rev;

  float sum_result, y = 0.0f;

  std::thread thread_hyper_edge;
  std::thread thread_comb1;
  std::thread thread_comb2;
  std::thread thread_comb3;
  std::thread thread_comb4;
  std::thread thread_sum;
  std::thread thread_allpass1;
  std::thread thread_allpass2;
  std::thread thread_allpass3;

  float* buffer_in;
  float* buffer_sum;
  float* buffer_out;
  unsigned short i = 0;

  float dry_wet_mix = 0.9f; //percentage of wet sound

};//class

#endif//SCHRODINGERSREVERB_H_

/**
  TODO:
    - array of filter pointers.
    - parameter control.
    - Combine ticks.
    - Combine fillbuffer.

*/
