#pragma once

#include "Int.h"
#include <stdio.h>
#include <wchar.h>

#include "ForwardDeclarations.h"

#define MAX_TWEET_CHARACTER_COUNT 512

typedef struct _UNICODE_APPEARANCE
{
	wchar_t UnicodeCharacter;
	uint16_t NumberOfAppearance;	
	
} UNICODE_APPEARANCE, * PUNICODE_APPEARANCE;

typedef struct _TWEET {
	
	uint8_t SearchTermValue;
	uint8_t Size;
	uint16_t NumberOfDifferentUnicodes;
	UNICODE_APPEARANCE * UnicodeAppearance;

	uint64_t TweetStringID;
	
} TWEET, * PTWEET;

void PrintTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);
void PrintTweetDebugInfoToStream(FILE * Stream, PTWEET Tweet);
void CleanUpTweet(PTWEET Tweet);

int CompareTweets(const void * a, const void * b);