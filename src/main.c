#include <mpi.h>
#include <stdio.h>

#include <locale.h> 

#include "Program.h"
#include "Node.h"
#include "TweetParsing.h"

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


    //Parse File
    Status = ParseTweets(&ProgramContext);
    if(Status != 0)
    {
    	return Status;
    }
    
    //Start Bitonic Sort
    
    
    FinalizeProgramContext(&ProgramContext);

    MPI_Finalize();
	return 0;
}