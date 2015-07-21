#pragma once

#include <time.h>
#include "Int.h"

#include "ForwardDeclarations.h"

typedef struct _BENCHMARK_CONTEXT 
{
	//Memory
	uint64_t TweetDataMemory;

	//Times
	clock_t Start;
	clock_t DoneReading;
	clock_t DoneInitializing;
	clock_t DoneSorting;
	clock_t DoneWriting;
	
	clock_t QSortTime;
	clock_t QSortStart;

	clock_t NetworkingOverheadTime;
	clock_t NetworkingOverheadStart;
	
	clock_t NetworkingTime;
	clock_t NetworkingStart;
	uint32_t NetworkingExchangeCount;
	
	clock_t BitonicCompareTime;
	clock_t BitonicCompareStart;
	
	clock_t MergeSortTime;
	clock_t MergeSortStart;
	
	
} BENCHMARK_CONTEXT;

void InitBenchmark(PBENCHMARK_CONTEXT BenchmarkContext);
void PrintMemoryConsumption(PPROGRAM_CONTEXT ProgramContext, PBENCHMARK_CONTEXT BenchmarkContext);
void StartBenchmark(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneReading(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneInitializing(PBENCHMARK_CONTEXT BenchmarkContext);
void StartQsort(PBENCHMARK_CONTEXT BenchmarkContext);
void StopQsort(PBENCHMARK_CONTEXT BenchmarkContext);
void StartNetworkingOverhead(PBENCHMARK_CONTEXT BenchmarkContext);
void StopNetworkingOverhead(PBENCHMARK_CONTEXT BenchmarkContext);
void StartNetworking(PBENCHMARK_CONTEXT BenchmarkContext);
void StopNetworking(PBENCHMARK_CONTEXT BenchmarkContext);
void StartBitonicCompare(PBENCHMARK_CONTEXT BenchmarkContext);
void StopBitonicCompare(PBENCHMARK_CONTEXT BenchmarkContext);
void StartMergeSort(PBENCHMARK_CONTEXT BenchmarkContext);
void StopMergeSort(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneSorting(PBENCHMARK_CONTEXT BenchmarkContext);
void DoneWriting(PBENCHMARK_CONTEXT BenchmarkContext);
void PrintTimes(PBENCHMARK_CONTEXT BenchmarkContext);
