SOURCES = httpserver.c List.c Headers.c Request.c Parse.c Process.c ThreadPool.c helpers.c MutexMap.c
INCLUDES = $(wildcard *.h)

# executable name
TARGET = httpserver

# compiler flags for compiling code
CFLAGS = -Wall -Werror -Wextra -pedantic

# linker flags
LDFLAGS = -lpthread

# use clang as compiler
CC = clang

# object files
OBJECTS = $(SOURCES:.c=.o)

all: $(TARGET)

clean:
	-rm -rf $(OBJECTS) $(TARGET)

format:
	clang-format -i $(SOURCES) $(INCLUDES)

$(TARGET): $(OBJECTS)
	$(CC) $(LDFLAGS) -o $@ $(OBJECTS)

# compile .c files into .o
%.o: %.c
	$(CC) $(CFLAGS) -g -c -o $@ $<

# all and clean aren't real targets
.PHONY: all clean format
