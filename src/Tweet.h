#pragma once

#include <stdint.h>
#include <wchar.h>

#define MAX_TWEET_CHARACTER_COUNT 200

typedef struct _UNICODE_APPEARANCE
{
	wchar_t UnicodeCharacter;
	uint8_t NumberOfAppearance;	
	
} UNICODE_APPEARANCE, * PUNICODE_APPEARANCE;

typedef struct _TWEET {
	
	uint8_t SearchTermValue;
	uint8_t Size;
	uint8_t NumberOfDifferentUnicodes;
	UNICODE_APPEARANCE UnicodeAppearance[MAX_TWEET_CHARACTER_COUNT];
	
	uint8_t FileID;
	uint64_t PositionInFile;
	
} TWEET, * PTWEET;

void PrintTweet(PTWEET Tweet);
