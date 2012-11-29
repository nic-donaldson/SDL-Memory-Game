CPP=g++
CPPFLAGS=-g -Wall -lSDL -lSDL_image
LINK=g++ -g
all: memory

memory: memory.cpp timer.o

timer: timer.cpp
