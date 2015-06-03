#pragma once

#include <stdint.h>
#include <time.h>

typedef struct _BENCHMARK_CONTEXT 
{
	clock_t Start;
	clock_t DoneReadingAndProcessing;
	clock_t DoneSorting;
	clock_t DoneWriting;
	
} BENCHMARK_CONTEXT, * PBENCHMARK_CONTEXT;

void StartBenchmark(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneReadingAndProcessing(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneSorting(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneWriting(PBENCHMARK_CONTEXT BenchmarkContext);
void PrintTimes(PBENCHMARK_CONTEXT BenchmarkContext);