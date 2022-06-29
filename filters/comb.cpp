#include "comb.h"
#include <iostream>

Comb::Comb(unsigned int z, double g) {

  filterBuffer = new float[z];
  for(unsigned int i = 0; i < z; i++) {
    filterBuffer[i] = 0;
  }//for

this->g = g;
this->z = z;
}//Comb

Comb::~Comb() {
  filterBuffer = nullptr;
  delete filterBuffer;
}//~Comb


float Comb::process(float x) {
  y = x + this->g * filterBuffer[index];
  updateBuffer(y);
  return y;
}

void Comb::process(float x, float* result) {
/*
 *  Calculates the filtered sample
 */
  //x is input (x[n]) and y is output (y[n])
  y = x + this->g * filterBuffer[index];
  updateBuffer(y);
  *result = y;
}//process()

void Comb::process_fifo(float* comb_in, float* comb_out, unsigned short buffersize) {
  for (unsigned short i = 0; i < buffersize; i++) {
    y = comb_in[i] + this->g * filterBuffer[index];
    updateBuffer(y);
    comb_out[i] = y;
  }
}

void Comb::updateBuffer(float input) {
/*
 *  Fill the buffer with the new sample
 */
  filterBuffer[index] = input;
  tick();
}//updateBuffer()

void Comb::tick(){
/*
 *  Increase the index of the buffer
 */
  index++;
  index = (index % this->z);
}//tick()

void Comb::reset() {
  index = 0;
  for(unsigned int i = 0; i < z; i++) {
    filterBuffer[i] = 0;
  }
}
