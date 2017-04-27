CC = mpic++
CXXFLAGS =-Wall
LIBS=-lpmi

FILE1 = seqMatrix.cpp
FILE2 = parMatrix.cpp

OUT1 = sequential
OUT2 = parallel

all: $(OUT1) $(OUT2)

$(OUT1): $(FILE1)
	$(CC) $(CXXFLAGS) -o $(OUT1) $(FILE1) $(LIBS)

$(OUT2): $(FILE2)
	$(CC) $(CXXFLAGS) -o $(OUT2) $(FILE2) $(LIBS)

clean:
	/rm *.o $(OUT1) $(OUT2) *~
