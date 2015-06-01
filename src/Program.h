#pragma once

#include "Node.h"

typedef struct _PROGRAM_CONTEXT 
{
	const char * Filename;
	unsigned int TweetCount;
	
	NODE_CONTEXT NodeContext;

} PROGRAM_CONTEXT, * PPROGRAM_CONTEXT;

int InitProgramContext(PPROGRAM_CONTEXT ProgramContext, int argc, char * argv[]);