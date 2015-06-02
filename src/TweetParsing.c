#include "TweetParsing.h"

#include <stdlib.h>

#include "Tweet.h"

int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	TWEET_PARSING_CONTEXT TweetParsingContext;
	
	//Tweets Per Node
	uint64_t TotalAmountOfTweets = ProgramContext->TweetsPerFile * ProgramContext->NumberOfFiles;
	ProgramContext->NodeContext.ElementsPerNode = TotalAmountOfTweets/ProgramContext->NodeContext.NumberOfNodes;
	
	//Allocate Space
	ProgramContext->NodeContext.Data = malloc(ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET));
	
	if(IsMasterNode(&(ProgramContext->NodeContext)))
	{
        printf("Total Amount of Tweets: %llu\n", TotalAmountOfTweets);
        printf("Tweets Per Node: %i\n", ProgramContext->NodeContext.ElementsPerNode);
		printf("Allocated Space: %lu\n", ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET));
	}
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		printf("Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return 2;
	}
	
	return 0;
}
