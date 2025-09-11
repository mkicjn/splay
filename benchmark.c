#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////

#include "src/splay.h"

#define KEYLEN 32

struct record {
	char key[KEYLEN];
	int val;
	struct splay_link link;
};

enum splay_dir record_nav(const struct splay_link *arg, const struct splay_link *tree_node)
{
	struct record *a = SPLAY_CONTAINER(arg, struct record, link);
	struct record *b = SPLAY_CONTAINER(tree_node, struct record, link);

	int cmp = strcmp(a->key, b->key);
	if (cmp < 0)
		return SPLAY_LEFT;
	else if (cmp > 0)
		return SPLAY_RIGHT;
	return SPLAY_HERE;
}

////////////////////////////////////////////////////////////////////////////////

void rand_string(char key[KEYLEN])
{
#define MINLEN 20
	size_t len = MINLEN + (rand() % (KEYLEN-1-MINLEN));
#define CONSONANTS "bcdfghjklmnpqrstvwxyz"
#define VOWELS "aeiou"
	for (size_t i = 0; i < len; i++) {
		char c;
		if (i % 2 == 0) {
			c = CONSONANTS[rand() % (sizeof(CONSONANTS)-1)];
		} else {
			c = VOWELS[rand() % (sizeof(VOWELS)-1)];
		}
		key[i] = c;
	}
	key[len] = '\0';
}

void shuffle(int *arr, size_t len)
{
	for (size_t i = 0; i < len - 2; i++) {
		int j = i + (rand() % (len - i));
		int temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
}

void print_records(struct splay_link *root)
{
	if (!root)
		return;
	struct record *rec = SPLAY_CONTAINER(root, struct record, link);

	print_records(rec->link.child[SPLAY_LEFT]);
	printf("%s: %d\n", rec->key, rec->val);
	print_records(rec->link.child[SPLAY_RIGHT]);
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	size_t num_tests = 1000000;
	if (argc > 1)
		sscanf(argv[1], "%lu", &num_tests);

	struct record *test_nodes = malloc(num_tests * sizeof(*test_nodes));
	size_t next_rec = 0;

	struct splay_link *root = NULL;

	printf("Initializing node pool with random bytes...\n");
	for (size_t i = 0; i < num_tests * sizeof(*test_nodes); i++)
		((char *)test_nodes)[i] = rand() % 0xff;

	printf("Generating and inserting %lu random string/integer pairs\n", num_tests);

	// Measure time to insert nodes
	clock_t insert_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		struct record *rec = &test_nodes[next_rec++];
		rand_string(rec->key);
		rec->val = rand();
		if (splay_insert(&root, record_nav, &rec->link) != NULL) {
			printf("Insertion failed (duplicate)\n");
			printf("Word was: %s\n", rec->key);
		}
	}
	insert_time = clock() - insert_time;
	if (argc > 2)
		print_records(root);
	printf("Average generation + insertion time: %fms\n", (insert_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests);

	// Generate a shuffled array of indices for testing randomized lookup time
	int *rand_indices = malloc(num_tests * sizeof(*rand_indices));
	for (size_t i = 0; i < num_tests; i++)
		rand_indices[i] = i;
	shuffle(rand_indices, num_tests);

	// Measure the time to do lookups in that randomized order
	clock_t find_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		struct record *rec = &test_nodes[rand_indices[i]];
		if (!splay_find(&root, record_nav, &rec->link))
			printf("Find failed\n");
	}
	find_time = clock() - find_time;
	printf("Average find time (random order): %fms\n", (find_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests);

	// Shuffle the index array again
	shuffle(rand_indices, num_tests);

	// Measure the time to delete each item in that order
	clock_t delete_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		struct record *rec = &test_nodes[rand_indices[i]];
		if (!splay_delete(&root, record_nav, &rec->link)) {
			printf("Deletion failed\n");
			printf("Word was: %s\n", rec->key);
		}
	}
	delete_time = clock() - delete_time;
	printf("Average delete time (random order): %fms\n", (delete_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests);

	free(rand_indices);
	free(test_nodes);
	if (root != NULL)
		printf("Unexpected nodes remaining in splay tree\n");
	return 0;
}
