# Makefile for PylonPlay project

# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -I. -IGames -ITest -IUtilities

# Source files
SOURCES = main.cpp       \
		  Games/Hit.cpp  \
		  Games/Open.cpp \
		  Test/DataSource.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Executable name
TARGET = PylonPlay.exe

# Default target
all: $(TARGET)

# Link object files to create executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compile source files to object files
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Clean build artifacts
clean:
	del /Q $(subst /,\,$(OBJECTS)) $(TARGET)

# Run the executable
run: $(TARGET)
	$(TARGET)

.PHONY: all clean run