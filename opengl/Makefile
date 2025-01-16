CXX=g++
CC=gcc
INCLUDE=-I./glad/include
FLAGS=-Wall -Wextra -pedantic -ggdb $(INCLUDE)
CXXFLAGS=$(FLAGS) -std=c++23
CFLAGS=$(FLAGS) -std=c99
LIBS=-lGL -lglfw -lX11
GLADGL=./glad/src/gl.o ./glad/src/glx.o

all: main.o $(GLADGL)
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o out

xlib: xlib.o $(GLADGL)
	$(CC) $(CFLAGS) $(LIBS) $^ -o out

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o out $(GLADGL)

.PHONY: clean
