#ifndef SCHRODINGERSREVERB_H_
#define SCHRODINGERSREVERB_H_

#include <iostream>
#include <thread>
#include <atomic>

#include "filters/allpassDFII.h"
#include "filters/comb.h"
#include "rts_buffer.h"

class SchrodingersReverb {

public:
  SchrodingersReverb(unsigned short buffersize, int reverbTime);
  ~SchrodingersReverb();

  float process(float x);
  void  process_single_task(float* input, float* output);
  void  process_multi_task(float* input, float* output);

  // -- tasks --
  void  single_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  hyper_edge_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  comb1_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  comb2_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  comb3_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  comb4_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  sum_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  allpass1_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  allpass2_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);
  //void  allpass3_task(RTS_Buffer<float>* input, RTS_Buffer<float>* output);

  void sum(float* sum_in1, float* sum_in2, float* sum_in3, float* sum_in4, float* sum_out, unsigned short buffersize = 1);
  void sum_ap_combo(float* sum_in1, float* sum_in2, float* sum_in3, float* sum_in4, float* ap_out, unsigned short buffersize = 1);
  void fill_hyper_edge_fifos(float* edge_in, float* edge1, float* edge2, float* edge3, float* edge4, float* dry, unsigned short buffersize = 1);


  void drywetmix(float* dry_in, float* wet_in, float* mix_out, unsigned short buffersize);
  void setDryWetMix(float mix);
  void reset();

private:
  void fill_hyper_edge_fifos_intern(float* input);
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

  float* buffer_intern_single_task;
  float* buffer_sum_ap_combo;

  float* fifo_rev_comb1;
  float* fifo_rev_comb2;
  float* fifo_rev_comb3;
  float* fifo_rev_comb4;
  float* fifo_rev_dry;
  float* fifo_comb1_sum;
  float* fifo_comb2_sum;
  float* fifo_comb3_sum;
  float* fifo_comb4_sum;
  float* fifo_sum_ap1;
  float* fifo_ap1_ap2;
  float* fifo_ap2_ap3;
  float* fifo_ap3_wet;

  float y = 0.0f;

  std::thread thread_hyper_edge;
  std::thread thread_comb1;
  std::thread thread_comb2;
  std::thread thread_comb3;
  std::thread thread_comb4;
  std::thread thread_sum;
  std::thread thread_allpass1;
  std::thread thread_allpass2;
  std::thread thread_allpass3;

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
