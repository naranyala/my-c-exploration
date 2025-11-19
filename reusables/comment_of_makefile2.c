// # Makefile template for C projects
//
// # Compiler and flags
// CC = gcc
// CFLAGS = -Wall -Wextra -std=c99 -pedantic
// DEBUG_FLAGS = -g -O0
// RELEASE_FLAGS = -O2
//
// # Directories
// SRCDIR = src
// INCDIR = include
// BUILDDIR = build
// BINDIR = bin
//
// # Targets
// TARGET = $(BINDIR)/program
// SOURCES = $(wildcard $(SRCDIR)/*.c)
// OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(BUILDDIR)/%.o)
//
// # Default target
// all: $(TARGET)
//
// # Create binary
// $(TARGET): $(OBJECTS) | $(BINDIR)
// 	$(CC) $(OBJECTS) -o $@
//
// # Create object files
// $(BUILDDIR)/%.o: $(SRCDIR)/%.c | $(BUILDDIR)
// 	$(CC) $(CFLAGS) -I$(INCDIR) -c $< -o $@
//
// # Create directories
// $(BINDIR):
// 	mkdir -p $(BINDIR)
//
// $(BUILDDIR):
// 	mkdir -p $(BUILDDIR)
//
// # Debug build
// debug: CFLAGS += $(DEBUG_FLAGS)
// debug: all
//
// # Release build
// release: CFLAGS += $(RELEASE_FLAGS)
// release: all
//
// # Clean build artifacts
// clean:
// 	rm -rf $(BUILDDIR) $(BINDIR)
//
// # Run the program
// run: $(TARGET)
// 	./$(TARGET)
//
// .PHONY: all debug release clean run
