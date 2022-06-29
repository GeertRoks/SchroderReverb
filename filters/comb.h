#ifndef COMB_H
#define COMB_H

class Comb {
public:
  Comb ();
  Comb (unsigned int z, double g);
  virtual ~Comb ();

  float process(float x);
  void process(float x, float* result);
  void  process_fifo(float* comb_in, float* comb_out, unsigned short buffersize = 1);
  void reset();

private:
  void tick();
  void updateBuffer(float x);

  double g = 0.2;             //feedback gain
  unsigned int z = 8;         //amount of delay samples
  float y = 0.0f;             //intermediate result

  float *filterBuffer;
  unsigned int index = 0;
};//class

#endif
