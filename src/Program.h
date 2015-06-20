#pragma once

#include "Int.h"
#include <wchar.h>

#include "ForwardDeclarations.h"

#include "Node.h"

typedef wchar_t * PWSTRING;

typedef struct _PROGRAM_CONTEXT 
{
	const char * Filename;
	uint64_t NumberOfFiles;
	uint64_t TweetsPerFile;
	uint64_t TotalAmountOfTweets;
	const wchar_t * SearchTerm;
	
	uint64_t PageSize;
	
	uint64_t TweetStringsSize;
	PWSTRING TweetStrings;
	NODE_CONTEXT NodeContext;
	
} PROGRAM_CONTEXT, * PPROGRAM_CONTEXT;

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[]);
void FinalizeProgramContext(PPROGRAM_CONTEXT ProgramContext);