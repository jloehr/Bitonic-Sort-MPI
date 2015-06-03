#include "Tweet.h"

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

}