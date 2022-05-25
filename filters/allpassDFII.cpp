#include "allpassDFII.h"
#include <iostream>

AllpassDFII::AllpassDFII(unsigned int z, double a0) {

  filterBuffer = new float[z];
  for(unsigned int i = 0; i < z; i++) {
    filterBuffer[i] = 0;
  }//for

this->a0 = a0;
this->a1 = -1;
this->b0 = 1;
this->b1 = a0;
this->z = z;
}//AllpassDFII

AllpassDFII::~AllpassDFII() {
  delete filterBuffer;
  filterBuffer = nullptr;
}//~AllpassDFII

float AllpassDFII::process(float x) {
/*
 *  Calculates the filtered sample
 */

  //x is input (x[n]) and y is output (y[n])
  float w = this->b0 * x + this->b1 * filterBuffer[index];
  float y = this->a0 * w + this->a1 * filterBuffer[index];
  updateBuffer(w);
  return y;
}//process()

void AllpassDFII::updateBuffer(float input) {
/*
 *  Fill the buffer with the new sample
 */
  filterBuffer[index] = input;
  tick();
}//updateBuffer()

void AllpassDFII::tick(){
/*
 *  Increase the index of the buffer
 */
  index++;
  index = (index % this->z);
}//tick()
