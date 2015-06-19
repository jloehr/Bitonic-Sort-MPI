#pragma once

#include <time.h>
#include "Int.h"

typedef struct _BENCHMARK_CONTEXT 
{
	//Memory
	uint64_t TweetArrayMemory;
	uint64_t TweetStringsMemory;
	uint64_t TweetDataMemory;
	uint64_t UnicodeAppearanceMemory;
	uint64_t UnicodeAppearanceFields;
	
	//Times
	clock_t Start;
	clock_t DoneReading;
	clock_t DoneInitializing;
	clock_t DoneSorting;
	clock_t DoneWriting;
	
} BENCHMARK_CONTEXT, * PBENCHMARK_CONTEXT;

void InitBenchmark(PBENCHMARK_CONTEXT BenchmarkContext);
void PrintMemoryConsumption(PBENCHMARK_CONTEXT BenchmarkContext);
void StartBenchmark(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneReading(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneInitializing(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneSorting(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneWriting(PBENCHMARK_CONTEXT BenchmarkContext);
void PrintTimes(PBENCHMARK_CONTEXT BenchmarkContext);