.PHONY: all examples format clean

all: examples

examples: \
	graph.o

graph.o: examples/graph.cpp
	g++ -Wall -Wextra -Werror -Wpedantic \
		-std=c++20 -O3 \
		-I./include \
		examples/graph.cpp -o $@

format: \
		include/tante.hpp \
		examples/graph.cpp
	clang-format -i $^

clean:
	rm -rf `find . -name "*.o"`
	rm -rf `find . -name "*.csv"`
	rm -rf `find . -name "*.txt"`
