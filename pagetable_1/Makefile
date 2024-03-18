CC = gcc
CFLAGS = -Wall -Wextra -std=c11
LDFLAGS =

SRCDIR = src
INCDIR = include
OBJDIR = obj
BINDIR = bin

SRCS = $(wildcard $(SRCDIR)/*.c)

OBJS = $(patsubst $(SRCDIR)/%.c,$(OBJDIR)/%.o,$(SRCS))

HEADERS = $(wildcard $(INCDIR)/*.h)

LIBNAME = libmlpt.a
LIBRARY = $(BINDIR)/$(LIBNAME)

all: $(LIBRARY)

$(OBJDIR)/%.o: $(SRCDIR)/%.c $(HEADERS)
	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@

$(LIBRARY): $(OBJS)
	ar rcs $@ $^

clean:
	rm -f $(OBJDIR)/*.o $(LIBRARY)

.PHONY: all clean
