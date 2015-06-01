#include "TweetParsing.h"

unsigned int GetTweetCount(PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Buffer = 0;
	unsigned int TweetCount = 0;
	
	while((Buffer = fgetc(TweetParsingContext->File)) != EOF)
	{
		if(Buffer == '\n')
		{
			TweetCount++;
		}
	}
	
	rewind(TweetParsingContext->File);
	
	return TweetCount;
}

int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	TWEET_PARSING_CONTEXT TweetParsingContext;
	
	TweetParsingContext.File = fopen(ProgramContext->Filename, "r");
	if(TweetParsingContext.File == NULL)
	{
        printf("Node: %i failed to open File\n", ProgramContext->NodeContext.NodeID);
		return 2;
	}
	
	//Get Tweet Count if none were specified
	if(ProgramContext->TweetCount == 0)
	{
		for(int i = 0; i < 30; i++)
		{
			ProgramContext->TweetCount = GetTweetCount(&TweetParsingContext);
		}		
	}
	
	//Allocate Space
	
	//Do Parsing
	
	fclose(TweetParsingContext.File);
	
	return 0;
}