#include "task_audio_io.h"
#include <iostream>

template<std::size_t buffersize>
TaskAudioIO<buffersize>::TaskAudioIO() : period(std::chrono::microseconds(40)) {
    adc.begin(SPI_0, BCM2835_SPI_CS0);
    pwm_init();
}

template<std::size_t buffersize>
TaskAudioIO<buffersize>::TaskAudioIO(int init_period_us) : period(std::chrono::microseconds(init_period_us)) {
    adc.begin(SPI_0, BCM2835_SPI_CS0);
    pwm_init();
}

template<std::size_t buffersize>
TaskAudioIO<buffersize>::~TaskAudioIO() {
    bcm2835_pwm_set_mode(0, 1, 0);
    bcm2835_pwm_set_mode(1, 1, 0);
}

template<std::size_t buffersize>
void TaskAudioIO<buffersize>::run(std::queue<float>* fifo_adc_out, std::queue<float>* fifo_dac_in) {

	auto start = std::chrono::steady_clock::now();
	auto stop = std::chrono::steady_clock::now();
	std::chrono::nanoseconds elapsed = stop - start;
	std::chrono::nanoseconds timeToWait = period - elapsed;

	while (1) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------


        if (idx_buffer_adc >= buffersize) {
            // empty buffer and send to fifo
            for (auto sample : buffer_adc) {
                fifo_adc_out->push(sample);
            }
            idx_buffer_adc = 0;
        }
        // retrieve sample from adc
        tmp_adc = (this->adc.readADC(CH0_1)/2048.0f)-1.0f;
        buffer_adc[idx_buffer_adc++] = tmp_adc;
        
        if (idx_buffer_dac >= buffersize) {
            if (fifo_dac_in->size() >= buffersize) {
                for (unsigned short i = 0; i < buffersize; i++) {
                    buffer_dac[i] = fifo_dac_in->front();
                    fifo_dac_in->pop();
                }
                idx_buffer_dac = 0;
            }
        }
        // send sample to dac
        tmp_dac = (buffer_dac[idx_buffer_dac++] + 1.0f)*2048.0f;
        bcm2835_pwm_set_data(1, tmp_dac & 0x3F);
        bcm2835_pwm_set_data(0, tmp_dac >> 6);

        // -------------^ Do audio stuff ^---------------------

		stop = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
		timeToWait = period - elapsed;

		// if time is left in the period then wait until the period is over
		if (timeToWait > std::chrono::microseconds::zero()) {
            //busy waiting (not giving the thread back to the scheduler)
            while(elapsed < period) {
                elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
            }
		}

	} //while(1)

}


// debug variant of run()
template<std::size_t buffersize>
void TaskAudioIO<buffersize>::run_debug(std::queue<int>* fifo_adc_out, std::queue<int>* fifo_dac_in, std::chrono::nanoseconds* duration, int counter) {
    int count = counter;

	auto start = std::chrono::steady_clock::now();
	auto stop = std::chrono::steady_clock::now();
	std::chrono::nanoseconds elapsed = stop - start;
	std::chrono::nanoseconds timeToWait = period - elapsed;

	std::chrono::time_point<std::chrono::steady_clock> start_time;
	std::chrono::time_point<std::chrono::steady_clock> stop_time;
	std::chrono::time_point<std::chrono::steady_clock> loop_time;

    start_time = std::chrono::steady_clock::now();
	while (count > 0) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------

		// retrieve sample from adc
		fifo_adc_out->push(this->adc.readADC(CH0_1));
        // send sample to dac
        if (!fifo_dac_in->empty()) {
            bcm2835_pwm_set_data(1, fifo_dac_in->front() & 0x3F);
            bcm2835_pwm_set_data(0, fifo_dac_in->front() >> 6);
            fifo_dac_in->pop();
        }

        // -------------^ Do audio stuff ^---------------------

		stop = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);

		timeToWait = period - elapsed;
		// if time is left in the period then wait until the period is over
		if (timeToWait > std::chrono::microseconds::zero()) {
	//		std::cout << "time cycle: " << 10-counter << ", start: " << std::chrono::duration_cast<std::chrono::nanoseconds>(start - start_time).count() << " ns, stop: " << std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start_time).count() << " ns, time to wait: " << timeToWait.count() << " ns, loop_time: " << std::chrono::duration_cast<std::chrono::nanoseconds>(loop_time - start_time).count() << std::endl;

            //busy waiting (not giving the thread back to the scheduler)
            while(elapsed < period) {
                elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
            }
		}
		count--;
		//loop_time = std::chrono::steady_clock::now();
	}
    stop_time = std::chrono::steady_clock::now();
   	*duration = std::chrono::duration_cast<std::chrono::nanoseconds>(stop_time - start_time);

}

template<std::size_t buffersize>
void TaskAudioIO<buffersize>::pwm_init() {
    bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_ALT5); //pwm0
    bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_ALT0); //pwm1
    bcm2835_pwm_set_clock(2); // PWM clock divider: 19.2MHz/ clock_div
    bcm2835_pwm_set_mode(0, 1, 1); //enable pwm0 in marksmode
    bcm2835_pwm_set_mode(1, 1, 1); //enable pwm1 in marksmode
    bcm2835_pwm_set_range(0, 64); // set pwm0 range to 6bit (MSBs)
    bcm2835_pwm_set_range(1, 64); // set pwm1 range to 6bit (LSBs)
}
