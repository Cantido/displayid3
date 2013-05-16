CFLAGS = -Wall -std=gnu99

all: displayid3 id3v2

displayid3: displayid3.c
	$(CC) $(CFLAGS) $^ -c

id3v2: id3v2.h id3v2.c
	$(CC) $(CFLAGS) $^ -c

clean:
	rm -f ./*.o ./*.gch ./*.exe ./*.stackdump