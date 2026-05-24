CC = gcc
CFLAGS = -Wall -I include

all:
	mkdir -p build
	$(CC) $(CFLAGS) src/*.c -o build/integrctrl

test:
	mkdir -p build
	$(CC) $(CFLAGS) test.c src/vector.c src/integrctrl.c src/db.c src/md5.c -o build/test
	./build/test

clean:
	rm -rf build
