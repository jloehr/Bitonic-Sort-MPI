#include "Program.h"

#include <stdlib.h>

#include "ArgumentParsing.h"

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[])
{
	int Result = 0;
	
	InitNodeContext(&(ProgramContext->NodeContext));
	
	ProgramContext->Filename = NULL;
	ProgramContext->NumberOfFiles = 0;
	ProgramContext->TweetsPerFile = 0;
	ProgramContext->TotalAmountOfTweets = 0;
	ProgramContext->SearchTerm = NULL;
	ProgramContext->Tweets = NULL;
	
	Result = ParseArguments(argc, argv, ProgramContext);
	
	//Tweets Per Node
	ProgramContext->TotalAmountOfTweets = ProgramContext->TweetsPerFile * ProgramContext->NumberOfFiles;
	ProgramContext->NodeContext.ElementsPerNode = ProgramContext->TotalAmountOfTweets/ProgramContext->NodeContext.NumberOfNodes;
	
	return Result;
}

void FinalizeProgramContext(PPROGRAM_CONTEXT ProgramContext)
{
	FinalizeNodeContext(&(ProgramContext->NodeContext));
	
	//Free the Search Term String
	if(ProgramContext->SearchTerm != NULL)
	{
		free((void *)ProgramContext->SearchTerm);
		ProgramContext->SearchTerm = NULL;
	}
	
	//Free the Tweet Strings
	if(ProgramContext->Tweets != NULL)
	{
		for(PWSTRING DataPointer = ProgramContext->Tweets; DataPointer != ProgramContext->Tweets + ProgramContext->TotalAmountOfTweets; DataPointer++)
		{
			free(*DataPointer);
		}
		
		free(ProgramContext->Tweets);
		ProgramContext->Tweets = NULL;
	}
}