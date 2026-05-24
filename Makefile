CC = gcc
CFLAGS = -Wall -I include

all:
	mkdir -p build
	$(CC) $(CFLAGS) src/*.c -o build/integrctrl

clean:
	rm -rf build data
