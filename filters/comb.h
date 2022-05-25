#ifndef COMB_H
#define COMB_H

class Comb {
public:
  Comb ();
  Comb (unsigned int z, double g);
  virtual ~Comb ();

  float process(float x);

private:
  void tick();
  void updateBuffer(float x);

  double g = 0.2;               //feedback gain
  unsigned int z = 8;         //amount of delay samples

  float *filterBuffer;
  unsigned int index = 0;
};//class

#endif
