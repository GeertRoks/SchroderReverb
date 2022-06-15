#pragma once

#include <chrono>
#include <thread>
#include <queue>
#include <bcm2835.h>

#include "MCP3204_BCM2835/MCP3204_BCM2835.h"

class TaskAudioIO {
    public:
        TaskAudioIO(); //default: buffersize = 64, period =  40us (fs=25KHz)
        TaskAudioIO(int buffersize, int period_us);
        ~TaskAudioIO();

        void run(std::queue<int>* fifo_adc_out, std::queue<int>* fifo_dac_in);
        void run_debug(std::queue<int>* fifo_adc_out, std::queue<int>* fifo_dac_in, std::chrono::nanoseconds* duration, int counter); //debug variant

    private:
        void pwm_init();

        MCP3204 adc;
        const std::chrono::microseconds period;
        const int buffersize;

};
