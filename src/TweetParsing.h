#pragma once

#include "Int.h"
#include <wchar.h>

#include "ForwardDeclarations.h"
#include "Tweet.h"

typedef struct _TWEET_PARSING_CONTEXT 
{
	uint16_t NumberOfDifferentUnicodes;
	UNICODE_APPEARANCE UnicodeAppearance[MAX_TWEET_CHARACTER_COUNT];
		
} TWEET_PARSING_CONTEXT, * PTWEET_PARSING_CONTEXT;

int ParseTweets(PPROGRAM_CONTEXT ProgramContext);