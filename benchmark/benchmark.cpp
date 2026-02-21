#include <stdlib.h>

#include <benchmark/benchmark.h>

#include "mellow/mellow.h"

static void BM_LibcMallocFree(benchmark::State &state)
{
    size_t block_size = static_cast<size_t>(state.range(0));
    for (auto _ : state)
    {
        void *p = malloc(block_size);
        benchmark::DoNotOptimize(p);
        free(p);
    }
}
BENCHMARK(BM_LibcMallocFree)->Arg(64)->Arg(256)->Arg(1024);

static void BM_MwMallocFree(benchmark::State &state)
{
    size_t block_size = static_cast<size_t>(state.range(0));
    for (auto _ : state)
    {
        void *p = mw_malloc(block_size);
        benchmark::DoNotOptimize(p);
        mw_free(p);
    }
}
BENCHMARK(BM_MwMallocFree)->Arg(64)->Arg(256)->Arg(1024);

BENCHMARK_MAIN();
