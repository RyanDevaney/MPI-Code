CC = mpic++
CXXFLAGS=-Wall
LIBS=-lpmi

FILE1 = PIMFuncs.cpp
FILE2 = PIMFuncs.h
FILE3 = seq.cpp
FILE4 = static.cpp

OUT1 = sequential
OUT2 = static

all: $(OUT1) $(OUT2)

$(OUT2): $(FILE4) PIMFuncs.o
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE4) PIMFuncs.o $(LIBS)

$(OUT1): $(FILE3) PIMFuncs.o
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE3) PIMFuncs.o $(LIBS)

PIMFuncs.o: $(FILE1) $(FILE2)
	g++ -c -g $(FILE1)

clean:
	/rm *.o $(OUT1) *~
