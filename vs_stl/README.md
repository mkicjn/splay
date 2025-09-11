## Comparison vs. STL

These programs are an application of the same benchmarking procedure to `std::map`... at least, as accurately as reasonably possible.

It's more of a norm in C to avoid memory allocations when possible (as `../benchmark.c` does), but this style is unusual in C++.
Conceptually, `../benchmark.c` represents the use case of a `std::map<std::string, int>`, but a simplistic translation would seem to put C++ at a slight disadvantage.

Therefore, two implementations test this idea:
* `benchmark_fair.cpp` avoids memory allocations to the extent possible by the benchmarking routine
* `benchmark_idiomatic.cpp` does the same task in whatever way is the most convenient in C++


## Results

On author's machine, each benchmark's performance is *roughly* comparable, but in relative terms place as follows:

1. `./benchmark_idiomatic` (fastest, surprisingly)
2. `./benchmark_fair` (marginally worse)
3. `../benchmark` (marginally worse still)
4. `../benchmark_dynamic` (quite a bit worse somehow)

Despite being marginally outperformed, this homegrown splay tree implementation punches way above its weight class, weighing in at ~120 LOC top to bottom.
Meanwhile, GCC's `libstdc++-v3/include/bits/stl_map.h` (which seems to drive `std::map`) weighs in at ~14x the size.

Actual measurements were omitted above, but because splay trees have unique advantages and disadvantages that aren't being demonstrated here at all, it's apples-to-oranges anyway.

Namely, splay trees should perform much better with non-uniformly random access patterns (even completely sequential ones), which again, aren't (yet) being tested at all.
Meanwhile, `std::map` would be expected to exhibit roughly the same performance per access, regardless of whether accesses are uniform or not.

On the other hand, splay trees also have the unique disadvantage of being modified on reads as well as writes.
This is somewhat meaningless for single-threaded applications, but more-or-less makes them unusable in any parallelizable way.
