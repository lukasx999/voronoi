CXX=g++
CC=gcc
INCLUDE=-I./glad/include
FLAGS=-Wall -Wextra -pedantic -ggdb $(INCLUDE)
CXXFLAGS=$(FLAGS) -std=c++23
CFLAGS=$(FLAGS) -std=c99
LIBS=-lGL -lglfw
GLADGL=./glad/src/gl.o

all: main.o $(GLADGL)
	$(CXX) $(CXXFLAGS) $(LIBS) $^ -o out

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm *.o out $(GLADGL)

.PHONY: clean
