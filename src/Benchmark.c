#include "Benchmark.h"

#include <stdio.h>
#include <wchar.h>

#include "Program.h"

void InitBenchmark(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->TweetDataMemory = 0;
	BenchmarkContext->UnicodeAppearanceMemory = 0;
	BenchmarkContext->UnicodeAppearanceFields = 0;
}

void PrintMemoryConsumption(PPROGRAM_CONTEXT ProgramContext, PBENCHMARK_CONTEXT BenchmarkContext)
{
	uint64_t Total = ProgramContext->TweetStringsSize;
	Total += BenchmarkContext->TweetDataMemory;
	Total += BenchmarkContext->UnicodeAppearanceMemory;
	
	wprintf(L"\n");
	wprintf(L"Tweet String Memory:\t%12"PRIu64"\n", ProgramContext->TweetStringsSize);
	wprintf(L"Tweet Data Memory:\t%12"PRIu64"\n", BenchmarkContext->TweetDataMemory);
	wprintf(L"Unicode Appearance Memory:%10"PRIu64"\n", BenchmarkContext->UnicodeAppearanceMemory);
	wprintf(L"Unicode Appearance Fields:%10"PRIu64"\n", BenchmarkContext->UnicodeAppearanceFields);
	wprintf(L"------------------------------------\n");
	wprintf(L"Total:\t\t\t%12"PRIu64"\n", Total);
	wprintf(L"\n");
}

void StartBenchmark(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->Start = clock();
}

void DoneReading(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->DoneReading = clock();
}

void DoneInitializing(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->DoneInitializing = clock();
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
	double ReadingTime = (double)(BenchmarkContext->DoneReading - BenchmarkContext->Start)/CLOCKS_PER_SEC;
	double InitializingTime = (double)(BenchmarkContext->DoneInitializing - BenchmarkContext->DoneReading)/CLOCKS_PER_SEC;
	double SortingTime = (double)(BenchmarkContext->DoneSorting - BenchmarkContext->DoneInitializing)/CLOCKS_PER_SEC;
	double WritingTime = (double)(BenchmarkContext->DoneWriting - BenchmarkContext->DoneSorting)/CLOCKS_PER_SEC;
	double TotalTime = (double)(BenchmarkContext->DoneWriting - BenchmarkContext->Start)/CLOCKS_PER_SEC;

	wprintf(L"Reading:\t%#10.5fs\n", ReadingTime);
	wprintf(L"Initializing:\t%#10.5fs\n", InitializingTime);
	wprintf(L"Sorting:\t%#10.5fs\n", SortingTime);
	wprintf(L"Writing:\t%#10.5fs\n", WritingTime);
	wprintf(L"Total:\t\t%#10.5fs\n", TotalTime);
}