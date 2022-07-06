#include <iostream>
#include <assert.h>

#include "../../src/rts_buffer.h"

int main() {

  std::size_t buffersize = 128;

  RTS_Buffer<float> fifo(buffersize, 2);

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
  assert(fifo.write(input) == 1);
  assert(fifo.write(input) == 0);

  std::cout << "pre read 1" << std::endl;
  fifo.read(output);
  std::cout << "post read 1" << std::endl;

  assert(fifo.write(input) == 1);
  std::cout << "pre read 2" << std::endl;
  fifo.read(output);
  std::cout << "post read 2" << std::endl;
  std::cout << "pre read 3" << std::endl;
  fifo.read(output);
  std::cout << "post read 3" << std::endl;

  std::cout << "output after: ";
  for (unsigned int i = 0; i < buffersize; i++) {
    std::cout << output[i] << ", ";
  }
  std::cout << std::endl;

  return 0;
}
