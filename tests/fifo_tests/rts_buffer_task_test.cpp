#include <iostream>
#include <assert.h>
#include <thread>

#include "../../src/rts_buffer.h"

void task(RTS_Buffer<float>* input, RTS_Buffer<float>* output, std::size_t* buffersize) {
  float* buffer = new float[*buffersize];
  while (1) {
    input->read(buffer);
    std::cout << "task: read data" << std::endl;
    for (unsigned int i = 0; i < *buffersize; i++) {
      assert(buffer[i] == i);
      buffer[i] = buffer[i] + 2;
    }
    while(!output->write(buffer)) {
      //std::cout << "task: trying to write to output" << std::endl;
    }
  }
}

void producer(RTS_Buffer<float>* output, std::size_t* buffersize) {
  float* input = new float[*buffersize];
  for (unsigned int i = 0; i < *buffersize; i++) {
    input[i] = i;
  }
  while (1) {
    if(output->write(input)) {
      std::cout << "producer: data send" << std::endl;
      std::this_thread::sleep_for(std::chrono::seconds(2));
    }
  }
}

void consumer(RTS_Buffer<float>* input, std::size_t* buffersize) {
  float* output = new float[*buffersize];
  while (1) {
    input->read(output);
    std::cout << "consumer: data received" << std::endl;
  for (unsigned int i = 0; i < *buffersize; i++) {
      assert(output[i] == i + 2);
    }
  }
}


int main() {

  std::size_t buffersize = 128;

  RTS_Buffer<float> fifo_in(buffersize);
  RTS_Buffer<float> fifo_out(buffersize);

  std::thread producer_thread(producer, &fifo_in, &buffersize);
  std::thread task_thread(task, &fifo_in, &fifo_out, &buffersize);
  std::thread consumer_thread(consumer, &fifo_out, &buffersize);

  producer_thread.join();
  task_thread.join();
  consumer_thread.join();

  return 0;
}

