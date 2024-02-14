CC = gcc
CFLAGS = -Wall -Wextra -pedantic -std=c11
LDFLAGS =

SRC = gettimings.c
OBJ = $(SRC:.c=.o)
TARGET = gettimings

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ) $(TARGET)
