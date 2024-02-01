CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99
SRCS = split.c main.c
OBJS = $(SRCS:.c=.o)
EXEC = split
.PHONY: all clean
all: $(EXEC)

$(EXEC): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(EXEC)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS) $(EXEC)
