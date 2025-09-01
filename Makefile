CFLAGS=-O3

ALL=test sort

all: $(ALL)

%: %.c src/splay.c
	$(CC) $(CFLAGS) $^ -o $@

.PHONY: clean
clean:
	rm -f $(ALL)
