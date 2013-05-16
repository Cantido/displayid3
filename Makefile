CFLAGS = -Wall -std=gnu99

all: displayid3

displayid3: displayid3.c
	$(CC) $(CFLAGS) $^

clean:
	rm -f ./*.o ./*.gch ./*.exe ./*.stackdump