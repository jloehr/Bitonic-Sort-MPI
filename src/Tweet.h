#pragma once

#include "Int.h"
#include <stdio.h>
#include <wchar.h>

//#define UNICODE_APPEARANCE_ARRAY
//#define SAVE_TWEET_POSITION

#define MAX_TWEET_CHARACTER_COUNT 450

typedef struct _UNICODE_APPEARANCE
{
	wchar_t UnicodeCharacter;
	uint16_t NumberOfAppearance;	
	
} UNICODE_APPEARANCE, * PUNICODE_APPEARANCE;

typedef struct _TWEET {
	
	uint8_t SearchTermValue;
	uint8_t Size;
	uint16_t NumberOfDifferentUnicodes;
#ifdef UNICODE_APPEARANCE_ARRAY
	UNICODE_APPEARANCE UnicodeAppearance[MAX_TWEET_CHARACTER_COUNT];
#else
	UNICODE_APPEARANCE * UnicodeAppearance;
#endif

#ifdef SAVE_TWEET_POSITION
	uint8_t FileID;
	uint64_t PositionInFile;
#else
	wchar_t * Tweet;
#endif	
	
} TWEET, * PTWEET;

void PrintTweet(PTWEET Tweet);
void PrintTweetDebugInfoToStream(FILE * Stream, PTWEET Tweet);
void CleanUpTweet(PTWEET Tweet);

int CompareTweets(const void * a, const void * b);