#include "splay.h"

/*
 * Move x where p was and swap links around
 * e.g., rotate(&g->child[SPLAY_LEFT], SPLAY_RIGHT);
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

struct splay_link *splay_find(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *arg)
{
	// Top-down splaying strategy
	if (*root_ptr == NULL)
		return NULL;

	struct splay_link *subtree[2] = {NULL, NULL};
	struct splay_link **leaf[2] = {&subtree[0], &subtree[1]};

	for (;;) {
		struct splay_link *g = *root_ptr;
		enum splay_dir dir = nav(g, arg);
		if (dir == SPLAY_HERE)
			break;

		struct splay_link *p = g->child[dir];
		// Special handling for zig-zig case
		if (p != NULL && nav(p, arg) == dir) {
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
	*leaf[SPLAY_LEFT] = g->child[SPLAY_LEFT];
	*leaf[SPLAY_RIGHT] = g->child[SPLAY_RIGHT];
	g->child[SPLAY_LEFT] = subtree[SPLAY_LEFT];
	g->child[SPLAY_RIGHT] = subtree[SPLAY_RIGHT];

	if (nav(g, arg) == SPLAY_HERE)
		return g;
	else
		return NULL;
}

struct splay_link *splay_insert(struct splay_link **root_ptr, splay_nav_fn nav, struct splay_link *arg)
{
	if (*root_ptr == NULL) {
		*root_ptr = arg;
		arg->child[SPLAY_LEFT] = NULL;
		arg->child[SPLAY_RIGHT] = NULL;
		return NULL;
	}

	struct splay_link *collision = splay_find(root_ptr, nav, arg);
	if (collision != NULL)
		return collision;

	struct splay_link *g = *root_ptr;
	int dir = nav(g, arg);
	arg->child[1-dir] = g;
	arg->child[dir] = g->child[dir];
	g->child[dir] = NULL;
	*root_ptr = arg;
	return NULL;
}

static enum splay_dir splay_nav_to_min(const struct splay_link *n, const struct splay_link *unused)
{
	(void)unused;
	if (n->child[SPLAY_LEFT] == NULL)
		return SPLAY_HERE;
	else
		return SPLAY_LEFT;
}

struct splay_link *splay_delete(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *arg)
{
	struct splay_link *del = splay_find(root_ptr, nav, arg);
	if (del == NULL)
		return NULL;

	if (del->child[SPLAY_RIGHT] == NULL) {
		*root_ptr = del->child[SPLAY_LEFT];
		return del;
	} else if (del->child[SPLAY_LEFT] == NULL) {
		*root_ptr = del->child[SPLAY_RIGHT];
		return del;
	}
	*root_ptr = del->child[SPLAY_RIGHT];
	splay_find(root_ptr, splay_nav_to_min, NULL);
	(*root_ptr)->child[SPLAY_LEFT] = del->child[SPLAY_LEFT];
	return del;
}
