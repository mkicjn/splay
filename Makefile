CFLAGS=-O3

ALL=benchmark sorting_test

all: $(ALL)

%: %.c src/splay.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(ALL)
