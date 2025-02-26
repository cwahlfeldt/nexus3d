# Nexus3D Game Engine Makefile

# Compiler settings
CC = gcc
CXX = g++
AR = ar
CFLAGS = -std=c11 -Wall -Wextra -pedantic -O2 -fPIC -I./include -I./lib/SDL3/include -I./lib/cglm/include -I./lib/flecs
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic -O2 -fPIC -I./include -I./lib/SDL3/include -I./lib/cglm/include -I./lib/flecs
LDFLAGS = -L./lib -lSDL3 -lcglm -lflecs -lm

# Debug flags
DEBUG_CFLAGS = -g -DNEXUS_DEBUG
DEBUG_CXXFLAGS = -g -DNEXUS_DEBUG

# Output directories
BUILD_DIR = build
LIB_DIR = $(BUILD_DIR)/lib
BIN_DIR = $(BUILD_DIR)/bin
OBJ_DIR = $(BUILD_DIR)/obj
INCLUDE_DIR = include

# Library name
LIB_NAME = nexus3d
LIB_STATIC = $(LIB_DIR)/lib$(LIB_NAME).a
LIB_SHARED = $(LIB_DIR)/lib$(LIB_NAME).so

# Source files
SRC_DIRS = src src/core src/renderer src/ecs src/input src/utils src/math src/audio src/physics
SRCS = $(foreach dir,$(SRC_DIRS),$(wildcard $(dir)/*.c))
OBJS = $(patsubst %.c,$(OBJ_DIR)/%.o,$(SRCS))

# Example sources
EXAMPLE_SRCS = $(wildcard examples/*.c)
EXAMPLE_BINS = $(patsubst examples/%.c,$(BIN_DIR)/%,$(EXAMPLE_SRCS))

# Default target
all: directories static shared examples

# Create build directories
directories:
	@mkdir -p $(BUILD_DIR)
	@mkdir -p $(LIB_DIR)
	@mkdir -p $(BIN_DIR)
	@mkdir -p $(OBJ_DIR)
	@mkdir -p $(foreach dir,$(SRC_DIRS),$(OBJ_DIR)/$(dir))

# Build static library
static: directories $(LIB_STATIC)

# Build shared library
shared: directories $(LIB_SHARED)

# Build examples
examples: directories static $(EXAMPLE_BINS)

# Static library
$(LIB_STATIC): $(OBJS)
	@echo "Creating static library: $@"
	@$(AR) rcs $@ $^

# Shared library
$(LIB_SHARED): $(OBJS)
	@echo "Creating shared library: $@"
	@$(CC) -shared -o $@ $^ $(LDFLAGS)

# Compile source files
$(OBJ_DIR)/%.o: %.c
	@echo "Compiling: $<"
	@mkdir -p $(dir $@)
	@$(CC) $(CFLAGS) -c $< -o $@

# Build examples
$(BIN_DIR)/%: examples/%.c $(LIB_STATIC)
	@echo "Building example: $@"
	@$(CC) $(CFLAGS) $< -o $@ -L$(LIB_DIR) -l$(LIB_NAME) $(LDFLAGS)

# Install
install: all
	@echo "Installing Nexus3D..."
	@mkdir -p $(DESTDIR)/usr/local/lib
	@mkdir -p $(DESTDIR)/usr/local/include/nexus3d
	@cp $(LIB_STATIC) $(DESTDIR)/usr/local/lib/
	@cp $(LIB_SHARED) $(DESTDIR)/usr/local/lib/
	@cp -r $(INCLUDE_DIR)/nexus3d $(DESTDIR)/usr/local/include/

# Uninstall
uninstall:
	@echo "Uninstalling Nexus3D..."
	@rm -f $(DESTDIR)/usr/local/lib/lib$(LIB_NAME).a
	@rm -f $(DESTDIR)/usr/local/lib/lib$(LIB_NAME).so
	@rm -rf $(DESTDIR)/usr/local/include/nexus3d

# Clean build files
clean:
	@echo "Cleaning build files..."
	@rm -rf $(BUILD_DIR)

# Debug build
debug: CFLAGS += $(DEBUG_CFLAGS)
debug: CXXFLAGS += $(DEBUG_CXXFLAGS)
debug: all

# Run examples
run-examples: examples
	@for example in $(EXAMPLE_BINS); do \
		echo "Running $$example..."; \
		LD_LIBRARY_PATH=$(LIB_DIR) $$example; \
	done

# Help
help:
	@echo "Nexus3D Game Engine Build System"
	@echo "--------------------------------"
	@echo "make              - Build everything (static, shared, examples)"
	@echo "make static       - Build static library"
	@echo "make shared       - Build shared library"
	@echo "make examples     - Build example applications"
	@echo "make install      - Install libraries and headers"
	@echo "make uninstall    - Uninstall libraries and headers"
	@echo "make clean        - Remove build files"
	@echo "make debug        - Build with debug symbols"
	@echo "make run-examples - Build and run all examples"
	@echo "make help         - Show this help message"

.PHONY: all directories static shared examples install uninstall clean debug run-examples help
