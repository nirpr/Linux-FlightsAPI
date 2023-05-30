# Makefile for compiling C++ flights programs with a shared library

CXX = g++ # Compiler
CXXFLAGS = -Werror -std=c++17 # Compiler flags
LDFLAGS = -Wl,-rpath=`pwd` -lzip
LIB = libutilities.so # Name of shared library

# getting all programs with .cpp
LIBDIR=lib
SRCS := $(wildcard $(LIBDIR)/*.cpp) # Create list of sources cpp files
TARGET := $(patsubst $(LIBDIR)/%.cpp,%.out,$(SRCS)) # Create list of sources in target location

# getting all utilities required for shared program
SLIBDIR = $(LIBDIR)/slib
UTILS := $(wildcard $(SLIBDIR)/*.cpp) # Create list of utilities cpp files

# List of object files (generated from source files)
OBJS := $(patsubst %.cpp,%.out,$(SRCS))

# for in the end of makefile .o files will not delete created OBJO
OBJSO := $(patsubst %.cpp,%.o,$(SRCS)) 

# Default targets
all: $(TARGET) $(OBJSO)

# Rule for create .out file -- Done
%.out : $(LIBDIR)/%.o	$(LIB)  #
	$(CXX) $^ $(LDFLAGS) -o $(patsubst $(LIBDIR)/%.out,%.out, $@)

# Rule for compiling source filesSS
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

# Rule for creating shared library
$(LIB): $(UTILS) 
	$(CXX) -shared -fPIC $^ -o $@ 

# Clean up
clean:
	find . -name "*.out" -exec rm -rf {} \;
	find . -name "*.so" -exec rm -rf {} \;
	find . -name "*.o" -exec rm -rf {} \;