#include "TweetParsing.h"

#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>

#include "ErrorCodes.h"

#include "Tweet.h"
#include "Program.h"

static int InitTweetParsingContext(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int AllocateMemory(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int ParseTweetStrings(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int ParseTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int SkipTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);

int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	int Result = NO_ERROR;
	TWEET_PARSING_CONTEXT TweetParsingContext;
	
	Result = InitTweetParsingContext(ProgramContext, &TweetParsingContext);	
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	Result = AllocateMemory(ProgramContext, &TweetParsingContext);	
	if(Result != NO_ERROR)
	{
		return Result;
	}
		
	Result = ParseTweetStrings(ProgramContext, &TweetParsingContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	return Result;
}

int InitTweetParsingContext(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{	
	TweetParsingContext->ReadPointer = NULL;
	
	TweetParsingContext->CurrentTweetNumber = 0;
	TweetParsingContext->NodeTweetNumberStart = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	TweetParsingContext->NodeTweetNumberEnd = TweetParsingContext->NodeTweetNumberStart + ProgramContext->NodeContext.ElementsPerNode;
	TweetParsingContext->CurrentTweet = NULL;	
	
	return NO_ERROR;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{	
	//Alloctate Size for the Tweet Data
	uint64_t TweetDataSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
	
	//Allocate Space
	ProgramContext->NodeContext.Data = malloc(TweetDataSize);
	ProgramContext->NodeContext.BenchmarkContext.TweetDataMemory = TweetDataSize;
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		wprintf(L"Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	return NO_ERROR;
}

int ParseTweetStrings(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	TweetParsingContext->ReadPointer = ProgramContext->TweetStrings;
	TweetParsingContext->CurrentTweet = ProgramContext->NodeContext.Data;

	wprintf(L"Node %i: Parsing %"PRIu64" Tweets \n", ProgramContext->NodeContext.NodeID, ProgramContext->NodeContext.ElementsPerNode);
	
	
	for(; TweetParsingContext->CurrentTweetNumber < TweetParsingContext->NodeTweetNumberStart; TweetParsingContext->CurrentTweetNumber++)
	{
		Result = SkipTweet(ProgramContext, TweetParsingContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	for(; TweetParsingContext->CurrentTweetNumber < TweetParsingContext->NodeTweetNumberEnd; TweetParsingContext->CurrentTweetNumber++)
	{
		Result = ParseTweet(ProgramContext, TweetParsingContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	for(; TweetParsingContext->CurrentTweetNumber < ProgramContext->TotalAmountOfTweets; TweetParsingContext->CurrentTweetNumber++)
	{
		Result = SkipTweet(ProgramContext, TweetParsingContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	wprintf(L"Node %i: Done Parsing Tweets \n", ProgramContext->NodeContext.NodeID);
	
	return Result;
}

int ParseTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	const wchar_t * SearchTermPointer = ProgramContext->SearchTerm;
	
	//Set TweetStringOffset
	TweetParsingContext->CurrentTweet->TweetStringOffset = TweetParsingContext->ReadPointer - ProgramContext->TweetStrings;
	TweetParsingContext->CurrentTweet->SearchTermValue = 0;
	
	for(wchar_t CurrentCharachter = (*TweetParsingContext->ReadPointer);
		CurrentCharachter != '\0'; 
		CurrentCharachter = (*++TweetParsingContext->ReadPointer))
	{
		//SearchTerm Substring
		//If unequal out pointer to first position so it is checked against first search term character
		if((*SearchTermPointer) != CurrentCharachter)
		{
			SearchTermPointer = ProgramContext->SearchTerm;
		}
		
		if((*SearchTermPointer) == CurrentCharachter)
		{
			SearchTermPointer++;
			if((*SearchTermPointer) == '\0')
			{
				SearchTermPointer = ProgramContext->SearchTerm;
				TweetParsingContext->CurrentTweet->SearchTermValue++;
			}
		}	
		
		TweetParsingContext->CurrentTweet->Size++;
	}
	
	TweetParsingContext->CurrentTweet++;
	
	//Skip that null terminator
	TweetParsingContext->ReadPointer++;
	
	return Result;
}

int SkipTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	
	for(wchar_t CurrentCharachter = (*TweetParsingContext->ReadPointer);
		CurrentCharachter != '\0'; 
		CurrentCharachter = (*++TweetParsingContext->ReadPointer));

	//Skip that null terminator
	TweetParsingContext->ReadPointer++;
	
	return Result;
}
