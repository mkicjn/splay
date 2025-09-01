#ifndef SPLAY_H
#define SPLAY_H

#include <stddef.h>
#include <stdbool.h>

struct splay_link {
	struct splay_link *child[2];
};

enum splay_dir {
	// (used for child indexing)
	SPLAY_LEFT  = 0,
	SPLAY_RIGHT = 1,
	// (not used for child indexing)
	SPLAY_HERE,
};

typedef enum splay_dir (*splay_nav_fn)(const struct splay_link *here, const struct splay_link *nav_arg);

bool splay_find(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *nav_arg);
bool splay_insert(struct splay_link **root_ptr, splay_nav_fn nav, struct splay_link *x);
bool splay_delete(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *nav_arg);

#define SPLAY_CONTAINER(PTR, T, MEMB) ((T *)((char *)(PTR) - ((size_t)(&((T *)0)->MEMB))))

#endif
