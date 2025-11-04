.PHONY: all

all:
	@echo "Usage: make <num>  or  make answers-<num>"

answers-%:
	@g++ -std=c++11 tests/test$*.cpp answers.o -o test$*_sol
	@./test$*_sol
	@rm test$*_sol

%:
	@g++ -std=c++11 -c matching.cc -o matching.o
	@g++ -std=c++11 tests/test$@.cpp matching.o -o test$@
	@./test$@
	@rm test$@
