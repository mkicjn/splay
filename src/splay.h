#ifndef SPLAY_H
#define SPLAY_H

#include <stddef.h>
#include <stdbool.h>


// Intrusive splay link structure
//
// Usage example:
// ...
// struct my_record { // "User struct"
// 	// User-defined struct members
// 	key_t my_key;
// 	val_t my_value;
// 	// Splay linkage struct required by the API
// 	struct splay_link link; // "Link member" (can be called anything)
// }
// ...
//
// The link field need not be initialized by the user before insertion

struct splay_link {
	struct splay_link *child[2];
};


// A navigation (nav) function must also be provided by the user to compare two user structs
// Each user struct is referenced by a pointer to the link member inside it
//
// Parameters:
// - here: A node currently existing in the tree which forms the basis for the comparison
// - arg: The splay_link pointer argument provided to the API function for comparison
//
// Returns: enum splay_dir value described below
// - SPLAY_HERE if arg and here are equivalent (i.e., arg == here)
// - SPLAY_LEFT if the splaying algorithm should go LEFT from here (i.e., arg < here)
// - SPLAY_RIGHT if the splaying algorithm should go RIGHT from here (i.e., arg > here)

enum splay_dir {
	// (never use for child indexing)
	SPLAY_HERE = -1,
	// (OK to use for child indexing)
	SPLAY_LEFT  = 0,
	SPLAY_RIGHT = 1,
};

typedef enum splay_dir (*splay_nav_fn)(const struct splay_link *here, const struct splay_link *arg);

// To retrieve a pointer to a user struct based on a pointer to its link member, invoke the below macro
//
// Parameters:
// - PTR: A pointer to the link member inside a user struct
// - T: The type of the user struct (e.g., `struct my_record`)
// - MEMB: The name of the link member within the user struct (e.g., `link`)
//
// Expands to: An expression that calculates a user struct pointer from a given link member pointer
#define SPLAY_CONTAINER(PTR, T, MEMB) ((T *)((char *)(PTR) - ((size_t)(&((T *)0)->MEMB))))



// Once a user struct and nav function are implemented, the below API functions become usable:


// splay_insert(): Insert a user struct into a splay tree
//
// Parameters:
// - root_ptr: A pointer to the pointer representing the head of the splay tree
// 	- NOTE: The value of a splay tree head must be initialized to NULL
// - nav: A navigation function provided by the user (see definition of splay_nav_fn)
// - arg: A pointer to the link member inside a user struct to insert into the tree
// 	- NOTE: The user retains ownership over all user struct memory
// 	- NOTE: The contents of a link member need not be initialized before insertion
// 	- NOTE: A user struct must not be inserted into multiple splay trees
//
// Returns NULL on success; pointer to conflicting node's link member on failure
struct splay_link *splay_insert(struct splay_link **root_ptr, splay_nav_fn nav, struct splay_link *arg);


// splay_find(): Search the splay tree, bringing the node to the root
//
// Parameters:
// - root_ptr: A pointer to the pointer representing the head of the splay tree
// - nav: A navigation function provided by the user (see definition of splay_nav_fn)
// - arg: A pointer to the link node inside a user struct matching the desired node (according to nav)
//
// Returns pointer to matching node on success; NULL on failure
struct splay_link *splay_find(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *arg);


// splay_delete(): Remove a user struct from a splay tree
//
// Parameters:
// - root_ptr: A pointer to the pointer representing the head of the splay tree
// - nav: A navigation function provided by the user (see definition of splay_nav_fn)
// - arg: A pointer to the link node inside a user struct matching the desired node (according to nav)
//
// Returns pointer to matching node on success; NULL on failure
struct splay_link *splay_delete(struct splay_link **root_ptr, splay_nav_fn nav, const struct splay_link *arg);

#endif
