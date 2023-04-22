# Makefile for compiling C++ programs with a shared library

# Compiler
CXX = g++
# Compiler flags
CXXFLAGS = -Werror -std=c++17 
# Linker flags
LDFLAGS =  -Wl,-rpath=`pwd`
# -L./lib -Ilib/slib -Ilib
# getting all programs with .cpp
LIBDIR=lib
SRCS := $(subst ./,,$(shell find . -name "*.cpp"))
#SRCS := $(wildcard $(LIBDIR)/*.cpp) # Create list of srouces cpp files

# getting all utilities required for shared program
SLIBDIR=$(LIBDIR)/slib
# UTILS := $(wildcard $(SLIBDIR)/*.cpp) 
UTILS := $(wildcard $(SLIBDIR)/*.cpp) # Create list of utilities cpp files
# List of object files (generated from source files)
OBJS := $(patsubst %.cpp,%.out,$(wildcard lib/*.cpp))

# Name of shared library
LIBRARY = utilities.so

# Default target
all: $(OBJS) 

# Rule for create .out file -- Done
%.out : %.o $(LIBRARY)
	$(CXX) $^ -o $@ $(LDFLAGS)
# Rule for compiling source files
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@
# Rule for creating shared library
$(LIBRARY): $(UTILS)
	$(CXX) -shared -fPIC $(LDFLAGS) $(CXXFLAGS) $^ -o $@ 
# Clean up
.PHONY: clean
clean:
	find . -name "*.out" -exec rm -rf {} \;