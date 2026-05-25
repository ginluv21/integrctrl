CC = gcc
CFLAGS = -Wall -I include

all:
	mkdir -p build data
	$(CC) $(CFLAGS) src/*.c -o build/integrctrl

clean:
	rm -rf build data
