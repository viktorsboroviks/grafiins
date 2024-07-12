.PHONY: all examples format clang-format black clean

all: examples

examples: \
	graph.o

graph.o: examples/graph.cpp
	g++ -Wall -Wextra -Werror -Wpedantic \
		-std=c++20 -O3 \
		-I./include \
		examples/graph.cpp -o $@

clang-format: \
		include/vgraph.hpp \
		examples/graph.cpp
	clang-format -i $^

black: plot_graph.py
	black $^

format: clang-format black

clean:
	rm -rf `find . -name "*.o"`
	rm -rf `find . -name "*.csv"`
	rm -rf `find . -name "*.txt"`
