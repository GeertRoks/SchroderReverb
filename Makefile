CXXFLAGS := -Wall -std=c++11 -g
LDFLAGS =
LDLIBS = -lpthread -lbcm2835 -lm
PROGRAM_NAME = schroederreverb


ADC_OBJ = adc/task_audio_io.o adc/MCP3204_BCM2835/MCP3204_BCM2835.o
REV_OBJ = schrodingersReverb.o filters/allpassDFII.o filters/comb.o
OBJ = single_task_main.o
OBJ += $(REV_OBJ) $(ADC_OBJ)

all: reverb_single_task reverb_multi_task

perf_tests: *_perf_test

tests: perf_tests adc_test single_vs_multi_task_test

reverb_single_task: single_task_main.o $(REV_OBJ) $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) single_task_main.o $(REV_OBJ) $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

reverb_multi_task: multi_task_main.o $(REV_OBJ) $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) multi_task_main.o $(REV_OBJ) $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

$(PROGRAM_NAME): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LDLIBS)

adc_test: adc/rpi_mcp3204_test.o $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $< $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

single_vs_multi_task_test: random_tests/single_vs_multi_task_speed.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $< $(REV_OBJ) $(LDFLAGS) -lpthread

queue_perf_test: random_tests/queue_perf.o
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS)

comb_perf_test: random_tests/comb_perf.o filters/comb.o
	$(CXX) -o $@ $(CXXFLAGS) filters/comb.o $< $(LDFLAGS) $(LDLIBS)

allpass_perf_test: random_tests/allpass_perf.o filters/allpassDFII.o
	$(CXX) -o $@ $(CXXFLAGS) filters/allpassDFII.o $< $(LDFLAGS) $(LDLIBS)

sum_perf_test: random_tests/sum_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

sum_ap_combo_perf_test: random_tests/sum_ap_combo_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

edge_perf_test: random_tests/edge_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

single_task_perf_test: random_tests/single_task_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

multi_task_perf_test: random_tests/multi_task_perf_test.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

multi_task_debug_bench: random_tests/multi_task_debug_bench.o $(REV_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(REV_OBJ) $< $(LDFLAGS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
	rm $(PROGRAM_NAME)
	rm $(ADC_OBJ)
	rm adc_test

.PHONY: clean all
