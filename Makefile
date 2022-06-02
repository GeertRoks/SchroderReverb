CXXFLAGS := -Wall -std=c++11
LDFLAGS =
LDLIBS =
PROGRAM_NAME = test

OBJ = main.o schrodingersReverb.o filters/comb.o

all: test

test: $(OBJ)
	$(CXX) -o $@ $(CXXFLAGS) $(OBJ) $(LDFLAGS) $(LDLIBS)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)
	rm $(PROGRAM_NAME)

.PHONY: clean all
