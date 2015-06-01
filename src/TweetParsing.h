#pragma once

#include <stdio.h>
#include <wchar.h>

#include "Program.h"

#define TWEET_PARSING_BUFFER_SIZE 280

typedef struct _TWEET_PARSING_CONTEXT 
{
	FILE * File;
	wchar_t Buffer[TWEET_PARSING_BUFFER_SIZE];
	
} TWEET_PARSING_CONTEXT, * PTWEET_PARSING_CONTEXT;

int ParseTweets(PPROGRAM_CONTEXT ProgramContext);