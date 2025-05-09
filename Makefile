CC = gcc
CFLAGS = `pkg-config --cflags gtk+-3.0` -Iinclude -Wall -g
LDFLAGS = `pkg-config --libs gtk+-3.0`
SRC = src/main.c
TARGET = build/calc-app

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

run: $(TARGET)
	$(TARGET)

clean:
	rm -f $(TARGET) src/*.o

.PHONY: all clean run
