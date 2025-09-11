// Application of same benchmarking procedure to std::map... this time, in a more "idiomatic" style
//
// Conceptually, the benchmark tests the use case of std::map<std::string, int>, but a simplistic translation puts C++ at a slight disadvantage
// For a fairer comparison, look at benchmark_fair.cpp

#include <iostream>
#include <vector>

#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <ctime>

////////////////////////////////////////////////////////////////////////////////

#include <map>

#define KEYLEN 32

////////////////////////////////////////////////////////////////////////////////

std::string rand_string()
{
#define MINLEN 20
	size_t len = MINLEN + (rand() % (KEYLEN-1-MINLEN));
#define CONSONANTS "bcdfghjklmnpqrstvwxyz"
#define VOWELS "aeiou"
	std::string key(len, '\0');
	for (size_t i = 0; i < len; i++) {
		char c;
		if (i % 2 == 0) {
			c = CONSONANTS[rand() % (sizeof(CONSONANTS)-1)];
		} else {
			c = VOWELS[rand() % (sizeof(VOWELS)-1)];
		}
		key[i] = c;
	}
	return key;
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

void print_records(std::map<std::string, int> &map)
{
	for (const auto &kvp : map) {
		auto k = kvp.first;
		auto v = kvp.second;
		std::cout << k << ": " << v << std::endl;
	}
}

int main(int argc, char **argv)
{
	srand(time(nullptr));

	size_t num_tests = 1000000;
	if (argc > 1)
		sscanf(argv[1], "%lu", &num_tests);

	std::map<std::string, int> map;

	std::cout << "Generating and inserting " << num_tests << " random string/integer pairs" << std::endl;

	// Measure time to insert nodes
	std::vector<std::string> test_keys;
	clock_t insert_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		std::string key = rand_string();
		int val = rand();
		if (map.count(key)) {
			std::cout << "Insertion failed (duplicate)" << std::endl;
			std::cout << "Word was: " << key << std::endl;
		}
		map[key] = val;
		test_keys.push_back(key);
	}
	insert_time = clock() - insert_time;
	if (argc > 2)
		print_records(map);
	std::cout << "Average generation + insertion time: " << ((insert_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests) << "ms" << std::endl;

	// Generate a shuffled array of indices for testing randomized lookup time
	int *rand_indices = new int[num_tests];
	for (size_t i = 0; i < num_tests; i++)
		rand_indices[i] = i;
	shuffle(rand_indices, num_tests);

	// Measure the time to do lookups in that randomized order
	clock_t find_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		std::string &key = test_keys[rand_indices[i]];
		auto match = map.find(key);
		if (match == map.end())
			std::cout << "Find failed" << std::endl;
	}
	find_time = clock() - find_time;
	std::cout << "Average find time (random order): " << ((find_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests) << "ms" << std::endl;

	// Shuffle the index array again
	shuffle(rand_indices, num_tests);

	// Measure the time to delete each item in that order
	clock_t delete_time = clock();
	for (size_t i = 0; i < num_tests; i++) {
		std::string &key = test_keys[rand_indices[i]];
		auto match = map.find(key);
		if (match == map.end())
			std::cout << "Find failed" << std::endl;
		map.erase(match);
	}
	delete_time = clock() - delete_time;
	std::cout << "Average delete time (random order): " << ((delete_time / (double)CLOCKS_PER_SEC * 1000.0) / num_tests) << "ms" << std::endl;

	delete[] rand_indices;
	if (map.size() > 0)
		printf("Unexpected nodes remaining in splay tree\n");
	return 0;
}
