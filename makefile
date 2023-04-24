# Makefile for compiling C++ programs with a shared library
.DEFAULT_GOAL := all
CXX = g++ # Compiler
CXXFLAGS = -Werror -std=c++17  # Compiler flags
#LDFLAGS =  -Wl,-rpath=`pwd` # Linker flags -Ilib/slib -Ilib
LDFLAGS = -L. -Wl,-rpath=`pwd`
LIB = libutilities.so # Name of shared library

# getting all programs with .cpp
LIBDIR=lib
SRCS := $(wildcard $(LIBDIR)/*.cpp) # Create list of srouces cpp files

# getting all utilities required for shared program
SLIBDIR = $(LIBDIR)/slib
UTILS := $(wildcard $(SLIBDIR)/*.cpp) # Create list of utilities cpp files

# List of object files (generated from source files)
OBJS := $(patsubst %.cpp,%.out,$(SRCS))

# for in the end of makefile .o files will not delete created OBJO
OBJSO := $(patsubst %.cpp,%.o,$(SRCS)) 


# Default target
.PHONY: all move
all: $(OBJS) $(OBJSO) move

# Rule for create .out file -- Done
%.out : %.o	$(LIB)
	$(CXX) $^ -o $@ -L. $(LDFLAGS)

# Rule for compiling source filesSS
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Rule for creating shared library
$(LIB): $(UTILS) 
	$(CXX) -shared -fPIC $^ $(LDFLAGS) -o $@ 
# move the directory to parent directory
ODIR := .
move:
	mv ./$(LIBDIR)/*.out $(ODIR)
# # Clean up
# clean:
# 	find . -name "*.out" -exec rm -rf {} \;
# 	find . -name "*.so" -exec rm -rf {} \;
# 	find . -name "*.o" -exec rm -rf {} \;