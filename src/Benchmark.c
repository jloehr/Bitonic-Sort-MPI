#include "Benchmark.h"

#include <stdio.h>

void StartBenchmark(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->Start = clock();
}

void DoneReadingAndProcessing(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->DoneReadingAndProcessing = clock();
}

void DoneSorting(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->DoneSorting = clock();
}

void DoneWriting(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->DoneWriting = clock();
}

void PrintTimes(PBENCHMARK_CONTEXT BenchmarkContext)
{
	double ReadingAndProcessingTime = (double)(BenchmarkContext->DoneReadingAndProcessing - BenchmarkContext->Start)/CLOCKS_PER_SEC;
	double SortingTime = (double)(BenchmarkContext->DoneSorting - BenchmarkContext->DoneReadingAndProcessing)/CLOCKS_PER_SEC;
	double WritingTime = (double)(BenchmarkContext->DoneWriting - BenchmarkContext->DoneSorting)/CLOCKS_PER_SEC;
	
	printf("Reading & Processing:\t%#10.5fs\n", ReadingAndProcessingTime);
	printf("Sorting:\t\t%#10.5fs\n", SortingTime);
	printf("Writing:\t\t%#10.5fs\n", WritingTime);
}