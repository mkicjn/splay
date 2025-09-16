## Comparison vs. STL

These programs are an application of the same benchmarking procedure to `std::map`... at least, as accurately as reasonably possible.

It's more of a norm in C to avoid memory allocations when possible (as `../benchmark.c` does), but this style is unusual in C++.
Conceptually, `../benchmark.c` represents the use case of a `std::map<std::string, int>`, but a simplistic translation would seem to put C++ at a slight disadvantage.

Therefore, two implementations test this idea:
* `benchmark_fair.cpp` avoids memory allocations to the extent possible by the benchmarking routine
* `benchmark_idiomatic.cpp` does the same task in whatever way is the most convenient in C++


## Results

On author's machine, each benchmark's average lookup performance is *roughly* comparable, but in relative terms place as follows:

1. `./benchmark_idiomatic` (fastest, surprisingly)
2. `./benchmark_fair` (marginally worse, +0.04us)
3. `../benchmark` (marginally worse still, +0.02us)
4. `../benchmark_dynamic` (quite a bit worse somehow, +0.1us)

These rankings seem to vary machine-to-machine, and splay trees have unique tradeoffs that aren't demonstrated at all here, so it's all apples-to-oranges anyway.
It's also interesting to measure the total runtime of the different benchmarks, which results in a completely different ranking somehow as well.

Despite being marginally outperformed, this homegrown splay tree implementation punches way above its weight class, weighing in at ~120 LOC top to bottom.
Meanwhile, GCC's `libstdc++-v3/include/bits/stl_map.h` (which seems to drive `std::map`) weighs in at ~14x the size.
This testing was done under essentially the worst realistic conditions for the splay tree, too: re-accessing all of the nodes in a random order with no chance of repetition.

Theoretically, splay trees should exhibit significantly better amortized performance than std::map on non-uniformly random or non-random accesses, which again, aren't being tested at all yet.
Meanwhile, `std::map` would be expected to exhibit roughly the same amortized performance per access, regardless of any circumstances.

On the other hand, splay trees also have the unique disadvantage of being modified on reads as well as writes.
This is somewhat meaningless for single-threaded applications, but more-or-less makes them unusable in any parallelizable way.
