CFLAGS=-O3 -Wall -Wextra -pedantic

ALL=benchmark benchmark_dynamic sorting_test

all: $(ALL)

%: %.c src/splay.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(ALL)
