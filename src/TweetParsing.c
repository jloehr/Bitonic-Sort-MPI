#include "TweetParsing.h"

#include <stdlib.h>

#include "Tweet.h"

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext);
int ParseFiles(PPROGRAM_CONTEXT ProgramContext);

int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	int Result = 0;
	
	Result = AllocateMemory(ProgramContext);	
	if(Result != 0)
	{
		return Result;
	}
		
	Result = ParseFiles(ProgramContext);
	if(Result != 0)
	{
		return Result;
	}
	
	return Result;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext)
{
	//Tweets Per Node
	uint64_t TotalAmountOfTweets = ProgramContext->TweetsPerFile * ProgramContext->NumberOfFiles;
	
	ProgramContext->NodeContext.ElementsPerNode = TotalAmountOfTweets/ProgramContext->NodeContext.NumberOfNodes;
	
	uint64_t DataSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
	
	//Allocate Space
	ProgramContext->NodeContext.Data = malloc(DataSize);
	
	if(IsMasterNode(&(ProgramContext->NodeContext)))
	{
        printf("Total Amount of Tweets: %llu\n", TotalAmountOfTweets);
        printf("Tweets Per Node: %i\n", ProgramContext->NodeContext.ElementsPerNode);
		printf("Allocated Space: %llu\n", DataSize);
	}
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		printf("Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return 2;
	}
	
	return 0;
}

int ParseFiles(PPROGRAM_CONTEXT ProgramContext)
{
	uint64_t StartingTweetID = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	uint64_t EndTweetID = StartingTweetID + ProgramContext->NodeContext.ElementsPerNode - 1;
	
	uint64_t FirstFileID = StartingTweetID / ProgramContext->TweetsPerFile;
	uint64_t LastFileID = EndTweetID / ProgramContext->TweetsPerFile;
	
	for(uint64_t FileID = FirstFileID; FileID <= LastFileID; FileID++)
	{
		uint64_t StartingLine = (FileID == FirstFileID) ? (StartingTweetID - (FirstFileID * ProgramContext->TweetsPerFile)) : 0 ;
		uint64_t LastLine = (FileID == LastFileID) ? (EndTweetID - (LastFileID * ProgramContext->TweetsPerFile)) : ProgramContext->TweetsPerFile - 1;
		
		printf("Node %i: Parsing File %llu from %llu to %llu\n", ProgramContext->NodeContext.NodeID, FileID, StartingLine, LastLine);
		
		//Parse File ( Start, End)
	}
	
	return 0;
}
