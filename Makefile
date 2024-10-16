.PHONY: \
	all \
	venv \
	examples \
	examples-graph \
	format \
	format-cpp \
	format-python \
	format-json \
	lint \
	lint-cpp \
	lint-python \
	clean \
	distclean

all: examples

venv: requirements.txt
	python3 -m venv venv
	./venv/bin/pip3 install --no-cache-dir --requirement requirements.txt

garaza:
	git clone git@github.com:viktorsboroviks/garaza.git
	cd garaza; git checkout v3.3

rododendrs:
	git clone git@github.com:viktorsboroviks/rododendrs.git
	cd rododendrs; git checkout v1.4

examples: examples-graph

examples-graph: graph.o examples/graph_config.json
	./graph.o
	python3 scripts/plot_graph.py \
		--config=examples/graph_config.json \
		--vertices=vertices.csv \
		--edges=edges.csv \
		--output=graph.svg

graph.o: garaza rododendrs examples/graph.cpp
	g++ -Wall -Wextra -Werror -Wpedantic \
		-std=c++20 -O3 \
		-I./include \
		-I./garaza/include \
		-I./rododendrs/include \
		examples/graph.cpp -o $@

format: venv format-cpp format-python format-json

format-cpp: \
		include/grafiins.hpp \
		examples/graph.cpp
	clang-format -i $^

format-python: scripts/plot_graph.py
	source ./venv/bin/activate; \
		black $^

format-json: scripts/plot_graph_config.schema.json
	jq . \
		scripts/plot_graph_config.schema.json \
		| sponge \
		scripts/plot_graph_config.schema.json

lint: lint-cpp lint-python

lint-cpp: include/grafiins.hpp
	cppcheck \
		--enable=warning,portability,performance \
		--enable=style,information \
		--enable=missingInclude \
		--inconclusive \
		--library=std,posix,gnu \
		--platform=unix64 \
		--language=c++ \
		--std=c++20 \
		--inline-suppr \
		--check-level=exhaustive \
		--suppress=missingIncludeSystem \
		--suppress=checkersReport \
		--checkers-report=cppcheck_report.txt \
		-I./include \
		-I./garaza/include \
		-I./rododendrs/include \
		$^

lint-python: scripts/plot_graph.py
	source ./venv/bin/activate; \
		pylint $^; \
		flake8 $^

clean:
	rm -rf `find . -name "*.o"`
	rm -rf `find . -name "*.csv"`
	rm -rf `find . -name "*.svg"`
	rm -rf `find . -name "*.png"`
	rm -rf `find . -name "*.txt"`

distclean: clean
	rm -rf garaza
	rm -rf rododendrs
