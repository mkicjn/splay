#include <stddef.h>
#include <stdbool.h>

struct splay_link {
	struct splay_link *child[2];
};

#define SPLAY_CONTAINER(PTR, T, MEMB) ((T *)((char *)(PTR) - ((size_t)(&((T *)0)->MEMB))))

enum splay_dir {
	// (used for child indexing)
	LEFT  = 0,
	RIGHT = 1,
	// (not used for child indexing)
	HERE,
	NOWHERE,
};

typedef enum splay_dir (*splay_nav_fn)(const struct splay_link *here, const struct splay_link *nav_arg);

/*
 * Move x where p was and swap links around
 * e.g., rotate(&g->child[LEFT], RIGHT);
 *
 *      (g)           (g)
 *      / \           / \
 *     p   d   ->    x   d
 *    / \           / \
 *   a   x         p   c
 *      / \       / \
 *     b   c     a   b
 *
 * All types of splays have this in common
 */
static inline void rotate(struct splay_link **p_ptr, int x_dir)
{
	struct splay_link *p = *p_ptr;
	struct splay_link *x = p->child[x_dir];
	p->child[x_dir] = x->child[1-x_dir];
	x->child[1-x_dir] = p;
	*p_ptr = x;
}

// Top-down splaying strategy
bool splay_find(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *nav_arg)
{
	if (*root_ptr == NULL)
		return false;

	struct splay_link *subtree[2] = {NULL, NULL};
	struct splay_link **leaf[2] = {&subtree[0], &subtree[1]};

	for (;;) {
		struct splay_link *g = *root_ptr;
		enum splay_dir dir = nav(g, nav_arg);
		if (dir == HERE)
			break;

		struct splay_link *p = g->child[dir];
		// Special handling for zig-zig case
		if (p != NULL && nav(p, nav_arg) == dir) {
			g = p;
			rotate(root_ptr, dir);
		}
		// We check if p is NULL after, rather than before,
		// because the rotation could put NULL in its place
		if (g->child[dir] == NULL)
			break;

		*leaf[1-dir] = g;
		leaf[1-dir] = &g->child[dir];
		*root_ptr = g->child[dir];
	}

	struct splay_link *g = *root_ptr;
	*leaf[LEFT] = g->child[LEFT];
	*leaf[RIGHT] = g->child[RIGHT];
	g->child[LEFT] = subtree[LEFT];
	g->child[RIGHT] = subtree[RIGHT];
	return nav(g, nav_arg) == HERE;
}

bool splay_insert(struct splay_link **root_ptr, splay_nav_fn nav, struct splay_link *x)
{
	if (*root_ptr == NULL) {
		*root_ptr = x;
		return true;
	}

	if (splay_find(root_ptr, nav, x))
		return false;

	struct splay_link *g = *root_ptr;
	int dir = nav(g, x);
	x->child[1-dir] = g;
	x->child[dir] = g->child[dir];
	g->child[dir] = NULL;
	*root_ptr = x;
	return true;
}

static enum splay_dir splay_nav_to_min(const struct splay_link *n, const struct splay_link *unused)
{
	(void)unused;
	if (n->child[LEFT] == NULL)
		return HERE;
	else
		return LEFT;
}

bool splay_delete(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *nav_arg)
{
	if (!splay_find(root_ptr, nav, nav_arg))
		return false;
	struct splay_link *del = *root_ptr;

	if (!del->child[RIGHT]) {
		*root_ptr = del->child[LEFT];
		return true;
	} else if (!del->child[LEFT]) {
		*root_ptr = del->child[RIGHT];
		return true;
	}
	*root_ptr = del->child[RIGHT];
	splay_find(root_ptr, splay_nav_to_min, NULL);
	(*root_ptr)->child[LEFT] = del->child[LEFT];
	return true;
}
