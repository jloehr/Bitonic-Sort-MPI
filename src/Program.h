#pragma once

#include "Int.h"
#include <wchar.h>
#include <mpi.h>
#include <time.h>

#include "ForwardDeclarations.h"

#include "Node.h"

typedef wchar_t * PWSTRING;

typedef struct _PROGRAM_CONTEXT 
{
	const char * Filename;
	const char * OutputDir;
	uint64_t NumberOfFiles;
	uint64_t TweetsPerFile;
	uint64_t TotalAmountOfTweets;
	uint16_t MaxTweetSize;
	const wchar_t * SearchTerm;
	time_t StartTime;
	
	uint64_t PageSize;
	
	MPI_Datatype MPITweetType;
	
	uint64_t TweetStringsSize;
	PWSTRING TweetStrings;
	NODE_CONTEXT NodeContext;
	
} PROGRAM_CONTEXT;

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[]);
void FinalizeProgramContext(PPROGRAM_CONTEXT ProgramContext);
