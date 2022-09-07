NAME := $(shell basename $(PWD))
all: $(NAME)-64
LDFLAGS += -lm

include ../Makefile

.PHONY: run
BITS := 64
IMG := "fs.img"
run: all
	./$(NAME)-$(BITS) $(IMG)
gdb: all git
	gdb ./$(NAME)-$(BITS)
