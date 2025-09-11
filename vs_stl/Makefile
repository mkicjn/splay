CXXFLAGS=-O3 -Wall -Wextra -pedantic

ALL=benchmark_fair benchmark_idiomatic

all: $(ALL)

%: %.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(ALL)
