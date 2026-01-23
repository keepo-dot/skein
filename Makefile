# 1. Variables (The Ingredients)
CC = gcc
CFLAGS = `pkg-config --cflags gtk4` -Iinclude
LIBS = `pkg-config --libs gtk4`

# This variable lists all your source files
SRCS = main.c src/*/*.c

# This variable defines the name of your final program
TARGET = knitpatc

# 2. Rules (The Recipes)
# The first rule is what happens when you just type 'make'
all: $(TARGET)

$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET) $(LIBS)

# A 'clean' rule helps you start fresh by deleting the executable
clean:
	rm -f $(TARGET)
