#pragma once

#include <stdio.h>

#include "Program.h"

typedef struct _TWEET_PARSING_CONTEXT 
{
	FILE * File;
	uint32_t FileID;
	
} TWEET_PARSING_CONTEXT, * PTWEET_PARSING_CONTEXT;

int ParseTweets(PPROGRAM_CONTEXT ProgramContext);