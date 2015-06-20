#pragma once

#include "Int.h"
#include <stdio.h>
#include <wchar.h>

#include "ForwardDeclarations.h"
#include "Program.h"

typedef struct _FILE_READER_CONTEXT
{
	FILE * File;
	uint64_t CurrentTweetNumber;
	uint64_t NodeTweetNumberStart;
	uint64_t NodeTweetNumberEnd;
	void * EndOfBlock;
	PWSTRING WritePointer;
	PTWEET CurrentTweet;
	
} FILE_READER_CONTEXT, * PFILE_READER_CONTEXT;

int ReadInTweets(PPROGRAM_CONTEXT ProgramContext);