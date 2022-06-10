#include <chrono>
#include <thread>
#include <queue>

#include "MCP3204_BCM2835/MCP3204_BCM2835.h"

class TaskAudioIO {
    public:
        TaskAudioIO();
        ~TaskAudioIO();

	// object callable: this function can be called directly to the thread, without 
	// first creating a class instance in the main
        void operator()(std::queue<int>* fifo_adc_out, std::queue<int>* fifo_dac_in, std::chrono::nanoseconds* duration);

    private:
        MCP3204 adc;
        const std::chrono::microseconds period = std::chrono::microseconds(50);

};
