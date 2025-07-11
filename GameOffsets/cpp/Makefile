# Simple Makefile for GameOffsets Pattern library
# Alternative to CMake for basic builds

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -O2
INCLUDES = -I.

# Source files
SOURCES = Pattern.cpp StaticOffsetsPatterns.cpp
OBJECTS = $(SOURCES:.cpp=.o)
LIBRARY = libGameOffsetsPatterns.a
TEST_SOURCES = test_main.cpp
TEST_EXECUTABLE = test_patterns
EXAMPLE_SOURCES = example.cpp
EXAMPLE_EXECUTABLE = example

.PHONY: all clean test example

all: $(LIBRARY) $(TEST_EXECUTABLE) $(EXAMPLE_EXECUTABLE)

$(LIBRARY): $(OBJECTS)
	ar rcs $@ $^

$(TEST_EXECUTABLE): $(TEST_SOURCES) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< -L. -lGameOffsetsPatterns

$(EXAMPLE_EXECUTABLE): $(EXAMPLE_SOURCES) $(LIBRARY)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $< -L. -lGameOffsetsPatterns

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c $< -o $@

test: $(TEST_EXECUTABLE)
	./$(TEST_EXECUTABLE)

example: $(EXAMPLE_EXECUTABLE)
	./$(EXAMPLE_EXECUTABLE)

clean:
	rm -f $(OBJECTS) $(LIBRARY) $(TEST_EXECUTABLE) $(EXAMPLE_EXECUTABLE)

install: $(LIBRARY)
	mkdir -p /usr/local/lib /usr/local/include/GameOffsets
	cp $(LIBRARY) /usr/local/lib/
	cp *.h /usr/local/include/GameOffsets/

# Dependencies
Pattern.o: Pattern.h
StaticOffsetsPatterns.o: StaticOffsetsPatterns.h Pattern.h