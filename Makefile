CXXFLAGS := -Wall -std=c++11 -g
LDFLAGS =
LDLIBS = -lpthread -lbcm2835 -lm
PROGRAM_NAME = schroederreverb


ADC_OBJ = adc/task_audio_io.o adc/MCP3204_BCM2835/MCP3204_BCM2835.o
REV_OBJ = schrodingersReverb.o filters/allpassDFII.o filters/comb.o
OBJ = single_task_main.o
OBJ += $(REV_OBJ) $(ADC_OBJ)

all: reverb_single_task reverb_multi_task

reverb_single_task: single_task_main.o $(REV_OBJ) $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) single_task_main.o $(REV_OBJ) $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

reverb_multi_task: multi_task_main.o $(REV_OBJ) $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) multi_task_main.o $(REV_OBJ) $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

$(PROGRAM_NAME): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LDLIBS)

adc_test: adc/rpi_mcp3204_test.o $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $< $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

queue_perf_test: random_tests/queue_perf.o
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS)

rev_multi_thread_test: random_tests/rev_multi_thread_speed.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $< $(REV_OBJ) $(LDFLAGS) -lpthread

comb_perf_test: random_tests/comb_perf.o filters/comb.o
	$(CXX) -o $@ $(CXXFLAGS) filters/comb.o $< $(LDFLAGS) $(LDLIBS)

allpass_perf_test: random_tests/allpass_perf.o filters/allpassDFII.o
	$(CXX) -o $@ $(CXXFLAGS) filters/allpassDFII.o $< $(LDFLAGS) $(LDLIBS)

sum_perf_test: random_tests/sum_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

single_task_perf_test: random_tests/single_task_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
	rm $(PROGRAM_NAME)
	rm $(ADC_OBJ)
	rm adc_test

.PHONY: clean all
