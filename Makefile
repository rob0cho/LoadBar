# Name: Robert Choate

CC=g++
CLFAGS=-std=c++0x
LIBS=-lpthread

wordcount: wordcount.cpp
	$(CC) $(CLFAGS) wordcount.cpp -o wordcount $(LIBS)

clean:
	rm wordcount