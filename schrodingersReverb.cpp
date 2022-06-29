
#include "schrodingersReverb.h"

SchrodingersReverb::SchrodingersReverb(unsigned short buffersize, int reverbTime) {
  this->reverbTime = reverbTime;

  this->buffersize = buffersize;
  buffer_in = new float[buffersize];
  buffer_sum = new float[buffersize];
  buffer_dry = new float[buffersize];
  buffer_wet = new float[buffersize];
  buffer_mix = new float[buffersize];
  buffer_out = new float[buffersize];

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

void  SchrodingersReverb::process_single_task(std::queue<float>* input, std::queue<float>* output) {
    for (i = 0; i < buffersize; i++) {
        buffer_in[i] = input->front();
        input->pop();
    }
    for (i = 0; i < buffersize; i++) {
        buffer_sum[i] = ( comb1.process(buffer_in[i]) + comb2.process(buffer_in[i]) + comb3.process(buffer_in[i]) + comb4.process(buffer_in[i]) ) * 0.25;
        buffer_out[i] = allpass1.process(buffer_sum[i]);
        buffer_out[i] = allpass2.process(buffer_out[i]);
        buffer_out[i] = allpass3.process(buffer_out[i]);
    }
    for (i = 0; i < buffersize; i++) {
        buffer_out[i] = (dry_wet_mix * buffer_out[i]) + ((1.0f - dry_wet_mix) * buffer_in[i]);
    }
    for (i = 0; i < buffersize; i++) {
        output->push(buffer_out[i]);
    }
}

void SchrodingersReverb::process_multi_task(std::queue<float>* input, std::queue<float>* output) {

  thread_hyper_edge = std::thread(&SchrodingersReverb::fill_hyper_edge_fifos, this, input, &fifo_rev_comb1, &fifo_rev_comb2, &fifo_rev_comb3, &fifo_rev_comb4, &fifo_rev_dry, buffersize);
  thread_hyper_edge.join();

  thread_comb1= std::thread(&Comb::process_fifo, comb1, &fifo_rev_comb1, &fifo_comb1_sum, buffersize);
  thread_comb2= std::thread(&Comb::process_fifo, comb2, &fifo_rev_comb2, &fifo_comb2_sum, buffersize);
  thread_comb3= std::thread(&Comb::process_fifo, comb3, &fifo_rev_comb3, &fifo_comb3_sum, buffersize);
  thread_comb4= std::thread(&Comb::process_fifo, comb4, &fifo_rev_comb4, &fifo_comb4_sum, buffersize);

  thread_comb1.join();
  thread_comb2.join();
  thread_comb3.join();
  thread_comb4.join();

  sum(&fifo_comb1_sum, &fifo_comb2_sum, &fifo_comb3_sum, &fifo_comb4_sum, &fifo_sum_ap1, buffersize);
  allpass1.process_fifo(&fifo_sum_ap1, &fifo_ap1_ap2, buffersize);
  allpass2.process_fifo(&fifo_ap1_ap2, &fifo_ap2_ap3, buffersize);
  allpass3.process_fifo(&fifo_ap2_ap3, &fifo_ap3_wet, buffersize);
  drywetmix(&fifo_rev_dry, &fifo_ap3_wet, output, buffersize);
}

void SchrodingersReverb::drywetmix(std::queue<float>* dry_in, std::queue<float>* wet_in, std::queue<float>* out, unsigned short buffersize) {
    for (i = 0; i < buffersize; i++) {
        buffer_dry[i] = dry_in->front();
        dry_in->pop();
    }
    for (i = 0; i < buffersize; i++) {
        buffer_wet[i] = wet_in->front();
        wet_in->pop();
    }
    for (i = 0; i < buffersize; i++) {
        buffer_mix[i] = (dry_wet_mix * buffer_wet[i]) + ((1.0f - dry_wet_mix) * buffer_dry[i]);
    }
    for (i = 0; i < buffersize; i++) {
        out->push(buffer_mix[i]);
    }
}

void SchrodingersReverb::sum(std::queue<float>* sum_in1, std::queue<float>* sum_in2, std::queue<float>* sum_in3, std::queue<float>* sum_in4, std::queue<float>* sum_out, unsigned short buffersize) {
    for (unsigned short i = 0; i < buffersize; i++) {
        sum_result = sum_in1->front() + sum_in2->front() + sum_in3->front() + sum_in4->front();
        sum_result = sum_result * 0.25f;
        sum_out->push(sum_result);

        sum_in1->pop();
        sum_in2->pop();
        sum_in3->pop();
        sum_in4->pop();
    }
}

void SchrodingersReverb::sum_intern() {
  sum_result = fifo_comb1_sum.front() + fifo_comb2_sum.front() + fifo_comb3_sum.front() + fifo_comb4_sum.front();
  sum_result = sum_result * 0.25f;
  fifo_sum_ap1.push(sum_result);

  fifo_comb1_sum.pop();
  fifo_comb2_sum.pop();
  fifo_comb3_sum.pop();
  fifo_comb4_sum.pop();
}

void SchrodingersReverb::fill_hyper_edge_fifos(std::queue<float>* edge_in, std::queue<float>* edge1, std::queue<float>* edge2, std::queue<float>* edge3, std::queue<float>* edge4, std::queue<float>* dry,  unsigned short buffersize) {
  for (unsigned short i = 0; i < buffersize; i++) {
    buffer_in[i] = edge_in->front();
    edge_in->pop();
  }
  for (int i = 0; i < buffersize; i++) {
    edge1->push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    edge2->push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    edge3->push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    edge4->push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    dry->push(buffer_in[i]);
  }
}
void SchrodingersReverb::fill_hyper_edge_fifos_intern(std::queue<float>* input) {
  for (unsigned short i = 0; i < buffersize; i++) {
    buffer_in[i] = input->front();
    input->pop();
  }
  for (int i = 0; i < buffersize; i++) {
    fifo_rev_comb1.push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    fifo_rev_comb2.push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    fifo_rev_comb3.push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    fifo_rev_comb4.push(buffer_in[i]);
  }
  for (int i = 0; i < buffersize; i++) {
    fifo_rev_dry.push(buffer_in[i]);
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
