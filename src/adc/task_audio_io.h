#pragma once
#ifndef TASK_AUDIO_IO_H_
#define TASK_AUDIO_IO_H_

#include <chrono>
#include <thread>
#include <atomic>
#include <iostream>
#include <bcm2835.h>

#include "MCP3204_BCM2835/MCP3204_BCM2835.h"
#include "../rts_buffer.h"

class TaskAudioIO {
    public:
        TaskAudioIO(std::size_t buffersize); //default: period =  40us (fs=25KHz)
        TaskAudioIO(std::size_t buffersize, int period_us);
        ~TaskAudioIO();

        void run(RTS_Buffer<float>* fifo_adc_out, RTS_Buffer<float>* fifo_dac_in);
        void run_debug(RTS_Buffer<float>* fifo_adc_out, RTS_Buffer<float>* fifo_dac_in, std::chrono::nanoseconds* duration, int counter); //debug variant

        void adc_task(RTS_Buffer<float>* fifo_adc_out);
        void adc_task_debug(RTS_Buffer<float>* fifo_adc_out, int counter);
        void dac_task(RTS_Buffer<float>* fifo_dac_in);

    private:
        void pwm_init();

        MCP3204 adc;
        const std::size_t buffersize;
        const std::chrono::microseconds period;

};
#endif
