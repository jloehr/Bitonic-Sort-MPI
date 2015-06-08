#include "Tweet.h"

#include <stdlib.h>
#include <stdio.h>
#include <wchar.h>

void PrintTweet(PTWEET Tweet)
{
	wchar_t SmallestUnicode = Tweet->UnicodeAppearance[0].UnicodeCharacter;
	uint8_t SmallestUnicodeCount = Tweet->UnicodeAppearance[0].NumberOfAppearance;
	
#ifdef SAVE_TWEET_POSITION
	printf("%llu\t%u\t%u\t%x - %u\n", Tweet->PositionInFile, Tweet->Size, Tweet->SearchTermValue, SmallestUnicode, SmallestUnicodeCount);
#else
	wprintf(L"%u\t%u\t%x - %u\t%S\n", Tweet->Size, Tweet->SearchTermValue, SmallestUnicode, SmallestUnicodeCount, Tweet->Tweet);
#endif
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