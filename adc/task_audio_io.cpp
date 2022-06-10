#include "task_audio_io.h"
#include <iostream>

TaskAudioIO::TaskAudioIO() {
    adc.begin(SPI_0, BCM2835_SPI_CS0);

    bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_ALT5);
    bcm2835_pwm_set_clock(4);
    bcm2835_pwm_set_mode(0, 0, 1);
    bcm2835_pwm_set_range(0, 4096);

}

TaskAudioIO::~TaskAudioIO() {
    bcm2835_pwm_set_mode(0, 1, 0);
}

void TaskAudioIO::run(std::queue<int>* fifo_adc_out, std::queue<int>* fifo_dac_in, std::chrono::nanoseconds* duration) {
	int counter = 100000;
	auto start = std::chrono::steady_clock::now();
	auto stop = std::chrono::steady_clock::now();
	std::chrono::nanoseconds elapsed = stop - start;
	std::chrono::nanoseconds timeToWait = period - elapsed;
	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::chrono::time_point<std::chrono::steady_clock> stop_time;
	std::chrono::time_point<std::chrono::steady_clock> loop_time;
    	start_time = std::chrono::steady_clock::now();
	while (counter > 0) {
		start = std::chrono::steady_clock::now();

		// retrieve sample from adc
		fifo_adc_out->push(this->adc.readADC(CH0_1));
        bcm2835_pwm_set_data(0, fifo_dac_in->front());

		stop = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

		timeToWait = period - elapsed;
		// if time is left in the period then wait until the period is over
		if (timeToWait > std::chrono::microseconds::zero()) {
			//std::cout << "time cycle: " << 10-counter << ", start: " << std::chrono::duration_cast<std::chrono::nanoseconds>(start - start_time).count() << " ns, stop: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_time).count() << " ns, time to wait: " << timeToWait.count() << " ns, loop_time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(loop_time - start_time).count() << std::endl;

            // Give the thread to the scheduler and get it back after the wait time is over (not accurate)
			//std::this_thread::sleep_for(timeToWait);

            // give the thread to the next in line while waiting (can be short or long)
			//do {
            //    std::this_thread::yield();
            //} while (std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - stop) < timeToWait);

            //busy waiting (not giving the thread back to the scheduler)
            while(elapsed < period) {
                elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
                //std::cout << "loop: " << 10-counter << ", elapsed: " << elapsed.count() << std::endl;
            }
		}
		counter--;
		//loop_time = std::chrono::steady_clock::now();
	}
    	stop_time = std::chrono::steady_clock::now();
   	*duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

}

