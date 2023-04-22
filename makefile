# Makefile for compiling C++ programs with a shared library
.DEFAULT_GOAL := all
CXX = g++ # Compiler
CXXFLAGS = -Werror -std=c++17  # Compiler flags
#LDFLAGS =  -Wl,-rpath=`pwd` # Linker flags
LDFLAGS = -L./lib -Ilib/slib -Ilib -Wl,-rpath=`pwd`
LIB = libutilities.so # Name of shared library
INCLUDE_LIB = -I./lib/slib

# getting all programs with .cpp
LIBDIR=lib
# SRCS := $(subst ./,,$(shell find . -name "*.cpp"))
SRCS := $(wildcard $(LIBDIR)/*.cpp) # Create list of srouces cpp files

# getting all utilities required for shared program
SLIBDIR=$(LIBDIR)/slib
UTILS := $(wildcard $(SLIBDIR)/*.cpp) # Create list of utilities cpp files
# List of object files (generated from source files)

#OBJS := $(patsubst %.cpp,%.out,$(wildcard lib/*.cpp))
OBJS := $(patsubst %.cpp,%.out,$(wildcard lib/*.cpp))



# Default target
.PHONY: all move clean
all: $(OBJS) 

# Rule for create .out file -- Done
%.out : %.o	$(LIB)
	$(CXX) $^ -o $@ $(LDFLAGS) $(LIB)

# Rule for compiling source files
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Rule for creating shared library
$(LIB): $(UTILS)
	$(CXX) -shared -fPIC $(CXXFLAGS) $^ -o $@ 
# move the directory to parent directory
ODIR := .
move:
	mv ./$(LIBDIR)/*.out $(ODIR)
# Clean up
clean:
	find . -name "*.out" -exec rm -rf {} \;
	find . -name "*.so" -exec rm -rf {} \;