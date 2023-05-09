P1=ParallelScanSerial
#INC="./inc"
#FLAGS=-I$(INC)
OMPFLAG=-fopenmp
CC=g++
CFLAGS = -g -Wall

all: ParallelScanSerial $(P1)

$(P1): $(P1).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(P1).cpp -o $(P1)

clean:
	rm -vf $(P1)