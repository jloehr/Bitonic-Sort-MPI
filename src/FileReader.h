#pragma once

#include <stdio.h>
#include <wchar.h>

#include "ForwardDeclarations.h"
#include "Tweet.h"

typedef struct _FILE_READER_CONTEXT
{
	FILE * File;
	wchar_t TweetBuffer[MAX_TWEET_CHARACTER_COUNT];
	
} FILE_READER_CONTEXT, * PFILE_READER_CONTEXT;

int ReadInTweets(PPROGRAM_CONTEXT ProgramContext);