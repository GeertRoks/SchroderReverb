CXXFLAGS := -Wall -std=c++11
LDFLAGS =
LDLIBS = -lpthread -lbcm2835
PROGRAM_NAME = test


OBJ = main.o schrodingersReverb.o filters/allpassDFII.o filters/comb.o
ADC_OBJ = adc/rpi_mcp3204_test.o adc/task_audio_io.o adc/MCP3204_BCM2835/MCP3204_BCM2835.o

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LDLIBS)

adc_test: $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(ADC_OBJ) $(LDFLAGS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
	rm $(PROGRAM_NAME)
	rm $(ADC_OBJ)
	rm adc_test

.PHONY: clean all
