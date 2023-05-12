P1=scan
P2=scan_omp
P3=generator
P4=bitonic
P5=bitonic_omp
#INC="./inc"
#FLAGS=-I$(INC)
OMPFLAG=-fopenmp
CC=g++
CFLAGS = -g -Wall

all: scan $(P1) $(P2) $(P3) $(P4) $(P5)

$(P1): $(P1).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(P1).cpp -o $(P1)

$(P2): $(P2).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(P2).cpp -o $(P2)

$(P3): $(P3).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(P3).cpp -o $(P3)

$(P4): $(P4).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(P4).cpp -o $(P4)

$(P5): $(P5).cpp
	$(CC) $(CFLAGS) $(OMPFLAG) $(P5).cpp -o $(P5)

clean:
	rm -vf $(P1) $(P2) $(P3) $(P4) $(P5)