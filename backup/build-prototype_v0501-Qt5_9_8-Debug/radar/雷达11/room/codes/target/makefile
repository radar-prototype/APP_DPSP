CC:= g++
TARGET:= main

CFLAGS:= -g -Wall -D_REENTRANT

LIBS:= -L  /lib/  -lsubscriber -lpublisher -lrspl -lvsip -lpthread

#OBJECTS := MsgQueue.o  sim.o dpc.o cfar.o mtd.o sdae.o  slc.o main.o 



SRCS :=$(wildcard *.cpp) 

OBJECTS := $(patsubst %cpp,%o,$(SRCS))
     
$(TARGET): $(OBJECTS) 
	$(CC) -o $(TARGET)  $(OBJECTS) $(LIBS)
  
# $@表示所有目标集  
%.o:%cpp
	$(CC) -c $(CFLAGS)  $< -o $@

  
.PHONY : clean
clean:
	-rm -f $(OBJECTS) $(TARGET)












