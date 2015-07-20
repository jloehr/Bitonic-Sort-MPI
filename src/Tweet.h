#pragma once

#include "Int.h"
#include <stdio.h>
#include <wchar.h>

#include "ForwardDeclarations.h"

typedef struct _UNICODE_APPEARANCE
{
	wchar_t UnicodeCharacter;
	uint16_t NumberOfAppearance;	
	
} UNICODE_APPEARANCE;

typedef struct _TWEET 
{	
	uint8_t SearchTermValue;
	uint8_t Size;
	UNICODE_APPEARANCE SmallestUnicode;

	uint64_t TweetStringOffset;
	
} TWEET;

int InitMPITweetType(PPROGRAM_CONTEXT ProgramContext);

void PrintTweet(PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);
void PrintTweetDebugInfoToStream(FILE * Stream, PPROGRAM_CONTEXT ProgramContext, PTWEET Tweet);

int CompareTweetsDescQsort(const void * a, const void * b);
int CompareTweetsAscQsort(const void * a, const void * b);

int CompareTweetsDesc(const void * a, const void * b, PPROGRAM_CONTEXT ProgramContext);
int CompareTweetsAsc(const void * a, const void * b, PPROGRAM_CONTEXT ProgramContext);