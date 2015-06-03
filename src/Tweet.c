#include "Tweet.h"

#include <stdio.h>

void PrintTweet(PTWEET Tweet)
{
	wchar_t SmallestUnicode = Tweet->UnicodeAppearance[0].UnicodeCharacter;
	uint8_t SmallestUnicodeCount = Tweet->UnicodeAppearance[0].NumberOfAppearance;
	printf("%llu\t%u\t%u\t%x - %u\n", Tweet->PositionInFile, Tweet->Size, Tweet->SearchTermValue, SmallestUnicode, SmallestUnicodeCount );
}