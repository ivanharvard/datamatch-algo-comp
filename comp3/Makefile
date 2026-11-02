.PHONY: all main run clean

CXX      := g++
CXXFLAGS := -std=c++17 -O2 -I.

# Every .cc file here and in factors/ is part of ./main, like in the real
# algorithm. matching.o is the matching algorithm from comp1, precompiled.
SRCS   := $(wildcard *.cc factors/*.cc)
OBJS   := $(SRCS:.cc=.o)
TARGET := main

all: main

# Each .o is rebuilt when its .cc or any header changes
%.o: %.cc $(wildcard *.h factors/*.h)
	$(CXX) $(CXXFLAGS) -c $< -o $@

main: $(OBJS) matching.o
	$(CXX) $(CXXFLAGS) $(OBJS) matching.o -o $(TARGET)

# Runs the algorithm on the test data. The real algorithm takes the folders
# for answer-choice, bio, short-answer and Spotify similarities separately;
# here they are all in data/sims.
run: main
	./main data/FIREBASE_EXPORT.json data/sims data/sims data/sims data/sims

clean:
	rm -f $(TARGET) $(OBJS)
