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
static int AllocateUnicodeAppearanceMemory(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int ParseTweetStrings(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int ParseTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int ParseTweetForUnicodeAppearance(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int AddCharacterToUnicodeAppearance(wint_t ReadChar, PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext);
static int IncreaseCurrentUnicodeAppearanceTable(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext, PUNICODE_APPEARANCE * CurrentUnicodeAppearancePointer);

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
	TweetParsingContext->CurrentNumberOfDifferentUnicodes = 0;
	TweetParsingContext->EndOfBlock = NULL;
	TweetParsingContext->CurrentUnicodeAppearanceStart = NULL;
	TweetParsingContext->ReadPointer = NULL;
	
	TweetParsingContext->CurrentTweetNumber = 0;
	TweetParsingContext->NodeTweetNumberStart = ProgramContext->NodeContext.NodeID * ProgramContext->NodeContext.ElementsPerNode;
	TweetParsingContext->NodeTweetNumberEnd = TweetParsingContext->NodeTweetNumberStart + ProgramContext->NodeContext.ElementsPerNode;
	TweetParsingContext->CurrentTweet = NULL;	
	
	return NO_ERROR;
}

int AllocateMemory(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	
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
	
	//Allocate Starting Space for the Unicode Appearances
	Result = AllocateUnicodeAppearanceMemory(ProgramContext, TweetParsingContext);	
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	return NO_ERROR;
}

int AllocateUnicodeAppearanceMemory(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	uint64_t CurrentWritePosition = TweetParsingContext->CurrentUnicodeAppearanceStart - ProgramContext->UnicodeAppearances;
	ProgramContext->UnicodeAppearancesSize += ProgramContext->PageSize;
	void * Result = realloc(ProgramContext->UnicodeAppearances, ProgramContext->UnicodeAppearancesSize);
	
	if(Result == NULL)
	{
		wprintf(L"Node %i: failed to allocate UnicodeAppearances Array Memory\n", ProgramContext->NodeContext.NodeID);
		return ERROR_NOT_ENOUGH_MEMORY;
	}
	
	ProgramContext->UnicodeAppearances = Result;
	TweetParsingContext->CurrentUnicodeAppearanceStart = ProgramContext->UnicodeAppearances + CurrentWritePosition;
	TweetParsingContext->EndOfBlock = ((void *)ProgramContext->UnicodeAppearances) + ProgramContext->UnicodeAppearancesSize;
	
	return NO_ERROR;
}

int ParseTweetStrings(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	TweetParsingContext->ReadPointer = ProgramContext->TweetStrings;
	TweetParsingContext->CurrentUnicodeAppearanceStart = ProgramContext->UnicodeAppearances;
	TweetParsingContext->CurrentTweet = ProgramContext->NodeContext.Data;

	wprintf(L"Node %i: Parsing %"PRIu64" Tweets \n", ProgramContext->NodeContext.NodeID, ProgramContext->NodeContext.ElementsPerNode);
	
	
	for(; TweetParsingContext->CurrentTweetNumber < TweetParsingContext->NodeTweetNumberStart; TweetParsingContext->CurrentTweetNumber++)
	{
		Result = ParseTweetForUnicodeAppearance(ProgramContext, TweetParsingContext);
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
		Result = ParseTweetForUnicodeAppearance(ProgramContext, TweetParsingContext);
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
		//Unicode Appearance
		Result = AddCharacterToUnicodeAppearance(CurrentCharachter, ProgramContext, TweetParsingContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
		
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
	
	TweetParsingContext->CurrentTweet->NumberOfDifferentUnicodes = TweetParsingContext->CurrentNumberOfDifferentUnicodes;
	TweetParsingContext->CurrentTweet->UnicodeAppearanceOffset = TweetParsingContext->CurrentUnicodeAppearanceStart - ProgramContext->UnicodeAppearances;
	TweetParsingContext->CurrentTweet++;
	
	TweetParsingContext->CurrentUnicodeAppearanceStart += TweetParsingContext->CurrentNumberOfDifferentUnicodes;
	TweetParsingContext->CurrentNumberOfDifferentUnicodes = 0;
	
	//Skip that null terminator
	TweetParsingContext->ReadPointer++;
	
	return Result;
}

int ParseTweetForUnicodeAppearance(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	
	for(wchar_t CurrentCharachter = (*TweetParsingContext->ReadPointer);
		CurrentCharachter != '\0'; 
		CurrentCharachter = (*++TweetParsingContext->ReadPointer))
	{
		//Unicode Appearance
		Result = AddCharacterToUnicodeAppearance(CurrentCharachter, ProgramContext, TweetParsingContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
	}
	
	TweetParsingContext->CurrentUnicodeAppearanceStart += TweetParsingContext->CurrentNumberOfDifferentUnicodes;
	TweetParsingContext->CurrentNumberOfDifferentUnicodes = 0;
	
	//Skip that null terminator
	TweetParsingContext->ReadPointer++;
	
	return Result;
}

int AddCharacterToUnicodeAppearance(wint_t ReadChar, PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext)
{
	int Result = NO_ERROR;
	
	PUNICODE_APPEARANCE UnicodeAppearance = TweetParsingContext->CurrentUnicodeAppearanceStart;

	for(; 
		UnicodeAppearance < (TweetParsingContext->CurrentUnicodeAppearanceStart + TweetParsingContext->CurrentNumberOfDifferentUnicodes); 
		UnicodeAppearance++) 
	{	
		//If the exact Unicode is already in the List, increase its Appearance Number
		if(UnicodeAppearance->UnicodeCharacter == ReadChar)
		{
			UnicodeAppearance->NumberOfAppearance++;
			return NO_ERROR;
		}	
		
		//Insert a new entry into the List, when the new Character is smaller than another
		if(UnicodeAppearance->UnicodeCharacter > ReadChar)
		{
			Result = IncreaseCurrentUnicodeAppearanceTable(ProgramContext,TweetParsingContext, &UnicodeAppearance);
			if(Result != NO_ERROR)
			{
				return Result;
			}
			
			//	 0	 1	 2	 3
			// | a | c | d | e
			//
			// Start = 0
			// Number = 4
			//
			uint64_t UnicodeAppearanceFieldsToMove = (TweetParsingContext->CurrentUnicodeAppearanceStart + TweetParsingContext->CurrentNumberOfDifferentUnicodes) - UnicodeAppearance;
			
			memmove(UnicodeAppearance + 1, UnicodeAppearance, UnicodeAppearanceFieldsToMove * sizeof(UNICODE_APPEARANCE));
			
			UnicodeAppearance->UnicodeCharacter = ReadChar;
			UnicodeAppearance->NumberOfAppearance = 1;
			TweetParsingContext->CurrentNumberOfDifferentUnicodes++;
			return NO_ERROR;
		}
	}

	//If no entry was found and it is the biggest Unicode so far, append it
	Result = IncreaseCurrentUnicodeAppearanceTable(ProgramContext,TweetParsingContext, &UnicodeAppearance);
	if(Result != NO_ERROR)
	{
		return Result;
	}
	
	UnicodeAppearance->UnicodeCharacter = ReadChar;
	UnicodeAppearance->NumberOfAppearance = 1;
	TweetParsingContext->CurrentNumberOfDifferentUnicodes++;
	
	return NO_ERROR;
}

int IncreaseCurrentUnicodeAppearanceTable(PPROGRAM_CONTEXT ProgramContext, PTWEET_PARSING_CONTEXT TweetParsingContext, PUNICODE_APPEARANCE * CurrentUnicodeAppearancePointer)
{	
	while((void *)(TweetParsingContext->CurrentUnicodeAppearanceStart + TweetParsingContext->CurrentNumberOfDifferentUnicodes) > (TweetParsingContext->EndOfBlock - sizeof(UNICODE_APPEARANCE)))
	{
		uint64_t CurrentUnicodeAppearancePointerOffset = (*CurrentUnicodeAppearancePointer) - TweetParsingContext->CurrentUnicodeAppearanceStart;
		
		int Result = AllocateUnicodeAppearanceMemory(ProgramContext, TweetParsingContext);
		if(Result != NO_ERROR)
		{
			return Result;
		}
		
		(*CurrentUnicodeAppearancePointer) = TweetParsingContext->CurrentUnicodeAppearanceStart + CurrentUnicodeAppearancePointerOffset;
	}

	return NO_ERROR;
}
