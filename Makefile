CXXFLAGS := -Wall -std=c++11 -g
LDFLAGS =
LDLIBS = -lpthread -lm
LDLIBS_BCM = -lbcm2835
PROGRAM_NAME = schroederreverb

BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj
SRC_TEST_DIR = tests

# --- Source files ---

MAIN_FILES = single_task_main.cpp multi_task_main.cpp main.cpp
REV_FILES = schrodingersReverb.cpp filters/allpassDFII.cpp filters/comb.cpp
ADC_FILES = adc/task_audio_io.o adc/MCP3204_BCM2835/MCP3204_BCM2835.o

PERF_TEST_FILES = $(wildcard tests/perf_tests/*.cpp)
COMPARISON_TEST_FILES = $(wildcard tests/comparison_tests/*.cpp)
FIFO_TEST_FILES = $(wildcard tests/fifo_tests/*.cpp)

SOURCE_FILES = $(MAIN_FILES) $(REV_FILES) $(ADC_FILES)
SOURCE_TEST_FILES = $(PERF_TEST_FILES) $(COMPARISON_TEST_FILES) $(FIFO_TEST_FILES)

# --- Executable files ---

BIN_FILES = $(SOURCE_FILES:%.cpp=$(BIN_DIR)/%)
BIN_TEST_FILES = $(SOURCE_TEST_FILES:tests/%.cpp=$(BIN_DIR)/%)

BIN_PERF_TEST_FILES = $(PERF_TEST_FILES:tests/perf_tests/%.cpp=$(BIN_DIR)/%)
BIN_COMPARISON_TEST_FILES = $(COMPARISON_TEST_FILES:tests/comparison_tests/%.cpp=$(BIN_DIR)/%)
BIN_FIFO_TEST_FILES = $(FIFO_TEST_FILES:tests/fifo_tests/%.cpp=$(BIN_DIR)/%)


all: reverb_single_task reverb_multi_task
tests: perf_tests comparison_tests fifo_tests
perf_tests:$(BIN_PERF_TEST_FILES)
comparison_tests: $(BIN_COMPARISON_TEST_FILES)
fifo_tests: $(BIN_FIFO_TEST_FILES)

$(BIN_DIR)/% : tests/perf_tests/%.cpp $(REV_FILES)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS)
$(BIN_DIR)/% : tests/comparison_tests/%.cpp $(REV_FILES)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS)
$(BIN_DIR)/% : tests/fifo_tests/%.cpp $(REV_FILES)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS)

reverb_single_task: $(SOURCE_FILES)
	$(CXX) -o $@ $(CXXFLAGS) $(SOURCE_FILES) $(LDFLAGS) $(LDLIBS) $(LDLIBS_BCM)

reverb_multi_task: $(SOURCE_FILES)
	$(CXX) -o $@ $(CXXFLAGS) $(SOURCE_FILES) $(LDFLAGS) $(LDLIBS) $(LDLIBS_BCM)

#Makefile variable print for debugging
# Usage: 	make print-<VARIABLE_NAME>
# result:	<VARIABLE_NAME> = the_value_of_the_variable
print-%  : ; @echo $* = $($*)

clean:
	rm -r $(BIN_DIR)
	rm reverb_multi_task reverb_single_task

.PHONY: clean all
