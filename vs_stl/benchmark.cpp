#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////

#include <map>

#define KEYLEN 32

struct Record {
	char key[KEYLEN];
	int val;
};

bool compare_records(char *const &a, char *const &b)
{
	return strcmp(b, a) < 0;
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

void print_records(std::map<char *, Record *, decltype(&compare_records)> &map)
{
	for (const auto &kvp : map) {
		auto k = kvp.first;
		auto v = kvp.second;
		printf("%s: %d\n", k, v->val);
	}
}

int main(int argc, char **argv)
{
	srand(time(NULL));

	size_t num_tests = 1000000;
	if (argc > 1)
		sscanf(argv[1], "%lu", &num_tests);

	Record *test_nodes = new Record[num_tests];
	size_t next_rec = 0;

	std::map<char *, Record *, decltype(&compare_records)> map(&compare_records);

	printf("Generating and inserting %lu random string/integer pairs\n", num_tests);

	// Measure time to insert nodes
	clock_t insert_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		Record *rec = &test_nodes[next_rec++];
		rand_string(rec->key);
		rec->val = rand();
		if (map.count(rec->key)) {
			printf("Insertion failed (duplicate)\n");
			printf("Word was: %s\n", rec->key);
		}
		map[rec->key] = rec;
	}
	insert_time = clock() - insert_time;
	if (argc > 2)
		print_records(map);
	printf("Average generation + insertion time: %fms\n", (insert_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests);

	// Generate a shuffled array of indices for testing randomized lookup time
	int *rand_indices = new int[num_tests];
	for (size_t i = 0; i < num_tests; i++)
		rand_indices[i] = i;
	shuffle(rand_indices, num_tests);

	// Measure the time to do lookups in that randomized order
	clock_t find_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		Record *rec = &test_nodes[rand_indices[i]];
		auto match = map.find(rec->key);
		if (match == map.end())
			printf("Find failed\n");
	}
	find_time = clock() - find_time;
	printf("Average find time (random order): %fms\n", (find_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests);

	// Shuffle the index array again
	shuffle(rand_indices, num_tests);

	// Measure the time to delete each item in that order
	clock_t delete_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		Record *rec = &test_nodes[rand_indices[i]];
		auto match = map.find(rec->key);
		if (match == map.end())
			printf("Find failed\n");
		map.erase(match);
	}
	delete_time = clock() - delete_time;
	printf("Average delete time (random order): %fms\n", (delete_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests);

	delete[] rand_indices;
	delete[] test_nodes;
	return 0;
}
