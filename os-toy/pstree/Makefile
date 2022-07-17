NAME := $(shell basename $(PWD))
export MODULE := M1
all: $(NAME)-64 $(NAME)-32

run:
	./$(NAME)-64

runp:
	./$(NAME)-64 -p

runn:
	./$(NAME)-64 -n

runnp:
	./$(NAME)-64 -np

include ../Makefile
