CC = mpic++
CXXFLAGS =-Wall
LIBS=-lpmi

FILE1 = seqMatrix.cpp

OUT1 = sequential

all: $(OUT1)

$(OUT1): $(FILE1)
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) $(LIBS)

clean:
	/rm *.o $(OUT1) *~
