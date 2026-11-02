.PHONY: all scoring clean

CXX      := g++
CXXFLAGS := -std=c++17

# Everything you compile: your files plus the provided scoring code
SRCS  := compatibility.cc scoring.cc $(wildcard factors/*.cc)
OBJS  := $(SRCS:.cc=.o)
TESTS := 1 2 3 4 5 6 7 8 9

all:
	@echo "Usage: make scoring, make <num>, or make answers-<num>"

%.o: %.cc scoring.hpp user.hpp factors/factors.hpp
	@$(CXX) $(CXXFLAGS) -c $< -o $@

scoring: $(OBJS)

# Runs a test against your code
$(TESTS): %: $(OBJS)
	@$(CXX) $(CXXFLAGS) tests/test$@.cpp $(OBJS) -o test$@
	@sh -c './test$@; rc=$$?; if [ $$rc -eq 0 ]; then echo "Test $@ passed"; else echo "Test $@ failed"; fi; rm -f test$@; exit $$rc'

# Runs a test against our answers
answers-%:
	@$(CXX) $(CXXFLAGS) tests/test$*.cpp answers.o -o test$*_sol
	@./test$*_sol
	@rm test$*_sol

clean:
	@rm -f $(OBJS) $(addprefix test,$(TESTS)) $(addsuffix _sol,$(addprefix test,$(TESTS)))
