CXXFLAGS := -Wall -std=c++11 -g
LDFLAGS =
LDLIBS = -lpthread -lm
LDLIBS_BCM = -lbcm2835
PROGRAM_NAME = schroederreverb

BIN_DIR = bin
OBJ_DIR = $(BIN_DIR)/obj
SRC_DIR = src
SRC_TEST_DIR = tests

# --- Source files ---

MAIN_FILES = src/single_task_main.cpp src/multi_task_main.cpp src/main.cpp
REV_FILES = src/schrodingersReverb.cpp src/filters/allpassDFII.cpp src/filters/comb.cpp
ADC_FILES = src/adc/task_audio_io.o src/adc/MCP3204_BCM2835/MCP3204_BCM2835.o

PERF_TEST_FILES = $(wildcard tests/perf_tests/*.cpp)
COMPARISON_TEST_FILES = $(wildcard tests/comparison_tests/*.cpp)
FIFO_TEST_FILES = $(wildcard tests/fifo_tests/*.cpp)
ADC_TEST_FILES = $(wildcard tests/adc_tests/*.cpp)

SOURCE_FILES = $(MAIN_FILES) $(REV_FILES) $(ADC_FILES)
SOURCE_TEST_FILES = $(PERF_TEST_FILES) $(COMPARISON_TEST_FILES) $(FIFO_TEST_FILES) $(ADC_TEST_FILES)

# --- Object files ---

REV_OBJ = $(REV_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)
ADC_OBJ = $(ADC_FILES:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

# --- Executable files ---

BIN_FILES = $(SOURCE_FILES:%.cpp=$(BIN_DIR)/%)
BIN_TEST_FILES = $(SOURCE_TEST_FILES:tests/%.cpp=$(BIN_DIR)/%)

BIN_PERF_TEST_FILES = $(PERF_TEST_FILES:tests/perf_tests/%.cpp=$(BIN_DIR)/%)
BIN_COMPARISON_TEST_FILES = $(COMPARISON_TEST_FILES:tests/comparison_tests/%.cpp=$(BIN_DIR)/%)
BIN_FIFO_TEST_FILES = $(FIFO_TEST_FILES:tests/fifo_tests/%.cpp=$(BIN_DIR)/%)
BIN_ADC_TEST_FILES = $(ADC_TEST_FILES:tests/adc_tests/%.cpp=$(BIN_DIR)/%)


all: reverb_single_task reverb_multi_task
tests: perf_tests comparison_tests fifo_tests adc_tests
perf_tests:$(BIN_PERF_TEST_FILES)
comparison_tests: $(BIN_COMPARISON_TEST_FILES)
fifo_tests: $(BIN_FIFO_TEST_FILES)
adc_tests: $(BIN_ADC_TEST_FILES)

$(BIN_DIR)/% : tests/perf_tests/%.cpp $(REV_FILES)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS)
$(BIN_DIR)/% : tests/comparison_tests/%.cpp $(REV_FILES)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS)
$(BIN_DIR)/% : tests/fifo_tests/%.cpp $(REV_FILES)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS)
$(BIN_DIR)/% : tests/adc_tests/%.o $(REV_OBJ) $(ADC_OBJ)
	@mkdir -p $(@D)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS) $(LDLIBS_BCM)

$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp src/%.h
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 
$(OBJ_DIR)/%.o : $(SRC_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 
$(OBJ_DIR)/%.o : $(SRC_TEST_DIR)/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CXXFLAGS) -c $< -o $@ 

reverb_single_task: $(OBJ_DIR)/single_task_main.o $(REV_OBJ) $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS) $(LDLIBS_BCM)

reverb_multi_task: src/multi_task_main.o $(REV_OBJ) $(ADC_OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $^ $(LDFLAGS) $(LDLIBS) $(LDLIBS_BCM)

#Makefile variable print for debugging
# Usage: 	make print-<VARIABLE_NAME>
# result:	<VARIABLE_NAME> = the_value_of_the_variable
print-%  : ; @echo $* = $($*)

clean:
	rm -r $(BIN_DIR)
	rm reverb_multi_task reverb_single_task

.PHONY: clean all
