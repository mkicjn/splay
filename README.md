## Usage notes

This is a very small (and fast?) implementation of generic splay trees in an "intrusive" style (i.e., you put its structures inside yours instead of vice versa).

The .c files in the top level directory (especially benchmark.c) serve as examples for how this might be used, but a brief explanation is provided below.

There are two steps to use this library:

1. Define your own node struct with a `struct splay_link` member inside it.
2. Define a "navigation" function (essentially a comparator) for your struct.
   * This function takes two pointers to `struct splay_link` and returns `LEFT`, `RIGHT`, or `HERE` depending on how they compare.
   * You can retrieve a pointer to your outer struct with `SPLAY_CONTAINER`, which is essentially a definition of `container_of` provided for you.

Afterwards, you can declare a `struct splay_link *root = NULL;` and call `splay_insert()`, `splay_find()`, and `splay_delete()` to manipulate the tree.
Functions signatures can be found in `src/splay.h`.
**Note that these functions take pointers to the `struct splay_link` member of your struct** and rely on your nav function to work properly.

For working with keys directly, it may be useful to define some shims that instantiate your struct with whatever it needs to be compared.
This isn't something the library can do for you, since it doesn't know what you plan to put in your structs (or how you plan to compare them).

(**TO-DO:** More detailed notes)
