.PHONY: \
	all \
	examples \
	examples_graph \
	format \
	clang-format \
	black \
	clean

all: examples

garaza:
	git clone git@github.com:viktorsboroviks/garaza.git
	cd garaza; git checkout v3.0

rododendrs:
	git clone git@github.com:viktorsboroviks/rododendrs.git
	cd rododendrs; git checkout v1.1

examples: \
	examples_graph

examples_graph: graph.o examples/graph_config.json
	./graph.o
	python3 scripts/plot_graph.py \
		examples/graph_config.json \
		vertices.csv \
		edges.csv \
		graph.svg

graph.o: garaza rododendrs examples/graph.cpp
	g++ -Wall -Wextra -Werror -Wpedantic \
		-std=c++20 -O3 \
		-I./include \
		-I./garaza/include \
		-I./rododendrs/include \
		examples/graph.cpp -o $@

format: clang-format black

clang-format: \
		include/grafiins.hpp \
		examples/graph.cpp
	clang-format -i $^

black: scripts/plot_graph.py
	black $^

clean:
	rm -rf `find . -name "*.o"`
	rm -rf `find . -name "*.csv"`
	rm -rf `find . -name "*.svg"`

distclean: clean
	rm -rf garaza
	rm -rf rododendrs
