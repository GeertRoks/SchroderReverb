#pragma once
#ifndef TASK_AUDIO_IO_H_
#define TASK_AUDIO_IO_H_

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <bcm2835.h>

#include "MCP3204_BCM2835/MCP3204_BCM2835.h"

template<std::size_t buffersize>
class TaskAudioIO {
    public:
        TaskAudioIO(); //default: period =  40us (fs=25KHz)
        TaskAudioIO(int period_us);
        ~TaskAudioIO();

        void run(float* fifo_adc_out, float* fifo_dac_in, std::atomic<bool>* new_audio_block);
        void run_debug(float* fifo_adc_out, float* fifo_dac_in, std::chrono::nanoseconds* duration, int counter); //debug variant

    private:
        void pwm_init();

        MCP3204 adc;
        const std::chrono::microseconds period;
        std::array<float, buffersize> buffer_adc = {};
        std::array<float, buffersize> buffer_dac = {};
        float tmp_adc = 0.0f;
        int tmp_dac = 0.0f;
        unsigned short idx_buffer_adc = 0;
        unsigned short idx_buffer_dac = 0;

};
#endif
