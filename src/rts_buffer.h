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
          buffer = new T[buffersize];
          isEmpty = true;
        }
        RTS_Buffer(std::size_t buffersize, std::size_t capacity) {
          this->buffersize = buffersize;
          this->capacity = capacity;
          buffer = new T[buffersize*capacity];
          isEmpty = true;
        }
        ~RTS_Buffer() {
          delete buffer;
          buffer = nullptr;
        }

        void read(T* buffer_out) {
          std::unique_lock<std::mutex> locker(mu);
          cond.wait(locker, [this](){ return !isEmpty; });

          std::copy(buffer, &buffer[buffersize], buffer_out);
          isEmpty = true;

          locker.unlock();
        }
        bool write(T* buffer_in) {
          if (isEmpty) {
            std::unique_lock<std::mutex> locker(mu);

            std::copy(buffer_in, &buffer_in[buffersize], buffer);
            isEmpty = false;

            locker.unlock();
            cond.notify_one();
            return 1;
          } else {
            cond.notify_one();
            return 0;
          }
        }

        std::mutex* getMutex() {
          return &mu;
        }
        std::condition_variable* getCondition() {
          return &cond;
        }

    private:
        T* buffer;
        std::atomic<bool> isEmpty;

        std::size_t buffersize;
        std::size_t capacity = 1;

        std::mutex mu;
        std::condition_variable cond;

};
#endif
