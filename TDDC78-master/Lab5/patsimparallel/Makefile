
CXXFLAGS = -g -std=c++11
CXX = mpicxx
#CXX = mpic++

all: patsim patsimtest

clean:
	-$(RM) *.o patsim patsimtest

patsim: physics.o main_sequential.o
	$(CXX) -o $@ physics.o main_sequential.o $(CXXFLAGS)

patsimtest: physics.o main_test.o
	$(CXX) -o $@ physics.o main_test.o $(CXXFLAGS)

