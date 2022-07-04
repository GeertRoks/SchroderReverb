#include <iostream>
#include <assert.h>

#include "../../rts_buffer.h"

int main() {

  std::size_t buffersize = 128;

  RTS_Buffer<float> fifo(buffersize);

  float* input = new float[buffersize];
  float* output = new float[buffersize];

  std::cout << "input: ";
  for (unsigned int i = 0; i < buffersize; i++) {
    input[i] = i;
    std::cout << input[i] << ", ";
  }
  std::cout << std::endl;

  std::cout << "output before: ";
  for (unsigned int i = 0; i < buffersize; i++) {
    std::cout << output[i] << ", ";
  }
  std::cout << std::endl;

  assert(fifo.write(input) == 1);
  assert(fifo.write(input) == 0);

  fifo.read(output);

  assert(fifo.write(input) == 1);
  fifo.read(output);

  std::cout << "output after: ";
  for (unsigned int i = 0; i < buffersize; i++) {
    std::cout << output[i] << ", ";
  }
  std::cout << std::endl;

  return 0;
}
