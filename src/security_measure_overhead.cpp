#include <chrono>
#include <cstdio>
#include <cstdlib>

constexpr std::size_t ITERS = 100'000'000;

__attribute__((noinline)) void empty() { asm volatile("" ::: "memory"); }

int main() {
    auto bench = [](const char *msg) {
        auto t0 = std::chrono::high_resolution_clock::now();
        for (std::size_t i = 0; i < ITERS; ++i) empty();
        auto t1 = std::chrono::high_resolution_clock::now();
        double ms = std::chrono::duration<double,std::milli>(t1 - t0).count();
        std::printf("%-25s %.2f ms\n", msg, ms);
    };

    bench("loop + ret");

    // malloc / free stress (alloc-heavy code paths)
    auto t0 = std::chrono::high_resolution_clock::now();
    for (std::size_t i = 0; i < ITERS/100; ++i) {
        void *p = std::malloc(128);
        empty();
        std::free(p);
    }
    auto t1 = std::chrono::high_resolution_clock::now();
    std::printf("%-25s %.2f ms\n", "malloc/free", 
        std::chrono::duration<double,std::milli>(t1 - t0).count());
    return 0;
}

/*
Build two variants & time:

# Hardening OFF
$ g++ -O2 -o overhead_no_prot security_measure_overhead.cpp
$ /usr/bin/time -f "%e s" ./overhead_no_prot

# Hardening ON (stack canaries, FORTIFY, RELRO, etc.)
$ g++ -O2 -fstack-protector-all -D_FORTIFY_SOURCE=2 \
      -Wl,-z,relro,-z,now -pie -o overhead_hardened security_measure_overhead.cpp
$ /usr/bin/time -f "%e s" ./overhead_hardened

Typical desktop hardware shows <1 % slowdown for these micro‑benchmarks.
*/
