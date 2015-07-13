#include "Benchmark.h"

#include <stdio.h>
#include <wchar.h>

#include "Program.h"
#include "Tweet.h"

void InitBenchmark(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->TweetDataMemory = 0;
	BenchmarkContext->QSortTime = 0;
	BenchmarkContext->NetworkingOverheadTime = 0;
	BenchmarkContext->NetworkingTime = 0;
	BenchmarkContext->NetworkingExchangeCount = 0;
	BenchmarkContext->BitonicCompareTime = 0;
	BenchmarkContext->MergeSortTime = 0;
}

void PrintMemoryConsumption(PPROGRAM_CONTEXT ProgramContext, PBENCHMARK_CONTEXT BenchmarkContext)
{
	uint64_t Total = ProgramContext->TweetStringsSize;
	Total += 2*BenchmarkContext->TweetDataMemory;
	
	wprintf(L"\n");
	wprintf(L"Tweet String Memory:\t%12"PRIu64"\n", ProgramContext->TweetStringsSize);
	wprintf(L"Tweet Data Memory:\t%12"PRIu64"\n", 2*BenchmarkContext->TweetDataMemory);
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

void StartQsort(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->QSortStart = clock();
}

void StopQsort(PBENCHMARK_CONTEXT BenchmarkContext)
{
	clock_t QsortEnd = clock();
	
	BenchmarkContext->QSortTime += QsortEnd - BenchmarkContext->QSortStart;
}
void StartNetworkingOverhead(PBENCHMARK_CONTEXT BenchmarkContext);
void StopNetworkingOverhead(PBENCHMARK_CONTEXT BenchmarkContext);

void StartNetworkingOverhead(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->NetworkingOverheadStart = clock();
}

void StopNetworkingOverhead(PBENCHMARK_CONTEXT BenchmarkContext)
{
	clock_t NetworkingOverheadEnd = clock();

	BenchmarkContext->NetworkingOverheadTime += NetworkingOverheadEnd - BenchmarkContext->NetworkingOverheadStart;
}

void StartNetworking(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->NetworkingStart = clock();
	BenchmarkContext->NetworkingExchangeCount++;
}

void StopNetworking(PBENCHMARK_CONTEXT BenchmarkContext)
{
	clock_t NetworkingEnd = clock();
	
	BenchmarkContext->NetworkingTime += NetworkingEnd - BenchmarkContext->NetworkingStart;
}

void StartBitonicCompare(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->BitonicCompareStart = clock();
}

void StopBitonicCompare(PBENCHMARK_CONTEXT BenchmarkContext)
{
	clock_t BitonicCompareEnd = clock();
	
	BenchmarkContext->BitonicCompareTime += BitonicCompareEnd - BenchmarkContext->BitonicCompareStart;
}

void StartMergeSort(PBENCHMARK_CONTEXT BenchmarkContext)
{
	BenchmarkContext->MergeSortStart = clock();
}

void StopMergeSort(PBENCHMARK_CONTEXT BenchmarkContext)
{
	clock_t MergeSortEnd = clock();
	
	BenchmarkContext->MergeSortTime += MergeSortEnd - BenchmarkContext->MergeSortStart;
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
	double QSortTime = (double)BenchmarkContext->QSortTime / CLOCKS_PER_SEC;
	double NetworkingOverheadTime = (double)BenchmarkContext->NetworkingOverheadTime / CLOCKS_PER_SEC;
	double NetworkingTime = (double)BenchmarkContext->NetworkingTime / CLOCKS_PER_SEC;
	double TimePerExchange = NetworkingTime / BenchmarkContext->NetworkingExchangeCount;
	double BitonicCompareTime = (double)BenchmarkContext->BitonicCompareTime/CLOCKS_PER_SEC;
	double MergeSortTime = (double)BenchmarkContext->MergeSortTime/CLOCKS_PER_SEC;
	double SortingTime = (double)(BenchmarkContext->DoneSorting - BenchmarkContext->DoneInitializing)/CLOCKS_PER_SEC;
	double WritingTime = (double)(BenchmarkContext->DoneWriting - BenchmarkContext->DoneSorting)/CLOCKS_PER_SEC;
	double TotalTime = (double)(BenchmarkContext->DoneWriting - BenchmarkContext->Start)/CLOCKS_PER_SEC;

	wprintf(L"Reading:\t%#10.5f\n", ReadingTime);
	wprintf(L"Initializing:\t%#10.5f\n", InitializingTime);
	wprintf(L"Sorting:\t%#10.5f\n", SortingTime);
	wprintf(L"- Qsort:\t%#10.5f\n", QSortTime);
	wprintf(L"- Net Ovrhd:\t%#10.5f\n", NetworkingOverheadTime);
	wprintf(L"- Networking:\t%#10.5f\n", NetworkingTime);
	wprintf(L"- Single Net:\t%#10.5f\n", TimePerExchange);
	wprintf(L"- Bitonic:\t%#10.5f\n", BitonicCompareTime - NetworkingTime - MergeSortTime);
	wprintf(L"- MergeSort:\t%#10.5f\n", MergeSortTime);
	wprintf(L"- Bare Sort:\t%#10.5f\n", SortingTime - NetworkingTime - NetworkingOverheadTime);
	wprintf(L"Writing:\t%#10.5f\n", WritingTime);
	wprintf(L"Total:\t\t%#10.5f\n", TotalTime);
}