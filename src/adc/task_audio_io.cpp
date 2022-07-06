#include "task_audio_io.h"

TaskAudioIO::TaskAudioIO(std::size_t _buffersize) : buffersize(_buffersize), period(std::chrono::microseconds(40)) {
    adc.begin(SPI_0, BCM2835_SPI_CS0);
    pwm_init();
}

TaskAudioIO::TaskAudioIO(std::size_t _buffersize, int _period_us) : buffersize(_buffersize), period(std::chrono::microseconds(_period_us)) {
    adc.begin(SPI_0, BCM2835_SPI_CS0);
    pwm_init();
}

TaskAudioIO::~TaskAudioIO() {
    bcm2835_pwm_set_mode(0, 1, 0);
    bcm2835_pwm_set_mode(1, 1, 0);
}

void TaskAudioIO::adc_task(RTS_Buffer<float>* fifo_adc_out) {
    std::chrono::time_point<std::chrono::steady_clock> start, stop;
	std::chrono::nanoseconds elapsed, timeToWait;

    float tmp_adc = 0.0f;
    float* buffer_adc = new float[buffersize]();
    unsigned short idx_buffer_adc = 0;

    while(1) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------
        if (idx_buffer_adc >= buffersize) {
            // empty buffer and send to fifo
            while(!fifo_adc_out->write(buffer_adc)) {};
            idx_buffer_adc = 0;
        }
        // retrieve sample from adc
        tmp_adc = (this->adc.readADC(CH0_1)/2048.0f)-1.0f;
        buffer_adc[idx_buffer_adc++] = tmp_adc;
        // -------------^ Do audio stuff ^---------------------

		stop = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
		timeToWait = period - elapsed;
		// if time is left in the period then wait until the period is over
		if (timeToWait > std::chrono::nanoseconds::zero()) {
            //busy waiting (not giving the thread back to the scheduler)
            while(elapsed < period) {
                elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
            }
		}
    }
}
void TaskAudioIO::adc_task_debug(RTS_Buffer<float>* fifo_adc_out, int counter) {
    std::chrono::time_point<std::chrono::steady_clock> start, stop;
	std::chrono::nanoseconds elapsed, timeToWait;

    float tmp_adc = 0.0f;
    float* buffer_adc = new float[buffersize]();
    unsigned short idx_buffer_adc = 0;

    int count = counter;

    while(count > 0) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------
        if (idx_buffer_adc >= buffersize) {
            // empty buffer and send to fifo
            while(!fifo_adc_out->write(buffer_adc)) {};
            idx_buffer_adc = 0;
        }
        // retrieve sample from adc
        tmp_adc = (this->adc.readADC(CH0_1)/2048.0f)-1.0f;
        buffer_adc[idx_buffer_adc++] = tmp_adc;
        // -------------^ Do audio stuff ^---------------------

		stop = std::chrono::steady_clock::now();
		elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start);
		timeToWait = period - elapsed;
		// if time is left in the period then wait until the period is over
		if (timeToWait > std::chrono::nanoseconds::zero()) {
            //busy waiting (not giving the thread back to the scheduler)
            while(elapsed < period) {
                elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
            }
		}
        count--;
    }
}
void TaskAudioIO::dac_task(RTS_Buffer<float>* fifo_dac_in) {
    std::chrono::time_point<std::chrono::steady_clock> start, stop;
	std::chrono::nanoseconds elapsed, timeToWait;

    int tmp_dac = 0.0f;
    float* buffer_dac = new float[buffersize]();
    unsigned short idx_buffer_dac = 0;

    while(1) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------
        if (idx_buffer_dac >= buffersize) {
            fifo_dac_in->read(buffer_dac);
            idx_buffer_dac = 0;
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
		if (timeToWait > std::chrono::nanoseconds::zero()) {
            //busy waiting (not giving the thread back to the scheduler)
            while(elapsed < period) {
                elapsed = std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::steady_clock::now() - start);
            }
		}
    }
}

void TaskAudioIO::run(RTS_Buffer<float>* fifo_adc_out, RTS_Buffer<float>* fifo_dac_in) {
    std::chrono::time_point<std::chrono::steady_clock> start, stop;
	std::chrono::nanoseconds elapsed, timeToWait;

    float tmp_adc = 0.0f;
    int tmp_dac = 0.0f;
    float* buffer_dac = new float[buffersize]();
    float* buffer_adc = new float[buffersize]();
    unsigned short idx_buffer_adc = 0;
    unsigned short idx_buffer_dac = 0;

	while (1) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------


        if (idx_buffer_adc >= buffersize) {
            // empty buffer and send to fifo
            while(!fifo_adc_out->write(buffer_adc)) {};
            idx_buffer_adc = 0;
        }
        // retrieve sample from adc
        tmp_adc = (this->adc.readADC(CH0_1)/2048.0f)-1.0f;
        buffer_adc[idx_buffer_adc++] = tmp_adc;
        
        if (idx_buffer_dac >= buffersize) {
            fifo_dac_in->read(buffer_dac);
            idx_buffer_dac = 0;
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
void TaskAudioIO::run_debug(RTS_Buffer<float>* fifo_adc_out, RTS_Buffer<float>* fifo_dac_in, std::chrono::nanoseconds* duration, int counter) {
    int count = counter;

    std::chrono::time_point<std::chrono::steady_clock> start, stop;
	std::chrono::nanoseconds elapsed, timeToWait;
	std::chrono::time_point<std::chrono::steady_clock> start_time, stop_time, loop_time;

    float tmp_adc = 0.0f;
    int tmp_dac = 0.0f;
    float* buffer_dac = new float[buffersize]();
    float* buffer_adc = new float[buffersize]();
    unsigned short idx_buffer_adc = 0;
    unsigned short idx_buffer_dac = 0;


    start_time = std::chrono::steady_clock::now();
	while (count > 0) {
		start = std::chrono::steady_clock::now();

        // -------------v Do audio stuff v---------------------

        if (idx_buffer_adc >= buffersize) {
            // empty buffer and send to fifo
            while(!fifo_adc_out->write(buffer_adc)) {}
            idx_buffer_adc = 0;
        }
		// retrieve sample from adc
        tmp_adc = (this->adc.readADC(CH0_1)/2048.0f)-1.0f;
        buffer_adc[idx_buffer_adc++] = tmp_adc;

        if (idx_buffer_dac >= buffersize) {
            fifo_dac_in->read(buffer_dac);
            idx_buffer_dac = 0;
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

void TaskAudioIO::pwm_init() {
    bcm2835_gpio_fsel(18, BCM2835_GPIO_FSEL_ALT5); //pwm0
    bcm2835_gpio_fsel(13, BCM2835_GPIO_FSEL_ALT0); //pwm1
    bcm2835_pwm_set_clock(2); // PWM clock divider: 19.2MHz/ clock_div
    bcm2835_pwm_set_mode(0, 1, 1); //enable pwm0 in marksmode
    bcm2835_pwm_set_mode(1, 1, 1); //enable pwm1 in marksmode
    bcm2835_pwm_set_range(0, 64); // set pwm0 range to 6bit (MSBs)
    bcm2835_pwm_set_range(1, 64); // set pwm1 range to 6bit (LSBs)
}
