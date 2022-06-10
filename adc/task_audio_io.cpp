#include "task_audio_io.h"
#include <iostream>

TaskAudioIO::TaskAudioIO() {
    adc.begin(SPI_0, BCM2835_SPI_CS0);
}

TaskAudioIO::~TaskAudioIO() {
}

void TaskAudioIO::operator()(std::queue<int>* fifo_adc_out, std::queue<int>* fifo_dac_in, std::chrono::nanoseconds* duration) {
	int counter = 10;
	auto start = std::chrono::steady_clock::now();
	auto stop = std::chrono::steady_clock::now();
	std::chrono::nanoseconds elapsed = stop - start;
	std::chrono::nanoseconds timeToWait = period - elapsed;
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::chrono::time_point<std::chrono::steady_clock> stop_time;
    	start_time = std::chrono::steady_clock::now();
	while (counter > 0) {
		start = std::chrono::steady_clock::now();

		// retrieve sample from adc
		fifo_adc_out->push(this->adc.readADC(CH0_1));

		stop = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

		timeToWait = period - elapsed;
		// if time is left in the period then wait until the period is over
		if (timeToWait > std::chrono::microseconds::zero()) {
			//std::cout << "time cycle: " << counter-10 << ", time to wait: " << timeToWait.count() << " nanoseconds" <<std::endl;
			std::this_thread::sleep_for(timeToWait);
		}
		counter--;
	}
    	stop_time = std::chrono::steady_clock::now();
   	*duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

}

