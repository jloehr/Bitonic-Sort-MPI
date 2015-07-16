#include "Program.h"

#include <unistd.h>
#include <stdlib.h>

#include "ErrorCodes.h"
#include "ArgumentParsing.h"
#include "Tweet.h"

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[])
{
	int Result = 0;
	
	InitNodeContext(&(ProgramContext->NodeContext));
	
	ProgramContext->Filename = NULL;
	ProgramContext->OutputDir = NULL;
	ProgramContext->NumberOfFiles = 0;
	ProgramContext->TweetsPerFile = 0;
	ProgramContext->TotalAmountOfTweets = 0;
	ProgramContext->MaxTweetSize = 0;
	ProgramContext->SearchTerm = NULL;
	ProgramContext->TweetStringsSize = 0;
	ProgramContext->TweetStrings = NULL;
	
  	time (&ProgramContext->StartTime);
	
	Result = ParseArguments(argc, argv, ProgramContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	//Tweets Per Node
	ProgramContext->TotalAmountOfTweets = ProgramContext->TweetsPerFile * ProgramContext->NumberOfFiles;
	ProgramContext->NodeContext.ElementsPerNode = ProgramContext->TotalAmountOfTweets/ProgramContext->NodeContext.NumberOfNodes;
	
	ProgramContext->PageSize = getpagesize();
	if(ProgramContext->PageSize == 0)
	{
		ProgramContext->PageSize = 4*1024;
	}
	
	Result = InitMPITweetType(ProgramContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
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
	if(ProgramContext->TweetStrings != NULL)
	{
		free(ProgramContext->TweetStrings);
		ProgramContext->TweetStringsSize = 0;
		ProgramContext->TweetStrings = NULL;
	}
}