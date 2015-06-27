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
	uint64_t UnicodeAppearanceOffset;

	uint64_t TweetStringOffset;
	
} TWEET, * PTWEET;

void PrintTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);
void PrintTweetDebugInfoToStream(FILE * Stream, PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);

int CompareTweets(const void * a, const void * b);