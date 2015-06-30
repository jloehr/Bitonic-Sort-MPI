#include <sys/mman.h>
#include <mpi.h>
#include <stdio.h>
#include <stdlib.h>

#include <locale.h>

#include "ErrorCodes.h" 

#include "Program.h"
#include "Node.h"
#include "Benchmark.h"
#include "Tweet.h"
#include "FileReader.h"
#include "TweetParsing.h"
#include "BitonicSort.h"
#include "FileWriter.h"


int main(int argc, char * argv[])
{   
    setlocale(LC_ALL, "");
    
    MPI_Init(&argc, &argv);

    PROGRAM_CONTEXT ProgramContext; 
    int Status;

    //Init Program & Node -> ParseArguments and set NodeID
    Status = InitProgramContext(&ProgramContext, argc, argv);
    if(Status != NO_ERROR)
    {
    	return Status;
    }
    
    if(IsMasterNode(&(ProgramContext.NodeContext)))
    {
		wprintf(L"Search Term:\t %S\n", ProgramContext.SearchTerm);
        wprintf(L"File:\t\t %s\n", ProgramContext.Filename);
        wprintf(L"Tweet Count:\t %d\n",ProgramContext.TotalAmountOfTweets);
        wprintf(L"Tweets Per Node: %"PRIu64"\n", ProgramContext.NodeContext.ElementsPerNode);
        wprintf(L"Size per Tweet:\t %llu\n", sizeof(TWEET)); 
	    wprintf(L"\n");
    }

    StartBenchmark(&ProgramContext.NodeContext.BenchmarkContext);
    
    //Read Files
    Status = ReadInTweets(&ProgramContext);
    if(Status != NO_ERROR)
    {
    	return Status;
    }
    
    if(IsMasterNode(&(ProgramContext.NodeContext)))
    {   
        wprintf(L"Max Tweet Size:\t %d\n", ProgramContext.MaxTweetSize); 
	    wprintf(L"\n");
    }
    
    DoneReading(&ProgramContext.NodeContext.BenchmarkContext);
    
    //Parse Tweets
    Status = ParseTweets(&ProgramContext);
    if(Status != NO_ERROR)
    {
    	return Status;
    }
    
    PrintMemoryConsumption(&ProgramContext, &ProgramContext.NodeContext.BenchmarkContext);
    
    DoneInitializing(&ProgramContext.NodeContext.BenchmarkContext);
    
	mlock(ProgramContext.NodeContext.Data, ProgramContext.NodeContext.ElementsPerNode * sizeof(TWEET));
    
    //Start Bitonic Sort
    Sort(&ProgramContext);
    
    DoneSorting(&ProgramContext.NodeContext.BenchmarkContext);
    
    //Write Results to file
    Status = WriteOutResults(&ProgramContext);
    if(Status != NO_ERROR)
    {
    	return Status;
    }
    
	munlock(ProgramContext.NodeContext.Data, ProgramContext.NodeContext.ElementsPerNode * sizeof(TWEET));
    
    DoneWriting(&ProgramContext.NodeContext.BenchmarkContext);
    PrintTimes(&ProgramContext.NodeContext.BenchmarkContext);
    
    FinalizeProgramContext(&ProgramContext);
    MPI_Finalize();
    
	return NO_ERROR;
}