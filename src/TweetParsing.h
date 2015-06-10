#pragma once

#include <stdio.h>

#include "Program.h"

typedef struct _TWEET_PARSING_CONTEXT 
{
	FILE * File;
	uint32_t FileID;
	
	uint16_t NumberOfDifferentUnicodes;
	UNICODE_APPEARANCE UnicodeAppearance[MAX_TWEET_CHARACTER_COUNT];
	
	
#ifndef SAVE_TWEET_POSITION
	wchar_t TweetBuffer[MAX_TWEET_CHARACTER_COUNT];
#endif	
	
} TWEET_PARSING_CONTEXT, * PTWEET_PARSING_CONTEXT;

int ParseTweets(PPROGRAM_CONTEXT ProgramContext);