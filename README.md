## Usage notes

This is a very small (and fast?) implementation of generic splay trees in an "intrusive" style (i.e., you put its structures inside yours instead of vice versa).

The .c files in the top level directory (especially benchmark.c) serve as examples for how this might be used, but a brief explanation is provided below.

There are two steps to use this library:

1. Define your own node struct with a `struct splay_link` member inside it.
2. Define a "navigation" function (essentially a comparator) for your struct.
   * This function takes two pointers to `struct splay_link` and returns `SPLAY_LEFT`, `SPLAY_RIGHT`, or `SPLAY_HERE` depending on how they compare.
   * You can retrieve a pointer to your outer struct with `SPLAY_CONTAINER`, which is essentially a definition of `container_of` provided for you.

Afterwards, you can declare a `struct splay_link *root = NULL;` and call `splay_insert()`, `splay_find()`, and `splay_delete()` to manipulate the tree
(using pointers to `splay_link` members inside structs you allocate yourself).

Functions signatures can be found in `src/splay.h`.
**Note that these functions take pointers to the `struct splay_link` member of your struct** and rely on your nav function to work properly.
This might seem a little awkward, but lends itself to making the API ultra-consistent (all three functions have the exact same signature, except for a single `const` qualifier missing in `splay_insert()`).

For working with keys directly, it may be useful to define some shims that instantiate your struct on the stack with whatever it needs to be comparable.
This isn't something the library can do for you, since it doesn't know what you plan to put in your structs (or how you plan to compare them).
Alternatively, you can pass in a pointer to something else as your argument and use a separate nav function that knows what to do with it.
A previous version of the API had `const void *` arguments instead to foreshadow this use case, but this inconsistency felt undesirable and lacked a clear practical benefit.
(There's some awkward instantiation/pointer-passing/casting nonsense going on either way, so might as well be consistent about it; regardless, it's trivial to switch back.)


## Example

```c
    #include "src/splay.h"

    // First, declare a user structure with desired data types
    struct record {
        char *key;
        int val;
        // Must include a splay_link member to be usable (can be named anything)
        struct splay_link link;
    };

    // Next, tell the splaying algorithm how to navigate the tree structure
    // (i.e., how to compare node data to one other)
    enum splay_dir record_nav(const struct splay_link *link, const struct splay_link *arg)
    {
        // Use SPLAY_CONTAINER to recover user structs from splay_link pointers. Arguments are as follows:
        // 1. Pointer to splay_link
        // 2. Name of user struct
        // 3. Name of splay_link member in user struct
        struct record *rec1 = SPLAY_CONTAINER(link, struct record, link);
        struct record *rec2 = SPLAY_CONTAINER(arg, struct record, link);

        // In this example, we just compare strings, but obviously you can do whatever you want.
        // The first argument to this function is the current node in our traversal.
        // The second argument is whatever was passed into the API function (insert/find/delete).
        int cmp = strcmp(rec2->key, rec1->key);
        if (cmp < 0)
            return SPLAY_LEFT; // rec2 is less than rec1; go left
        else if (cmp > 0)
            return SPLAY_RIGHT; // rec2 is greater than rec1; go right
        return SPLAY_HERE; // must be equal
    }

    int record_get(struct splay_link **root, char *key)
    {
        // Included to show example usage.
        // Also showcases one of the main idiosyncrasies of this API:
        // If splay_find() returns true, the sought node is moved to the root.
        // Other functions behave as expected.
        struct record query = {.key = key};
        if (!splay_find(root, record_nav, &query.link))
            return -1;
        struct record *result = SPLAY_CONTAINER(*root, struct record, link);
        return result->val;
    }
```
