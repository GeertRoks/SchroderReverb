
#include "schrodingersReverb.h"

SchrodingersReverb::SchrodingersReverb(int reverbTime) {
  this->reverbTime = reverbTime;

}//SchrodingersReverb()

SchrodingersReverb::~SchrodingersReverb() {

}//~SchrodingersReverb()


float SchrodingersReverb::process(float x) {
//Reference: https://www.dsprelated.com/freebooks/pasp/Schroeder_Reverberators.html
  float y = (comb1.process(x) + comb2.process(x) + comb3.process(x) + comb3.process(x)) * 0.25;
  y = allpass1.process(y);
  y = allpass2.process(y);
  y = allpass3.process(y);
  return y;
}//process()

void SchrodingersReverb::process(std::queue<float>* input, std::queue<float>* output, unsigned short buffersize) {

  fill_hyper_edge_fifos(input, buffersize);

  thread_comb1= std::thread(&Comb::process_fifo, comb1, &fifo_rev_comb1, &fifo_comb1_sum, buffersize);
  thread_comb2= std::thread(&Comb::process_fifo, comb2, &fifo_rev_comb2, &fifo_comb2_sum, buffersize);
  thread_comb3= std::thread(&Comb::process_fifo, comb3, &fifo_rev_comb3, &fifo_comb3_sum, buffersize);
  thread_comb4= std::thread(&Comb::process_fifo, comb4, &fifo_rev_comb4, &fifo_comb4_sum, buffersize);

  //comb1.process_fifo(&fifo_rev_comb1, &fifo_comb1_sum);
  //comb2.process_fifo(&fifo_rev_comb2, &fifo_comb2_sum);
  //comb3.process_fifo(&fifo_rev_comb3, &fifo_comb3_sum);
  //comb4.process_fifo(&fifo_rev_comb4, &fifo_comb4_sum);

  thread_comb1.join();
  thread_comb2.join();
  thread_comb3.join();
  thread_comb4.join();

  for (int i = 0; i < buffersize; i++) {
    sum(&fifo_comb1_sum, &fifo_comb2_sum, &fifo_comb3_sum, &fifo_comb4_sum, &fifo_sum_ap1, buffersize);

    allpass1.process(&fifo_sum_ap1, &fifo_ap1_ap2);
    allpass2.process(&fifo_ap1_ap2, &fifo_ap2_ap3);
    allpass3.process(&fifo_ap2_ap3, &fifo_ap3_rev);

    output->push(fifo_ap3_rev.front());
    fifo_ap3_rev.pop();
  }
}

void SchrodingersReverb::sum(std::queue<float>* sum_in1, std::queue<float>* sum_in2, std::queue<float>* sum_in3, std::queue<float>* sum_in4, std::queue<float>* sum_out, unsigned short buffersize) {
  sum_result += sum_in1->front();
  sum_result += sum_in2->front();
  sum_result += sum_in3->front();
  sum_result += sum_in4->front();
  sum_result = sum_result * 0.25f;
  sum_out->push(sum_result);

  sum_in1->pop();
  sum_in2->pop();
  sum_in3->pop();
  sum_in4->pop();

  sum_result = 0.0f;
}

void SchrodingersReverb::fill_hyper_edge_fifos(std::queue<float>* hyper_edge_in, unsigned short buffersize) {
  for (unsigned short i = 0; i < buffersize; i++) {
    in_hyper_edge = hyper_edge_in->front();
    hyper_edge_in->pop();

    fifo_rev_comb1.push(in_hyper_edge);
    fifo_rev_comb2.push(in_hyper_edge);
    fifo_rev_comb3.push(in_hyper_edge);
    fifo_rev_comb4.push(in_hyper_edge);
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
