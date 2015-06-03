#include <mpi.h>
#include <stdio.h>

#include <locale.h> 

#include "Program.h"
#include "Node.h"
#include "TweetParsing.h"
#include "Benchmark.h"

#include "Tweet.h"

int main(int argc, char * argv[])
{   
    setlocale(LC_ALL, "");
    
    MPI_Init(&argc, &argv);

    PROGRAM_CONTEXT ProgramContext; 
    int Status;

    //Init Program & Node -> ParseArguments and set NodeID
    Status = InitProgramContext(&ProgramContext, argc, argv);
    if(Status != 0)
    {
    	return Status;
    }
    
    if(IsMasterNode(&(ProgramContext.NodeContext)))
    {
        printf("File: %s\nTweet Count: %d\n", ProgramContext.Filename, (ProgramContext.TweetsPerFile * ProgramContext.NumberOfFiles));
    }

    StartBenchmark(&ProgramContext.NodeContext.BenchmarkContext);
    
    //Parse File
    Status = ParseTweets(&ProgramContext);
    if(Status != 0)
    {
    	return Status;
    }
    
    DoneReadingAndProcessing(&ProgramContext.NodeContext.BenchmarkContext);
    
    //Start Bitonic Sort
  
    
    DoneSorting(&ProgramContext.NodeContext.BenchmarkContext);
    
    if(IsMasterNode(&(ProgramContext.NodeContext)))
    {
        for(int i = 0; i < 10; i++)
        {
            PrintTweet(&ProgramContext.NodeContext.Data[i]);
        }
    }
    
    DoneWriting(&ProgramContext.NodeContext.BenchmarkContext);
    PrintTimes(&ProgramContext.NodeContext.BenchmarkContext);
    
    FinalizeProgramContext(&ProgramContext);
    MPI_Finalize();
    
	return 0;
}