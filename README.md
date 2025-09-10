## Usage notes

This is a very small (and fast?) implementation of generic splay trees in an "intrusive" style (i.e., you put its structures inside yours instead of vice versa).

Usage notes are extensively documented in `src/splay.h`.

The .c files in the top level directory (especially benchmark.c) serve as examples for how this might be used, but a brief explanation is also provided below for convenience.

There are two steps to use this library:

1. Define your own node struct with a `struct splay_link` member inside it.
2. Define a "navigation" function (essentially a comparator) for your struct.
   * This function takes two pointers to `struct splay_link` and returns `SPLAY_LEFT`, `SPLAY_RIGHT`, or `SPLAY_HERE` depending on how their enclosing structs compare.
   * You can retrieve a pointer to your outer struct with `SPLAY_CONTAINER`, which is essentially a redefinition of Linux's `container_of` provided for you.

Afterwards, you can declare a `struct splay_link *root = NULL;` and call `splay_insert()`, `splay_find()`, and `splay_delete()` to manipulate the tree
(using pointers to `splay_link` members inside structs you allocate and manage yourself).

**Note that these functions take pointers to the `struct splay_link` member of your struct** and rely on your nav function to work properly.
This might seem a little awkward at first, but lends itself to making the API generic and ultra-consistent
(all three functions have the exact same signature, except for a single `const` qualifier missing in `splay_insert()`).

Tip: For working with keys directly, it may be useful to define some shims that instantiate your struct on the stack with whatever it needs to be matched against.
This isn't something the library can do for you, since it doesn't know what you plan to put in your structs (or how you plan to compare them).

Alternatively, you can pass in a pointer to something else as your argument and use a separate nav function that knows what to do with it.
A previous version of the API had `const void *` arguments instead to foreshadow this use case, but this inconsistency in messaging felt undesirable and lacked a clear practical benefit.
(There's some awkward instantiation/pointer-passing/casting nonsense going on either way, so the API might as well be consistent about it; regardless, it's trivial to switch back.)


## Example

```c
    #include "src/splay.h"
    // ^ See here for usage notes
    
    #include <stdio.h>
    #include <string.h>
    
    struct record {
        char *key;
        int val;
        struct splay_link link;
    };
    
    enum splay_dir record_nav(const struct splay_link *link, const struct splay_link *arg)
    {
        struct record *rec1 = SPLAY_CONTAINER(link, struct record, link);
        struct record *rec2 = SPLAY_CONTAINER(arg, struct record, link);
    
        int cmp = strcmp(rec2->key, rec1->key);
        if (cmp < 0)
            return SPLAY_LEFT;
        else if (cmp > 0)
            return SPLAY_RIGHT;
        return SPLAY_HERE;
    }
    
    int record_get(struct splay_link **root, char *key)
    {
        struct record query = {.key = key};
    
        struct splay_link *match = splay_find(root, record_nav, &query.link);
        if (match == NULL)
            return -1;
    
        struct record *result = SPLAY_CONTAINER(match, struct record, link);
        return result->val;
    }
    
    void print_all_records(struct splay_link *root)
    {
        if (root == NULL)
            return;
    
        struct record *rec = SPLAY_CONTAINER(root, struct record, link);
    
        print_all_records(rec->link.child[SPLAY_LEFT]);
        printf("%s: %d\n", rec->key, rec->val);
        print_all_records(rec->link.child[SPLAY_RIGHT]);
    }
```
