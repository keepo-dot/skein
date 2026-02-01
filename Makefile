# Samitizer Flags
SANITIZER_FLAGS = -fsanitize=address,undefined

# Compiler and Flags
CC = clang
# -Iinclude tells the compiler to look in your include folder for .h files
CFLAGS = -Wall -Wextra -g $(shell pkg-config --cflags gtk4 json-glib-1.0) -Iinclude #$(SANITIZER_FLAGS)
LIBS = $(shell pkg-config --libs gtk4 json-glib-1.0) #$(SANITIZER_FLAGS)

# Directory Setup
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin

# The 5 core source files we mashed together
SRCS = $(SRC_DIR)/skein.c \
       $(SRC_DIR)/canvas.c \
       $(SRC_DIR)/toolbar.c \
       $(SRC_DIR)/skein_window.c \
       $(SRC_DIR)/utils.c \
			 $(SRC_DIR)/resources.c

# Automatically turn .c paths into .o paths in the obj/ folder
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# The final executable name
TARGET = $(BIN_DIR)/skein

# --- Rules ---

# Default rule
all: $(TARGET)

# Link the object files into the final binary
$(TARGET): $(OBJS)
	@mkdir -p $(BIN_DIR)
	$(CC) $(OBJS) -o $(TARGET) $(LIBS)

# Compile each .c file into a .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(OBJ_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up the mess
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)

.PHONY: all clean
