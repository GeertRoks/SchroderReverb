#ifndef COMB_H
#define COMB_H

#include <queue>

class Comb {
public:
  Comb ();
  Comb (unsigned int z, double g);
  virtual ~Comb ();

  float process(float x);
  void  process(std::queue<float>* comb_in, std::queue<float>* comb_out );

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
