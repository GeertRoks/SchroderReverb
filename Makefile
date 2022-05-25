CXXFLAGS := -Wall -std=c++11
CXXFLAGS += $(patsubst %,-I %, $(MODULES))
LDFLAGS =
LDLIBS =
PROGRAM_NAME = test

OBJ = main.o schrodingersReverb.o filters/allpassDFII.o filters/comb.o

all: $(PROGRAM_NAME)

$(PROGRAM_NAME): $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
	rm $(PROGRAM_NAME)

.PHONY: clean all
