.PHONY: all

CXXFLAGS = -std=c++17

all:
	@echo "Usage: make scoring, make <num>, or make answers-<num>"

answers-%:
	@g++ $(CXXFLAGS) tests/test$*.cpp answers.o -o test$*_sol
	@./test$*_sol
	@rm test$*_sol

scoring:
	@g++ $(CXXFLAGS) -c scoring.cc -o scoring.o

9:
	@g++ $(CXXFLAGS) -c scoring.cc -o scoring.o
	@g++ $(CXXFLAGS) tests/test$@.cpp scoring.o -o test$@
	@./test$@; rc=$$?; rm -f test$@; exit $$rc

%:
	@g++ $(CXXFLAGS) -c scoring.cc -o scoring.o
	@g++ $(CXXFLAGS) tests/test$@.cpp scoring.o -o test$@
	@sh -c 'set -e; ./test$@; rc=$$?; if [ $$rc -eq 0 ]; then echo "Test $@ passed"; else echo "Test $@ failed"; fi; rm -f test$@; exit $$rc'
