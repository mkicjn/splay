#include "src/splay.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

struct record {
	int num;
	struct splay_link link;
};

enum splay_dir record_nav(const struct splay_link *here, const struct splay_link *arg)
{
	struct record *a = SPLAY_CONTAINER(here, struct record, link);
	struct record *b = SPLAY_CONTAINER(arg, struct record, link);

	if (b->num < a->num)
		return LEFT;
	else if (b->num > a->num)
		return RIGHT;
	return HERE;
}

void save_records(struct splay_link *root, int *arr, size_t *len)
{
	struct record *rec = SPLAY_CONTAINER(root, struct record, link);

	if (rec->link.child[LEFT])
		save_records(rec->link.child[LEFT], arr, len);
	arr[(*len)++] = rec->num;
	if (rec->link.child[RIGHT])
		save_records(rec->link.child[RIGHT], arr, len);
}

void splaysort(int *arr, size_t len)
{
	struct record *nodes = malloc(len * sizeof(*nodes));
	struct splay_link *root = NULL;

	for (int i = 0; i < len; i++) {
		struct record *rec = &nodes[i];
		rec->num = arr[i];
		splay_insert(&root, record_nav, &rec->link);
	}

	len = 0;
	save_records(root, arr, &len);

	free(nodes);
}

void shuffle(int *arr, size_t len)
{
	for (int i = 0; i < len; i++) {
		int j = i + (rand() % (len - i));
		int temp = arr[i];
		arr[i] = arr[j];
		arr[j] = temp;
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	size_t num_tests = 1000000;
	if (argc > 1)
		sscanf(argv[1], "%lu", &num_tests);

	int *test_arr = malloc(num_tests * sizeof(*test_arr));
	for (int i = 0; i < num_tests; i++)
		test_arr[i] = i;
	shuffle(test_arr, num_tests);

	clock_t sort_time = clock();
	splaysort(test_arr, num_tests);
	sort_time = clock() - sort_time;
	printf("Splaysort: %fms\n", (sort_time / (double)CLOCKS_PER_SEC * 1000.0));

	for (int i = 0; i < num_tests - 1; i++) {
		if (test_arr[i+1] < test_arr[i]) {
			printf("Sanity check failed\n");
			for (int j = 0; j < num_tests - 1; j++)
				printf("%d\n", test_arr[j]);
			break;
		}
	}

	return 0;
}
