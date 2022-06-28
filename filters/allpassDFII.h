#ifndef ALLPASSDFII_H
#define ALLPASSDFII_H

#include <queue>

class AllpassDFII {
public:
  AllpassDFII (unsigned int z, double a0);
  virtual ~AllpassDFII ();

  float process(float x);
  void process_fifo(std::queue<float>* ap_in, std::queue<float>* ap_out);
  void process_fifo(std::queue<float>* ap_in, std::queue<float>* ap_out, unsigned short buffersize);
  void reset();

private:
  void tick();
  void updateBuffer(float x);

  double a0, a1, b0, b1 = 0;  //filtercoefficents
  float w, y = 0.0f;          //intermediate results
  unsigned int z = 0;         //amount of delay samples

  float *filterBuffer;
  unsigned int index = 0;

};//class

#endif//ALLPASSDFII_H
