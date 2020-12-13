
 .PHONY:clean all
 CFLAGS=-Wall -std=c++11 -pthread -w

 BIN=bin

 SUBDIR=$(shell ls -d */)

 ROOTSRC=$(wildcard *.cpp)
 ROOTOBJ=$(ROOTSRC:%.cpp=%.o)

 SUBSRC=$(shell find $(SUBDIR) -name '*.cpp')
 SUBOBJ=$(SUBSRC:%.cpp=%.o)

 $(BIN):$(ROOTOBJ) $(SUBOBJ)
     $(CXX) $(CFLAGS) -o $@ $^
 .cpp.o:
     $(CXX) $(CFLAGS) -c $< -o $@
 clean:
     rm -f *.o $(BIN) $(ROOTOBJ) $(SUBOBJ)
                     
