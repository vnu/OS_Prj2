
all: bb mh airline
exes = bb mh airline

bb: bb.c osheaders.h
	g++ bb.c -o bb -pthread
	
mh: mh.c osheaders.h
	g++ mh.c -o mh -pthread
	
airline: airline.c osheaders.h
	g++ airline.c -o airline -pthread

clean:
	rm $(exes)