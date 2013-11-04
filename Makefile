CFLAGS = -Wall -std=gnu99

all: displayid3.o id3v2.o

clean:
	rm -f ./*.o ./*.gch ./*.exe ./*.stackdump
