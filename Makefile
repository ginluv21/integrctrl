CC = gcc
CFLAGS = -Wall -std=c99

SRC = src/main.c src/integrctrl.c src/md5.c
TARGET = build/integrctrl

all: $(TARGET)

$(TARGET): $(SRC)
	mkdir -p build
	$(CC) $(CFLAGS) -I include $(SRC) -o $(TARGET)

clean:
	rm -rf build
