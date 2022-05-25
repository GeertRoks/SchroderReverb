CFLAGS = -std=c++11 -Wall

OBJ = schrodingersReverb.o filters/allpassDFII.o filters/comb.o

all: $(OBJ)

%.o: %.cpp %.h
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm $(OBJ)

.PHONY: clean all
