COMMON=../common
CFLAGS=-Wall -Wextra -g -I$(COMMON)
LDLIBS=-lpthread
CC=gcc
BIN=../bin
VPATH=$(COMMON)

all: sysmgr child prockiller install

sysmgr: sysmgr.o configreader.o
child: child.o configreader.o
prockiller: prockiller.o configreader.o

clean:
	rm -f *.o $(BIN)/sysmgr $(BIN)/child* $(BIN)/prockiller
    
install:    
	mv  child child1
	cp  child1 child2
	cp  child1 child3
	mv  child1 child2 child3 sysmgr prockiller $(BIN)
    

