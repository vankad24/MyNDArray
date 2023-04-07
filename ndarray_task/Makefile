CC = g++
CFLAGS = --std=c++11 -Wall -pedantic
SOURCE1 = main.cpp
OUT1 = main.exe

ifeq ($(OS), Windows_NT)
	RM = del
else
	RM = rm
endif

all:
	$(CC) $(CFLAGS) -o $(OUT1) $(SOURCE1)
	
clean:
	$(RM) $(OUT1)

run: all
	./$(OUT1)