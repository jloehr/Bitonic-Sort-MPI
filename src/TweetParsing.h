#pragma once

#include "Int.h"
#include <wchar.h>

#include "ForwardDeclarations.h"
#include "Tweet.h"

typedef struct _TWEET_PARSING_CONTEXT 
{
	uint64_t CurrentTweetNumber;
	uint64_t NodeTweetNumberStart;
	uint64_t NodeTweetNumberEnd;
	wchar_t * ReadPointer;
	PTWEET CurrentTweet;
		
} TWEET_PARSING_CONTEXT, * PTWEET_PARSING_CONTEXT;

int ParseTweets(PPROGRAM_CONTEXT ProgramContext);