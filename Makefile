CC = gcc
SRCDIR = src
SOURCES = $(shell find $(SRCDIR) -name '*.c')
OBJECTS = $(SOURCES:.c=.o)
TARGET = ./build/main

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET)

%.o: %.c
	$(CC) -c $< -o $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(OBJECTS) $(TARGET)

.PHONY: all clean
