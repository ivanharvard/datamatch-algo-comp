.PHONY: all

all:
	@echo "Usage: make <num>  or  make answers-<num>"

answers-%:
	@g++ tests/test$*.cpp answers.o -o test$*_sol
	@./test$*_sol
	@rm test$*_sol

%:
	@g++ -c matching.cc -o matching.o
	@g++ tests/test$@.cpp matching.o -o test$@
	@./test$@
	@rm test$@
