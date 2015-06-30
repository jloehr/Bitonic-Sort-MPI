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

	uint64_t TweetStringOffset;
	
} TWEET, * PTWEET;

int InitMPITweetType(PPROGRAM_CONTEXT ProgramContext);

void PrintTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);
void PrintTweetDebugInfoToStream(FILE * Stream, PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);

int CompareTweetsDescQsort(const void * a, const void * b);
int CompareTweetsAscQsort(const void * a, const void * b);

int CompareTweetsDesc(const void * a, const void * b, PPROGRAM_CONTEXT ProgramContext);
int CompareTweetsAsc(const void * a, const void * b, PPROGRAM_CONTEXT ProgramContext);