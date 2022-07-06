#pragma once
#ifndef RTS_BUFFER_H_
#define RTS_BUFFER_H_

#include <algorithm>
#include <atomic>
#include <mutex>
#include <condition_variable>

template<typename T>
class RTS_Buffer {
    public:
        RTS_Buffer(std::size_t buffersize) {
          this->buffersize = buffersize;
          this->capacity = 1;
          buffers = new T*[capacity];
          for (uint16_t i = 0; i < capacity; i++) {
              buffers[i] = new T[buffersize];
          }
        }
        RTS_Buffer(std::size_t buffersize, std::size_t capacity) {
          this->buffersize = buffersize;
          this->capacity = capacity;
          buffers = new T*[capacity];
          for (uint16_t i = 0; i < capacity; i++) {
              buffers[i] = new T[buffersize];
          }
        }
        ~RTS_Buffer() {
          for (uint16_t i = 0; i < capacity; i++) {
              delete buffers[i];
              buffers[i] = nullptr;
          }
          delete buffers;
          buffers = nullptr;
        }

        void read(T* buffer_out) {
            std::unique_lock<std::mutex> locker(mu);
            if (isEmpty()) { 
                throw std::runtime_error("Buffer is empty");
                cond.wait(locker, [this](){ return !isEmpty(); });
            }
            std::copy(&buffers[read_head%capacity][0], &buffers[read_head%capacity][buffersize], buffer_out);

            if (read_head == capacity) {
                read_head = 1;
            } else {
                read_head++;
            }

            locker.unlock();
        }
        bool write(T* buffer_in) {
            if (isFull()) {
                //throw std::runtime_error("Buffer is full");
                cond.notify_one();
                return 0;
            } else {
                std::unique_lock<std::mutex> locker(mu);

                std::copy(buffer_in, &buffer_in[buffersize], &buffers[write_head%capacity][0]);

                if (write_head == capacity) {
                    write_head = 1;
                } else {
                    write_head++;
                }

                locker.unlock();
                cond.notify_one();
                return 1;
            }
        }

        std::mutex* getMutex() {
          return &mu;
        }
        std::condition_variable* getCondition() {
          return &cond;
        }

        bool isEmpty() {
            return (read_head == write_head);
        }
        bool isFull() {
            return (read_head == ((write_head+1) % (capacity+1)));
        }

    private:
        T** buffers;

        std::size_t buffersize;
        std::size_t capacity = 1;

        std::size_t write_head = 0;
        std::size_t read_head = 0;

        std::mutex mu;
        std::condition_variable cond;

};
#endif
