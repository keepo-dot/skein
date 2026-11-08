# Sanitizer Flags
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

# Source files
SRCS = $(SRC_DIR)/skein.c \
       $(SRC_DIR)/canvas_render.c \
       $(SRC_DIR)/canvas_events.c \
       $(SRC_DIR)/toolbar.c \
       $(SRC_DIR)/pattern.c \
       $(SRC_DIR)/skein_window.c \
       $(SRC_DIR)/utils.c \
       $(SRC_DIR)/resources.c \
       $(SRC_DIR)/file_io.c

# Automatically turn .c paths into .o paths in the obj/ folder
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# The final executable name
TARGET = $(BIN_DIR)/skein

# ANSI formatting for console output
BOLD   := \033[1m
GREEN  := \033[32m
BLUE   := \033[34m
YELLOW := \033[33m
RED    := \033[31m
RESET  := \033[0m

# --- Rules ---

# Default rule
all: $(TARGET)

# Link the object files into the final binary
$(TARGET): $(OBJS) | $(BIN_DIR)
	@printf "$(BOLD)$(GREEN)[LD]$(RESET) Linking executable: $(BOLD)$@$(RESET)\n"
	@$(CC) $(OBJS) -o $(TARGET) $(LIBS)
	@printf "$(BOLD)$(GREEN)[OK]$(RESET) Build complete.\n"

# Compile each .c file into a .o file
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	@printf "$(BOLD)$(BLUE)[CC]$(RESET) Compiling $< -> $@\n"
	@$(CC) $(CFLAGS) -c $< -o $@

# Create directories when needed
$(OBJ_DIR):
	@printf "$(YELLOW)[MKDIR]$(RESET) Creating object directory: $@\n"
	@mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	@printf "$(YELLOW)[MKDIR]$(RESET) Creating binary directory: $@\n"
	@mkdir -p $(BIN_DIR)

# Clean up the mess
clean:
	@printf "$(BOLD)$(RED)[CLEAN]$(RESET) Removing build artifacts...\n"
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@printf "$(BOLD)$(GREEN)[OK]$(RESET) Workspace clean.\n"

.PHONY: all clean
