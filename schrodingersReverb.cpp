
#include "schrodingersReverb.h"

SchrodingersReverb::SchrodingersReverb(unsigned short buffersize, int reverbTime) {
  this->reverbTime = reverbTime;

  this->buffersize = buffersize;
  buffer_intern_single_task = new float[buffersize];
  buffer_sum_ap_combo = new float[buffersize];
  fifo_rev_comb1 = new float[buffersize];
  fifo_rev_comb2 = new float[buffersize];
  fifo_rev_comb3 = new float[buffersize];
  fifo_rev_comb4 = new float[buffersize];
  fifo_rev_dry = new float[buffersize];
  fifo_comb1_sum = new float[buffersize];
  fifo_comb2_sum = new float[buffersize];
  fifo_comb3_sum = new float[buffersize];
  fifo_comb4_sum = new float[buffersize];
  fifo_sum_ap1 = new float[buffersize];
  fifo_ap1_ap2 = new float[buffersize];
  fifo_ap2_ap3 = new float[buffersize];
  fifo_ap3_wet = new float[buffersize];

}//SchrodingersReverb()

SchrodingersReverb::~SchrodingersReverb() {
}//~SchrodingersReverb()


float SchrodingersReverb::process(float x) {
//Reference: https://www.dsprelated.com/freebooks/pasp/Schroeder_Reverberators.html
    y = ( comb1.process(x) + comb2.process(x) + comb3.process(x) + comb4.process(x) ) * 0.25;
    y = allpass1.process(y);
    y = allpass2.process(y);
    y = allpass3.process(y);
    return 0.0;
}

void  SchrodingersReverb::process_single_task(float* input, float* output) {
    for (i = 0; i < buffersize; i++) {
        buffer_intern_single_task[i] = ( comb1.process(input[i]) + comb2.process(input[i]) + comb3.process(input[i]) + comb4.process(input[i]) ) * 0.25;
        buffer_intern_single_task[i] = allpass1.process(buffer_intern_single_task[i]);
        buffer_intern_single_task[i] = allpass2.process(buffer_intern_single_task[i]);
        buffer_intern_single_task[i] = allpass3.process(buffer_intern_single_task[i]);
    }
    for (i = 0; i < buffersize; i++) {
        output[i] = (dry_wet_mix * buffer_intern_single_task[i]) + ((1.0f - dry_wet_mix) * input[i]);
    }
}

void SchrodingersReverb::process_multi_task(float* input, float* output) {

  thread_hyper_edge = std::thread(&SchrodingersReverb::fill_hyper_edge_fifos, this, input, fifo_rev_comb1, fifo_rev_comb2, fifo_rev_comb3, fifo_rev_comb4, fifo_rev_dry, buffersize);
  thread_hyper_edge.join();

  thread_comb1= std::thread(&Comb::process_fifo, comb1, fifo_rev_comb1, fifo_comb1_sum, buffersize);
  thread_comb2= std::thread(&Comb::process_fifo, comb2, fifo_rev_comb2, fifo_comb2_sum, buffersize);
  thread_comb3= std::thread(&Comb::process_fifo, comb3, fifo_rev_comb3, fifo_comb3_sum, buffersize);
  thread_comb4= std::thread(&Comb::process_fifo, comb4, fifo_rev_comb4, fifo_comb4_sum, buffersize);

  thread_comb1.join();
  thread_comb2.join();
  thread_comb3.join();
  thread_comb4.join();

  sum(fifo_comb1_sum, fifo_comb2_sum, fifo_comb3_sum, fifo_comb4_sum, fifo_sum_ap1, buffersize);
  allpass1.process_fifo(fifo_sum_ap1, fifo_ap1_ap2, buffersize);
  allpass2.process_fifo(fifo_ap1_ap2, fifo_ap2_ap3, buffersize);
  allpass3.process_fifo(fifo_ap2_ap3, fifo_ap3_wet, buffersize);
  drywetmix(fifo_rev_dry, fifo_ap3_wet, output, buffersize);
}

void SchrodingersReverb::drywetmix(float* dry_in, float* wet_in, float* mix_out, unsigned short buffersize) {
    for (i = 0; i < buffersize; i++) {
        mix_out[i] = (dry_wet_mix * wet_in[i]) + ((1.0f - dry_wet_mix) * dry_in[i]);
    }
}

void SchrodingersReverb::sum(float* sum_in1, float* sum_in2, float* sum_in3, float* sum_in4, float* sum_out, unsigned short buffersize) {
    for (i = 0; i < buffersize; i++) {
        sum_out[i] = (sum_in1[i] + sum_in2[i] + sum_in3[i] + sum_in4[i]) * 0.25f;
    }
}
void SchrodingersReverb::sum_ap_combo(float* sum_in1, float* sum_in2, float* sum_in3, float* sum_in4, float* ap_out, unsigned short buffersize) {
    for (i = 0; i < buffersize; i++) {
        buffer_sum_ap_combo[i] = (sum_in1[i] + sum_in2[i] + sum_in3[i] + sum_in4[i]) * 0.25f;
    }
    allpass1.process_fifo(buffer_sum_ap_combo, buffer_sum_ap_combo, buffersize);
    allpass2.process_fifo(buffer_sum_ap_combo, buffer_sum_ap_combo, buffersize);
    allpass3.process_fifo(buffer_sum_ap_combo, ap_out, buffersize);
}

void SchrodingersReverb::sum_intern() {
    for (i = 0; i < buffersize; i++) {
        fifo_sum_ap1[i] = (fifo_comb1_sum[i] + fifo_comb2_sum[i] + fifo_comb3_sum[i] + fifo_comb4_sum[i]) * 0.25f;
    }
}

void SchrodingersReverb::fill_hyper_edge_fifos(float* edge_in, float* edge1, float* edge2, float* edge3, float* edge4, float* dry, unsigned short buffersize) {
  for (i = 0; i < buffersize; i++) {
    edge1[i] = edge_in[i];
    edge2[i] = edge_in[i];
    edge3[i] = edge_in[i];
    edge4[i] = edge_in[i];
    dry[i] = edge_in[i];
  }
}
void SchrodingersReverb::fill_hyper_edge_fifos_intern(float* input) {
  for (i = 0; i < buffersize; i++) {
    fifo_rev_comb1[i] = input[i];
    fifo_rev_comb2[i] = input[i];
    fifo_rev_comb3[i] = input[i];
    fifo_rev_comb4[i] = input[i];
    fifo_rev_dry[i] = input[i];
  }
}

void SchrodingersReverb::reset() {
  comb1.reset();
  comb2.reset();
  comb3.reset();
  comb4.reset();

  allpass1.reset();
  allpass2.reset();
  allpass3.reset();
}
void SchrodingersReverb::setDryWetMix(float mix) {
    if ( mix > 1.0f ) {
        dry_wet_mix = 1.0f;
    } else if (mix < 0.0f) {
        dry_wet_mix = 0.0f;
    } else {
        dry_wet_mix = mix;
    }
}
