#include "Tweet.h"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

void PrintTweet(PTWEET Tweet)
{
	PrintTweetDebugInfoToStream(stdout, Tweet);
#ifdef SAVE_TWEET_POSITION
	printf("\n");
#else
	wprintf(L"%S\n", Tweet->Tweet);
#endif	
}

void PrintTweetDebugInfoToStream(FILE * Stream, PTWEET Tweet)
{
	//Position in File
#ifdef SAVE_TWEET_POSITION
	fwprintf(Stream, L"(%u)%12llu\t", Tweet->FileID, Tweet->PositionInFile);
#endif

	fwprintf(Stream, L"%3u\t%2u\t", Tweet->Size, Tweet->SearchTermValue);
	
	for(PUNICODE_APPEARANCE UnicodeAppearance = Tweet->UnicodeAppearance; UnicodeAppearance != (Tweet->UnicodeAppearance + Tweet->NumberOfDifferentUnicodes); UnicodeAppearance++)
	{
		fwprintf(Stream, L"%4x: %3u\t", UnicodeAppearance->UnicodeCharacter, UnicodeAppearance->NumberOfAppearance);
	}
}

void CleanUpTweet(PTWEET Tweet)
{
#ifndef UNICODE_APPEARANCE_ARRAY
	if(Tweet->UnicodeAppearance != NULL)
	{
		free(Tweet->UnicodeAppearance);
		Tweet->UnicodeAppearance = NULL;
	}
#endif

#ifndef SAVE_TWEET_POSITION
	if(Tweet->Tweet != NULL)
	{
		free(Tweet->Tweet);
		Tweet->Tweet = NULL;
	}
#endif
}

int CompareTweets(const void * a, const void * b)
{	
	const TWEET * A = a;
	const TWEET * B = b;
	
	if(A->SearchTermValue != B->SearchTermValue)
	{
		return B->SearchTermValue - A->SearchTermValue;
	}
	else
	{
		uint8_t MinIndex = A->NumberOfDifferentUnicodes < B->NumberOfDifferentUnicodes ? A->NumberOfDifferentUnicodes : B->NumberOfDifferentUnicodes;	
		
		PUNICODE_APPEARANCE UnicodeAppearanceA, UnicodeAppearanceB;
		
		for(UnicodeAppearanceA = A->UnicodeAppearance, UnicodeAppearanceB = B->UnicodeAppearance; 
			UnicodeAppearanceA != (A->UnicodeAppearance + MinIndex);
			UnicodeAppearanceA++, UnicodeAppearanceB++)
		{
			if(UnicodeAppearanceA->UnicodeCharacter != UnicodeAppearanceB->UnicodeCharacter)
			{
				return UnicodeAppearanceA->UnicodeCharacter - UnicodeAppearanceB->UnicodeCharacter;
			}
			else if(UnicodeAppearanceA->NumberOfAppearance != UnicodeAppearanceB->NumberOfAppearance)
			{
				return UnicodeAppearanceB->NumberOfAppearance - UnicodeAppearanceA->NumberOfAppearance;
			}
		}
		
		return 	B->NumberOfDifferentUnicodes - A->NumberOfDifferentUnicodes;
	}
}