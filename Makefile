CXXFLAGS := -Wall -std=c++11
LDFLAGS =
LDLIBS = -lpthread -lbcm2835
PROGRAM_NAME = schroederreverb


ADC_OBJ = adc/task_audio_io.o adc/MCP3204_BCM2835/MCP3204_BCM2835.o
REV_OBJ = schrodingersReverb.o filters/allpassDFII.o filters/comb.o
OBJ = main.o
OBJ += $(REV_OBJ) $(ADC_OBJ)

all: test

test: $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LDLIBS)

adc_test: adc/rpi_mcp3204_test.o $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $< $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

queue_perf_test: random_tests/queue_perf.o
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS)

comb_perf_test: random_tests/comb_perf.o
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS)

allpass_perf_test: random_tests/allpass_perf.o
	$(CXX) -o $@ $(CXXFLAGS) $< $(LDFLAGS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
	rm $(PROGRAM_NAME)
	rm $(ADC_OBJ)
	rm adc_test

.PHONY: clean all
