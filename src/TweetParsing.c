#include "TweetParsing.h"

#include <stdlib.h>
#include <wchar.h>
#include <stdio.h>
#include <string.h>

#include "ErrorCodes.h"

#include "Tweet.h"
#include "Program.h"

static int AllocateMemory(PPROGRAM_CONTEXT ProgramContext);
static int ParseTweetStrings(PPROGRAM_CONTEXT ProgramContext);
static int ParseTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext, PWSTRING TweetString, PTWEET Tweet, uint64_t TweetID);
static void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET_PARSING_CONTEXT Tweet);

int ParseTweets(PPROGRAM_CONTEXT ProgramContext)
{	
	int Result = NO_ERROR;
	
	Result = AllocateMemory(ProgramContext);	
	if(Result != NO_ERROR)
	{
		return Result;
	}
		
	Result = ParseTweetStrings(ProgramContext);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	return Result;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext)
{
	uint64_t DataSize = ProgramContext->NodeContext.ElementsPerNode * sizeof(TWEET);
	
	//Allocate Space
	ProgramContext->NodeContext.Data = malloc(DataSize);
	ProgramContext->NodeContext.BenchmarkContext.TweetDataMemory = DataSize;
	
	if(ProgramContext->NodeContext.Data == NULL)
	{
		wprintf(L"Node %i: failed to allocate Data Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	return NO_ERROR;
}

int ParseTweetStrings(PPROGRAM_CONTEXT ProgramContext)
{
	int Result = NO_ERROR;
	TWEET_PARSING_CONTEXT TweetParsingContext;
	
	uint64_t StartingTweetID = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	uint64_t TweetID = StartingTweetID;
	
	PWSTRING Tweet = ProgramContext->Tweets + TweetID; 
	
	wprintf(L"Node %i: Parsing %"PRIu64" Tweets \n", ProgramContext->NodeContext.NodeID, ProgramContext->NodeContext.ElementsPerNode);
		
	for(PTWEET DataPointer = ProgramContext->NodeContext.Data; DataPointer != ProgramContext->NodeContext.Data + ProgramContext->NodeContext.ElementsPerNode; DataPointer++,TweetID++,Tweet++)
	{
		Result = ParseTweet(ProgramContext, &TweetParsingContext, Tweet, DataPointer, TweetID);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	wprintf(L"Node %i: Done Parsing Tweets \n", ProgramContext->NodeContext.NodeID);
	
	return Result;
}

int ParseTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext, PWSTRING TweetString, PTWEET Tweet, uint64_t TweetID)
{
	const wchar_t * SearchTermPointer = ProgramContext->SearchTerm;
	Tweet->Size = 0;
	Tweet->SearchTermValue = 0;
	Tweet->NumberOfDifferentUnicodes = 0;
	Tweet->TweetStringID = TweetID;
	
	TweetParsingContext->NumberOfDifferentUnicodes = 0;
	
	for(wchar_t * ReadPointer = (*TweetString); (*ReadPointer) != '\0'; ReadPointer++)
	{
		//Unicode Appearance
		AddCharacterToUnicodeAppearance((*ReadPointer), TweetParsingContext);
		
		//SearchTerm Substring
		//If unequal out pointer to first position so it is checked against first search term character
		if((*SearchTermPointer) != (*ReadPointer))
		{
			SearchTermPointer = ProgramContext->SearchTerm;
		}
		
		if((*SearchTermPointer) == (*ReadPointer))
		{
			SearchTermPointer++;
			if((*SearchTermPointer) == '\0')
			{
				SearchTermPointer = ProgramContext->SearchTerm;
				Tweet->SearchTermValue++;
			}
		}	
		
		Tweet->Size++;
	}
	
	//Copy Unicode Apperance Array
	Tweet->NumberOfDifferentUnicodes = TweetParsingContext->NumberOfDifferentUnicodes;
	uint64_t NumberOfBytes = TweetParsingContext->NumberOfDifferentUnicodes * sizeof(UNICODE_APPEARANCE);
	Tweet->UnicodeAppearance = malloc(NumberOfBytes);
	if(Tweet->UnicodeAppearance == NULL)
	{
		wprintf(L"Error on Node %i: Out of Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_OUT_OF_MEMORY;
	}
	memcpy(Tweet->UnicodeAppearance, TweetParsingContext->UnicodeAppearance, NumberOfBytes);

	ProgramContext->NodeContext.BenchmarkContext.UnicodeAppearanceMemory += NumberOfBytes;
	ProgramContext->NodeContext.BenchmarkContext.UnicodeAppearanceFields += TweetParsingContext->NumberOfDifferentUnicodes;
	
	return NO_ERROR;
}

void AddCharacterToUnicodeAppearance(wint_t ReadChar, PTWEET_PARSING_CONTEXT Tweet)
{
	for(uint32_t i = 0; i < Tweet->NumberOfDifferentUnicodes; i++)
	{
		PUNICODE_APPEARANCE UnicodeAppearance = &(Tweet->UnicodeAppearance[i]);
		
		//If the exact Unicode is already in the List, increase its Appearance Number
		if(UnicodeAppearance->UnicodeCharacter == ReadChar)
		{
			UnicodeAppearance->NumberOfAppearance++;
			return;
		}	
		
		//Insert a new entry into the List, when the new Character is smaller than another
		if(UnicodeAppearance->UnicodeCharacter > ReadChar)
		{
			memmove(UnicodeAppearance + 1, UnicodeAppearance, (Tweet->NumberOfDifferentUnicodes - i) * sizeof(UNICODE_APPEARANCE));
			
			UnicodeAppearance->UnicodeCharacter = ReadChar;
			UnicodeAppearance->NumberOfAppearance = 1;
			return;
		}
	}

	//If no entry was found and it is the biggest Unicode so far, append it
	PUNICODE_APPEARANCE UnicodeAppearance = &(Tweet->UnicodeAppearance[Tweet->NumberOfDifferentUnicodes]);
	Tweet->NumberOfDifferentUnicodes++;
	
	UnicodeAppearance->UnicodeCharacter = ReadChar;
	UnicodeAppearance->NumberOfAppearance = 1;
}